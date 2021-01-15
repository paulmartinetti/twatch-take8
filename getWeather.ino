#include <WiFi.h>
#include "time.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <string.h>

// JSON size defined for largest of api call jsons (currently localeJson, see below)
// I started with the capacity value from JHershey's Oweather, which works.
// Then I copied the actual json from the serial monitor and pasted it here:
// https://arduinojson.org/v6/assistant/
const size_t capacity = 2048;

// reusable HTTP Client for each api call
String http(String myUri) {

  // swap myUri for its return json
  HTTPClient http;

  http.begin(myUri);
  int httpCode = http.GET();
  if (httpCode > 399) {
    Serial.print("http error");
    http.end();
  } else if (httpCode > 0) {
    // no longer need uri value, so swap w incoming json
    // (saves memory creating another String)
    myUri = http.getString();
  }  else {
    myUri = "failed";
  }
  http.end();
  return myUri;
}

void getWeather() {

  // connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // display WiFi status on watch
  tft->fillScreen(TFT_BLACK);
  //tft->setTextFont(2); -- too big, still testing free fonts..
  tft->setTextSize(2);
  tft->setCursor(0, 110);
  tft->println(F("Connecting ..."));

  // delay()s in this while function do not work as intended
  // planning to redo wifi code here..
  while (WiFi.status() != WL_CONNECTED) {}

  // WiFi get official time (from Arduino TimeSynchronization sample)
  const char* ntpServer = "pool.ntp.org";
  const long  gmtOffset_sec = -18000;
  const int   daylightOffset_sec = 3600;

  // might as well update clock while connected to wifi
  configTime(gmtOffset_sec, daylightOffset_sec , ntpServer, "time.nis.gov");
  // Connecting...
  delay(3000);

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    tft->drawString("Failed to update time",  5, 30, 1);
  } else {
    tft->fillScreen(TFT_BLACK);
    tft->setCursor(0, 30);
    tft->print(&timeinfo, "%A\n%d-%b-%Y\n%Hh%M");
    watch->rtc->setDateTime(timeinfo.tm_year, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  }

  // use http client function above to get json of my region (var from config.h)
  String localeJson = http(regionURL);
  // check out json to find filter keys for desired values
  //Serial.print(localeJson);

  // parse locale json into var called "doc" for document
  DynamicJsonDocument doc(capacity);
  // from this source, only interested in locality name
  StaticJsonDocument<200> filter;
  filter["locality"] = true;
  DeserializationError err = deserializeJson(doc, localeJson, DeserializationOption::Filter(filter));

  // if locality info exists, display on watch
  char city[60] = {NULL};
  if (doc["locality"]) {
    strncpy(city, doc["locality"], 60);
  } else {
    Serial.print(err.c_str());
  }
  //Serial.print(city);
  tft->setCursor(0, 120);
  tft->println(city);
  // city from bigdata
  delay(1000);

  // start weather check..
  tft->setCursor(0, 170);
  tft->println(F("Checking weather..."));
  delay(2000);

  // using reusable http client, get weather json name using latitude / longitude (from config.h)
  String weatherJson = http(oweatherURL);
  Serial.print(weatherJson);

  // parse weather json into doc2
  DynamicJsonDocument doc2(capacity);

  // organize weather data
  struct myTemp {
    char main[60]= {NULL};
    float feels;
    float tMax;
    float humidity;
    float wind;
    float clouds;
  };
  typedef struct myTemp MyTemp;
  MyTemp myTemp;

  // from this source, filter wanted info
  // to make this filter, I pasted the json from serial monitor
  // here http://jsonformatter.curiousconcept.com/
  StaticJsonDocument<200> filter2;
  filter2["daily"][0]["weather"][0]["main"] = true;
  filter2["current"]["feels_like"] = true;
  filter2["daily"][0]["temp"]["max"] = true;
  filter2["current"]["humidity"] = true;
  filter2["current"]["wind_speed"] = true;
  filter2["current"]["clouds"] = true;
  // de-serialize
  DeserializationError err2 = deserializeJson(doc2, weatherJson, DeserializationOption::Filter(filter2));

  // capture available values using keys
  // (hoping to find a way to loop this!)
  if (doc2["daily"][0]["weather"][0]["main"]) {
    strncpy(myTemp.main, doc2["daily"][0]["weather"][0]["main"], 60);
  } else {
    Serial.print("Main "); Serial.println(err2.c_str());
  }

  // feels like
  if (doc2["current"]["feels_like"]) {
    myTemp.feels = doc2["current"]["feels_like"];
  } else {
    Serial.print("feels like "); Serial.println(err2.c_str());
  }

  // temp max -- not a forecast but the high temp du jour
  if (doc2["daily"][0]["temp"]["max"]) {
    myTemp.tMax = doc2["daily"][0]["temp"]["max"];
  } else {
    Serial.print("forecast max"); Serial.println(err2.c_str());
  }

  // humidity
  if (doc2["current"]["humidity"]) {
    myTemp.humidity = doc2["current"]["humidity"];
  } else {
    Serial.print("humidity "); Serial.println(err2.c_str());
  }

  // wind speed
  if (doc2["current"]["wind_speed"]) {
    myTemp.wind = doc2["current"]["wind_speed"];
  } else {
    Serial.print("wind "); Serial.println(err2.c_str());
  }

  // clouds
  if (doc2["current"]["clouds"]) {
    myTemp.clouds = doc2["current"]["clouds"];
  } else {
    Serial.print("clouds "); Serial.println(err2.c_str());
  }

  //Serial.print(myTemp);
  tft->fillScreen(TFT_BLACK);
  tft->setCursor(0, 10);
  tft->println("Weather");
  tft->print("Mainly: "); tft->println(myTemp.main);
  tft->println(" ");
  tft->print("Real feel: "); tft->println(myTemp.feels);
  tft->println(" ");
  tft->print("Tmax: "); tft->println(myTemp.tMax);
  tft->println(" ");
  tft->print("Humidity: "); tft->println(myTemp.humidity);
  tft->println(" ");
  tft->print("Wind: "); tft->println(myTemp.wind);
  tft->println(" ");
  tft->print("Clouds: "); tft->println(myTemp.clouds);
  tft->println(" ");


  // hold to read weather
  delay(5000);

  // end awake session
  //tft->setCursor(0, 220);
  tft->println(F("Disconnecting ..."));
  delay(3000);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  // ensure light sleep after disconnect
  isAwake = timeAwake + 1;
}
