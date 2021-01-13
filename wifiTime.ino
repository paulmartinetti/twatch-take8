#include <WiFi.h>
#include "time.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <string.h>

void appWiFiTime() {

  const char* ntpServer = "pool.ntp.org";
  const long  gmtOffset_sec = -18000;
  const int   daylightOffset_sec = 3600;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // wifi screen
  tft->fillScreen(TFT_BLACK);
  //tft->setTextFont(2);
  tft->setTextSize(2);
  tft->setCursor(0, 110);
  tft->println(F("Connecting ..."));

  while (WiFi.status() != WL_CONNECTED) {}

  configTime(-18000, 3600 , "pool.ntp.org", "time.nis.gov");
  // Connecting...
  delay(3000);

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    tft->drawString("Failed",  5, 30, 1);
  } else {
    tft->fillScreen(TFT_BLACK);
    tft->setCursor(0, 30);
    tft->print(&timeinfo, "%A\n%d-%b-%Y\n%Hh%M");
    watch->rtc->setDateTime(timeinfo.tm_year, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  }

  // get location name using latitude / longitude
  /*HTTPClient http;
  String locale;
  //char locationURL[200];
  http.begin(regionURL);
  int httpCode = http.GET();
  if (httpCode > 399) {
    Serial.print("http error");
    http.end();
  } else if (httpCode > 0) {
    locale = http.getString();
    //Serial.print(locale);
  }  else {
    Serial.print("get failed");
  }
  http.end();*/
  
  // weather
  const size_t capacity = JSON_ARRAY_SIZE(4) + JSON_ARRAY_SIZE(5) + JSON_OBJECT_SIZE(2) + 2 * JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 3 * JSON_OBJECT_SIZE(6) + 2 * JSON_OBJECT_SIZE(8) + JSON_OBJECT_SIZE(14) + 1050;
  /*DynamicJsonDocument doc(capacity);

  // from this source, only interested in locality name
  StaticJsonDocument<200> filter;
  filter["locality"] = true;
  DeserializationError err = deserializeJson(doc, locale, DeserializationOption::Filter(filter));

  char city[60] = {NULL};
  if (doc["locality"]) {
    strncpy(city, doc["locality"], 60);
  } else {
    Serial.print(err.c_str());
  }
  //Serial.print(city);
  tft->setCursor(0, 120);
  tft->println(city);
  // city from bigdata*/
  delay(1000);
  tft->setCursor(0, 170);
  tft->println(F("Checking weather..."));
  delay(2000);

  //========= weather api call, new http client bc haven't figured out how to reset
  // get weather name using latitude / longitude
  String weather;
  //char locationURL[200];
  HTTPClient http2;
  http2.begin(oweatherURL);
  int httpCode = http2.GET();
  if (httpCode > 399) {
    Serial.print("http error");
    http2.end();
  } else if (httpCode > 0) {
    weather = http2.getString();
    Serial.println(weather);
  }  else {
    Serial.print("get failed");
  }
  http2.end();
  // parse
  DynamicJsonDocument doc2(capacity);

  // organize weather data
  struct myTemp {
    float temp;
    float feels;
    float tMax;
    float humidity;
    float wind;
    float clouds;
  };
  typedef struct myTemp MyTemp;
  MyTemp myTemp;

  // from this source, filter wanted info
  StaticJsonDocument<200> filter2;
  filter2["main"]["temp"] = true;
  filter2["main"]["feels_like"] = true;
  filter2["main"]["temp_max"] = true;
  filter2["main"]["humidity"] = true;
  filter2["wind"]["speed"] = true;
  filter2["clouds"]["all"] = true;

  DeserializationError err2 = deserializeJson(doc2, weather, DeserializationOption::Filter(filter2));

  // capture values
  // temp
  if (doc2["main"]["temp"]) {
    myTemp.temp = doc2["main"]["temp"];
  } else {
    Serial.print("temp "); Serial.println(err2.c_str());
  }

  // feels like
  if (doc2["main"]["feels_like"]) {
    myTemp.feels = doc2["main"]["feels_like"];
  } else {
    Serial.print("feels like "); Serial.println(err2.c_str());
  }

  // temp max
  if (doc2["main"]["temp_max"]) {
    myTemp.tMax = doc2["main"]["temp_max"];
  } else {
    Serial.print("temp max "); Serial.println(err2.c_str());
  }

  // humidity
  if (doc2["main"]["humidity"]) {
    myTemp.humidity = doc2["main"]["humidity"];
  } else {
    Serial.print("humidity "); Serial.println(err2.c_str());
  }

  // wind speed
  if (doc2["wind"]["speed"]) {
    myTemp.wind = doc2["wind"]["speed"];
  } else {
    Serial.print("wind "); Serial.println(err2.c_str());
  }

  // clouds
  if (doc2["clouds"]["all"]) {
    myTemp.clouds = doc2["clouds"]["all"];
  } else {
    Serial.print("clouds "); Serial.println(err2.c_str());
  }

  //Serial.print(myTemp);
  tft->fillScreen(TFT_BLACK);
  tft->setCursor(0, 10);
  tft->println("Weather");
  tft->print("Temp: "); tft->println(myTemp.temp);
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


  // close
  delay(5000);
  //tft->setCursor(0, 220);
  tft->println(F("Disconnecting ..."));
  delay(3000);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  // ensure light sleep after disconnect
  isAwake = 31;
}
