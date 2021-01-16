/*
   WakeupFormSensor: Use BMA423 external interrupt to wake up T-Watch. By default,
   use wrist lift, double click to wake up
   Copyright 2020 Lewis he
*/

#include "config.h"
#include <soc/rtc.h>

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
size_t capacity = 2048;

// timer to capture wrist flick
// loop counter
unsigned int ctCur = 0;
// bma negative y value from flicking wrist back
int mvBack = 0;
// bma positive y value from returning wrist quickly to normal
int mvFwd = 0;

// Using isAwake to control state and wake time
/**
 * 0 = light sleep
 * <30 = awake for 3 seconds, listen to touch
 * >= 30 = trigger sleep
 *
 */
uint8_t isAwake = 0;

// TO KEEP AWAKE LONGER - increase this value to 50 or 90 for example
// 30 = about 3 seconds because the loop() delay is 100;
uint8_t timeAwake = 30;


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
  tft->setTextColor(TFT_GREEN);
  delay(2000);
  tft->setCursor(10, 120);
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
    // count
    ctCur++;
    int myY = acc.y;

    // collect counter values at wrist flick extremes
    if (myY < -600) mvBack = ctCur;
    if (myY > 600 ) mvFwd = ctCur;
    // subtract the two times gives the speed, the smaller the difference the faster the flick
    int diff = mvFwd - mvBack;
    // cancel on random slow movement
    if (diff > 5) mvBack = mvFwd = 0;

    // 1. WAKE ..very limited correct movement along y wakes up, note that !isAwake = 0
    if (!isAwake && diff == 1) {
      // (Documents > Arduino > T-Watch-Projects-main > 11 wakeup axp202 minimal
      // wake from lightSleep
      wakeFromLightSleep();
      mvBack = mvFwd = 0;
      //Serial.print("capte, mvFwd:");Serial.print(mvFwd);
      //Serial.print("capte, mvBack:");Serial.print(mvBack);
      displayTime(true);
      isAwake += 1;
    }
    // 2. TOUCH LISTEN - during 3s wake time, 
    // when we write if(isAwake) that is asking if isAwake is true or >0
    if (isAwake && isAwake < timeAwake) {
      
      // listen for touch only while awake
      int16_t x, y;
      if (watch->getTouch(x, y)) {
        //Serial.println("touched");
        wifiConnect();
      }
      // count to about 3 seconds (delay 3000)
      isAwake += 1;
    }

    // 3. TO SLEEP - back to light sleep most of the time
    if (isAwake >= timeAwake) {
      lightSleep();
      isAwake = 0;
    }
    //Serial.println(isAwake);
    //Serial.print(watch->bl->isOn()); // yields 1 (true)
    //Serial.print(" height:"); Serial.print(tft->height()); // 240
    //Serial.print(" width:"); Serial.print(tft->width());    // 240
    //Serial.print(" T:"); Serial.print(ctCur); // float temp = power->getTemp();
    //Serial.print(" Y:"); Serial.println(myY);
    //Serial.print(" Battery:"); Serial.println(power->getBattPercentage());
    delay(100);
  }
}
