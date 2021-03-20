#include <WiFi.h>
#include <HTTPClient.h>

void wifiConnect() {
  // connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // display WiFi status on watch
  tft->fillScreen(TFT_BLACK);
  //tft->setTextSize(1);
  tft->setCursor(0, 100);
  tft->println(F("Connecting ..."));

  // cancel if failed WiFi
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    tft->setCursor(0, 140);
    tft->println("WiFi failed.");
    delay(2000);
    endWifi();
    return;
  }

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
    tft->setCursor(5, 10);
    tft->print(&timeinfo, "%A\n%d-%b-%Y\n%Hh%M");
    watch->rtc->setDateTime(timeinfo.tm_year, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  }

  /**

     connect to apis -- control which api

     NOTE getFrenchExpression is a private api call
     --use the code as an example to load data from an api of your choice


  */
  delay(2000);
  getWeather();
  //getFrenchExpression();
}
void endWifi() {
  // end awake session
  //tft->fillScreen(TFT_BLACK);
  tft->setTextSize(2);
  tft->setCursor(0, 200);
  tft->println(F("Disconnecting ..."));
  delay(1000);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

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
