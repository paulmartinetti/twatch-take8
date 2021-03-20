/*
   WakeupFormSensor: Use BMA423 external interrupt to wake up T-Watch. By default,
   use wrist lift, double click to wake up
   Copyright 2020 Lewis he
*/
#include "config.h"

TTGOClass *watch;
TFT_eSPI *tft;
AXP20X_Class *power;
BMA *sensor;


// for api calls, json capacity
// currently used in getWeather, getFrenchExpression
// JSON size defined for largest of api call jsons (currently localeJson, see below)
// I started with the capacity value from JHershey's Oweather, which works.
// Then I copied the actual json from the serial monitor and pasted it here:
// https://arduinojson.org/v6/assistant/
size_t capacity = 1024;

uint8_t isAwake = 0;
// square area where the watch is flat and awake
uint8_t give = 100;


// timepiece
uint32_t targetTime = 0;       // for next 1 second display update
// uint32_t clockUpTime = 0;      // track the time the clock is displayed
uint8_t hh, mm, ss, mmonth, dday; // H, M, S variables
uint16_t yyear; // Year is 16 bit int

// all wakeup functions
void wakeFromLightSleep() {
  power->setPowerOutPut(AXP202_LDO2, true); // v1
  setCpuFrequencyMhz(160);
  watch->displayWakeup();
  watch->openBL();
  watch->rtc->syncToSystem();
}

// all light sleep functions to minimize power while still listening for wrist flick
void lightSleep() {
  power->setPowerOutPut(AXP202_LDO2, false); // v1, battery lasted 6hrs killing display only
  watch->closeBL();
  watch->displaySleep();
  setCpuFrequencyMhz(10);
  // listen for wake up - espressif, still looking for source document
  gpio_wakeup_enable ((gpio_num_t)BMA423_INT1, GPIO_INTR_LOW_LEVEL);
  esp_sleep_enable_gpio_wakeup();
  esp_light_sleep_start();
}

void setup() {

  // Get TTGOClass instance
  watch = TTGOClass::getWatch();

  Serial.begin(115200);
  // Initialize the hardware, the BMA423 sensor has been initialized internally
  watch->begin();

  // Turn on the backlight
  watch->openBL();

  //Receive objects for easy writing
  tft = watch->tft;
  power = watch->power;
  sensor = watch->bma;

  // Turn off unused power
  power->setPowerOutPut(AXP202_EXTEN, AXP202_OFF);
  power->setPowerOutPut(AXP202_DCDC2, AXP202_OFF);
  power->setPowerOutPut(AXP202_LDO3, AXP202_OFF);
  power->setPowerOutPut(AXP202_LDO4, AXP202_OFF);

  // battery % charge
  power->adc1Enable(AXP202_BATT_CUR_ADC1, true);

  // Accel parameter structure
  Acfg cfg;
  // default was 100Hz, but this is lowest that works
  cfg.odr = BMA4_OUTPUT_DATA_RATE_12_5HZ;
  // lot of Gs not needed, this is lowest setting
  cfg.range = BMA4_ACCEL_RANGE_2G;
  cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
  //cfg.perf_mode = BMA4_CONTINUOUS_MODE;
  cfg.perf_mode = BMA4_CIC_AVG_MODE;
  // Configure the BMA423 accelerometer
  sensor->accelConfig(cfg);

  // Enable BMA423 accelerometer
  sensor->enableAccel();

  // to sleep
  tft->setTextColor(TFT_CYAN);
  delay(1000);
  tft->setCursor(10, 100);
  tft->setTextSize(2);
  tft->println("To sleep now...");
  delay(1000);
  lightSleep();
}

void loop() {

  // typ Accel defined in bma.h, typedef struct bma4_accel Accel;
  Accel acc;
  // confirm sensor on
  bool res = sensor->getAccel(acc);

  if (!res) {
    Serial.println("getAccel FAIL");
  } else {

    int myY = acc.y;
    int myX = acc.x;

    // 1. WAKE ..very limited correct movement along y wakes up, note that !isAwake = 0
    if (!isAwake && (myX >= -give && myX <= give) && (myY >= -give && myY <= give)) {
      // (Documents > Arduino > T-Watch-Projects-main > 11 wakeup axp202 minimal
      // wake from lightSleep
      wakeFromLightSleep();
      displayTime(true);
      isAwake = 1;
    }
    // 2. TOUCH LISTEN - during 3s wake time,
    // when we write if(isAwake) that is asking if isAwake is true or >0
    if (isAwake) {

      // listen for touch only while awake
      int16_t x, y;
      if (watch->getTouch(x, y)) {
        //Serial.println("touched");
        wifiConnect();
      }
    }

    // 3. TO SLEEP - back to light sleep most of the time
    if (isAwake && (myX >= -give || myX <= give) && (myY < -give || myY > give)) {
      lightSleep();
      isAwake = 0;
    }

    delay(10);
  }
}
