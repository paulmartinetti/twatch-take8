#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <string.h>

// JSON size defined for largest of api call jsons (currently localeJson, see below)
// I started with the capacity value from JHershey's Oweather, which works.
// Then I copied the actual json from the serial monitor and pasted it here:
// https://arduinojson.org/v6/assistant/

void getFrenchExpression() {
  // start get French
  tft->setCursor(0, 170);
  tft->println(F("Getting French..."));
  delay(2000);
  
  // using sheetdb.io for REST API to my google sheet containing French and English expressions
  // offset value in uri is rowNum-1

  // pick a random number
  unsigned short int randFr = random(1, 11400);
  // create uri (have to learn how to concatenate char* values..
  String myUri = frURL+"?limit=1&offset="+randFr;
  
  // get json from my google sheet using function in wifiConnect
  String frJson = http(myUri);
  // parse locale json into var called "doc" for document
  DynamicJsonDocument doc(capacity);
  DeserializationError err = deserializeJson(doc, frJson);

  // store fr and en for watch display
  char frExp[60];
  char enExp[60];

  // confirm
  if (doc[0]["french"]) {
    strncpy(frExp, doc[0]["french"], 60);
    //Serial.print(frExp);
  } else {
    Serial.println(err.c_str());
  }
  if (doc[0]["english"]) {
    strncpy(enExp, doc[0]["english"], 60);
    //Serial.print(enExp);
  } else {
    Serial.println(err.c_str());
  }

  // display on watch
  tft->fillScreen(TFT_BLACK);
  tft->setCursor(0, 10);
  tft->println("Expression");
  tft->println(" ");
  tft->println(frExp);
  tft->println(" ");
  tft->println(" ");
  tft->println(enExp);

  // hold to read expression
  delay(6000);

  // end wifi (currently supports viewing one api at a time..no nav yet)
  endWifi();
}
