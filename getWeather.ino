#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <string.h>

// JSON size defined for largest of api call jsons (currently localeJson, see below)
// I started with the capacity value from JHershey's Oweather, which works.
// Then I copied the actual json from the serial monitor and pasted it here:
// https://arduinojson.org/v6/assistant/

void getWeather() {

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
    char main[60] = {NULL};
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

  delay(6000);
  //getFrenchExpression();

  // end wifi (currently supports viewing one api at a time..no nav yet)
  endWifi();
}
