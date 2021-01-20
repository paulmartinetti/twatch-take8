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

  // pick a random row number in the Google Sheet (redo at n=23)
  unsigned short int randFr = random(1, 14550);
  // create uri (I have yet to learn how to concatenate char* values)
  // expression #25, 26 are longest
  String myUri = frURL + "?limit=1&offset=" + randFr;

  // get json from my google sheet using function in wifiConnect
  String frJson = http(myUri);
  // parse locale json into var called "doc" for document
  DynamicJsonDocument doc(capacity);
  DeserializationError err = deserializeJson(doc, frJson);

  // store fr and en for watch display
  char fr1[40]= "";
  char fr2[40]= "";
  char fr3[40]= "";
  char fr4[40]= "";
  char fr5[40]= "";
  char en1[40]= "";
  char en2[40]= "";
  char en3[40]= "";

  //Serial.print(frJson);

  // fr1-5
  if (doc[0]["fr1"]) {
    strncat(fr1, doc[0]["fr1"], 40);
  } else {
    Serial.println(err.c_str());
  }
  //
  if (doc[0]["fr2"]) {
    strncat(fr2, doc[0]["fr2"], 40);
  } else {
    Serial.println(err.c_str());
  }
  //
  if (doc[0]["fr3"]) {
    strncat(fr3, doc[0]["fr3"], 40);
  } else {
    Serial.println(err.c_str());
  }
  //
  if (doc[0]["fr4"]) {
    strncat(fr4, doc[0]["fr4"], 40);
  } else {
    Serial.println(err.c_str());
  }
  //
  if (doc[0]["fr5"]) {
    strncat(fr5, doc[0]["fr5"], 40);
  } else {
    Serial.println(err.c_str());
  }

  // en1-3
  if (doc[0]["en1"]) {
    strncat(en1, doc[0]["en1"], 40);
  } else {
    Serial.println(err.c_str());
  }
  //
  if (doc[0]["en2"]) {
    strncat(en2, doc[0]["en2"], 40);
  } else {
    Serial.println(err.c_str());
  }
  //
  if (doc[0]["en3"]) {
    strncat(en3, doc[0]["en3"], 40);
  } else {
    Serial.println(err.c_str());
  }

  // display on watch
  tft->fillScreen(TFT_BLACK);
  u8f.setFontDirection(0);          // left to right (this is default)
  u8f.setForegroundColor(TFT_CYAN); // apply color
  u8f.setFont(u8g2_font_10x20_tf);  // extended font
  u8f.setFontMode(1);               // use u8g2 transparent mode (this is default)

  u8f.setCursor(5, 20);
  u8f.println("Expression");
  u8f.println("");
  u8f.println(fr1);
  u8f.println(fr2);
  u8f.println(fr3);
  u8f.println(fr4);
  u8f.println(fr5);
  u8f.println("");
  u8f.println(en1);
  u8f.println(en2);
  u8f.println(en3);

  // hold to read expression
  // listen for weather touch
  int16_t x, y;
  unsigned short int i;
  for (i = 0; i < 8000; i++) {
    if (watch->getTouch(x, y)) {
      //getWeather();
      endWifi();
      return;
    }
    delay(10);
  }

  // end wifi (currently supports viewing one api at a time..no nav yet)
  endWifi();
}
