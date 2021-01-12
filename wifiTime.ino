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
  tft->setTextSize(2);
  tft->setCursor(0, 110);
  tft->println("Connecting ...");

  while (WiFi.status() != WL_CONNECTED) {}

  configTime(-18000, 3600 , "pool.ntp.org", "time.nis.gov");
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
  HTTPClient http;
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

  const size_t capacity = JSON_ARRAY_SIZE(4) + JSON_ARRAY_SIZE(5) + JSON_OBJECT_SIZE(2) + 2 * JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 3 * JSON_OBJECT_SIZE(6) + 2 * JSON_OBJECT_SIZE(8) + JSON_OBJECT_SIZE(14) + 1050;
  DynamicJsonDocument doc(capacity);
  // from this source, only interested in locality name
  StaticJsonDocument<200> filter;
  filter["locality"] = true;
  DeserializationError err = deserializeJson(doc, locale, DeserializationOption::Filter(filter));
  char CITY[60] = {NULL};
  if (doc["locality"]) {
    strncpy(CITY, doc["locality"], 60);
  } else {
    Serial.print(err.c_str());
  }
  //Serial.print(CITY);
  tft->setCursor(0, 120);
  tft->println(CITY);
  http.end();

  //========= weather api call
  // get weather name using latitude / longitude
  String weather;
  //char locationURL[200];
  HTTPClient http2;
  http2.begin(oweatherURL);
  httpCode = http2.GET();
  if (httpCode > 399) {
    Serial.print("http error");
    http2.end();
  } else if (httpCode > 0) {
    weather = http2.getString();
    Serial.println(weather);
  }  else {
    Serial.print("get failed");
  }
  DynamicJsonDocument doc2(capacity);
  // from this source, only interested in locality name
  StaticJsonDocument<200> filter2;
  filter2["main"]["feels_like"] = true;
  DeserializationError err2 = deserializeJson(doc2, weather, DeserializationOption::Filter(filter2));
 float mytemp;
  if (doc2["main"]["feels_like"]) {
    mytemp = doc2["main"]["feels_like"];
  } else {
    Serial.print("here "); Serial.println(err2.c_str());
  }
  Serial.print(mytemp);
  tft->setCursor(0, 170);
  tft->print("Temp: "); tft->print(mytemp);
  http2.end();
  /**
      printf("Current Temp : % .0f\n", OWOC.current->temperature);
      printf("Current Humidity : % .0f\n", OWOC.current->humidity);
  */

  // close
  delay(2000);
  tft->setCursor(0, 210);
  tft->println("Disconnecting ...");
  delay(2000);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  // ensure light sleep after disconnect
  isAwake = 31;
}
