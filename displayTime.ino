

#include <time.h>

byte xcolon = 0; // location of the colon

void displayTime(boolean fullUpdate) {

  byte xpos = 40; // Starting position for the display
  byte ypos = 90;

  // format month to 3 letters
  String monthThree[] = {"Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sept", "Oct", "Nov", "Dec"};

  // Get the current data - RTC = Real Time Clock
  RTC_Date tnow = watch->rtc->getDateTime();

  hh = tnow.hour;
  mm = tnow.minute;
  ss = tnow.second;
  dday = tnow.day;
  mmonth = tnow.month;
  yyear = tnow.year;

  if (fullUpdate) {
    // Font 7 is a 7-segment display and only contains
    // characters [space] 0 1 2 3 4 5 6 7 8 9 0 : .

    // tft->drawString("88:88", xpos, ypos, 7);
    tft->fillScreen(TFT_BLACK);
    tft->setTextSize(1);

    // draw hours
    if (hh < 10) xpos += tft->drawChar('0', xpos, ypos, 7);
    xpos += tft->drawNumber(hh, xpos, ypos, 7);

    // colon
    xcolon = xpos + 3;
    xpos += tft->drawChar(':', xcolon, ypos, 7);

    // minutes
    if (mm < 10) xpos += tft->drawChar('0', xpos, ypos, 7);
    tft->drawNumber(mm, xpos, ypos, 7);
  }

  // check battery %
  tft->setTextFont(2);
  tft->setTextSize(2);
  int pct = power->getBattPercentage();
  tft->setCursor(10, 5);
  tft->print("Battery: "); tft->print(pct); tft->println(" %");

  // date
  //tft->setTextSize(2);
  tft->setCursor( 10, 205);
  tft->print(dday);
  tft->print("-");
  tft->print(monthThree[mmonth - 1]);
  tft->print("-");
  tft->print(yyear);
}
