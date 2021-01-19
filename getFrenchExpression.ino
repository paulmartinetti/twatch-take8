#include <ArduinoJson.h>

// ST7789V -- TFT for t-watch-2020 v1
// U8g2 font instancefonts for French characters
U8G2_FOR_ADAFRUIT_GFX u8f;

void getFrenchExpression() {
  // connect u8g2 procedures to TFT_eSPI
  u8f.begin(*tft);

  // start get French
  tft->setCursor(0, 200);
  tft->println(F("Getting French..."));
  delay(2000);

  // using https://sheetdb.io for REST API to my google sheet containing French and English expressions
  // offset value in uri is rowNum-1

  // pick a random row number in the Google Sheet
  unsigned short int randFr = random(1, 11400);
  // create uri (I have yet to learn how to concatenate char* values)
  String myUri = frURL + "?limit=1&offset=" + randFr;

  // get json from my google sheet using function in wifiConnect
  String frJson = http(myUri);
  // parse locale json into var called "doc" for document
  DynamicJsonDocument doc(capacity);
  DeserializationError err = deserializeJson(doc, frJson);

  // store fr and en for watch display
  String frExp;
  //char frExp[60];
  char enExp[60];

  //
  if (doc[0]["french"]) {
    // explains .as<String>() syntax https://arduinojson.org/v6/api/config/enable_arduino_string/
    frExp = doc[0]["french"].as<String>();
    //strncpy(frExp, doc[0]["french"], 60);
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

  // start manual text wrap
  Serial.print(frExp.length());

  // display on watch
  tft->fillScreen(TFT_BLACK);
  u8f.setFontDirection(0);          // left to right (this is default)
  u8f.setForegroundColor(TFT_CYAN); // apply color
  u8f.setFont(u8g2_font_10x20_tf);  // extended font
  u8f.setFontMode(1);               // use u8g2 transparent mode (this is default)            

  u8f.setCursor(5, 20);
  u8f.println("Expression");
  u8f.println(" ");
  u8f.println(frExp);
  u8f.println(" ");
  u8f.println(" ");
  u8f.println(enExp);

  // continue to weather or not
  delay(2000);
  
  u8f.println("");
  u8f.println("");
  u8f.println("Touche pour la météo");

  // hold to read expression
  // listen for weather touch
  int16_t x, y;
  unsigned short int i;
  for (i = 0; i < 600; i++) {
    if (watch->getTouch(x, y)) {
      getWeather();
      return;
    }
    delay(10);
  }

  // end wifi (currently supports viewing one api at a time..no nav yet)
  endWifi();
}
