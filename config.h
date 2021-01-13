
// => Hardware select
// #define LILYGO_WATCH_2019_WITH_TOUCH     // To use T-Watch2019 with touchscreen, please uncomment this line
// #define LILYGO_WATCH_2019_NO_TOUCH       // To use T-Watch2019 Not touchscreen , please uncomment this line
#define LILYGO_WATCH_2020_V1              // To use T-Watch2020 , please uncomment this line
// #define LILYGO_WATCH_BLOCK               // To use T-Watch Block , please uncomment this line

#include <LilyGoWatch.h>

// 1. Replace values here with WIFI ssid and PW
const char* ssid = "YOUR WIFI SSID";
const char* password = "YOUR WIFI PW";

// 2. Your latitude and longitude
// use gps on your smartphone to get your latitude and longitude values like 
// (example--> from Google Maps, click on your location and numbers are displayed in the field above 29.02355 and -61.99456)

// 3. Your location
// -- replace YOUR_LATITUDE in this url with your latitude (for example 29.02355)
// -- replace YOUR_LONGITUDE in this url with your longitude value (for example -61.994561)
const char* regionURL = "https://api.bigdatacloud.net/data/reverse-geocode-client/?latitude=YOUR_LATITUDE&longitude=YOUR_LONGITUDE";


// 4. Your lat, lon, and and API key
// -- replace YOUR_LATITUDE in this url with your latitude (for example 29.02355)
// -- replace YOUR_LONGITUDE in this url with your longitude value (for example -61.994561)
// -- contact openweathermap to get an api key and replace YOUR_API_KEY with your key which looks like abc131cba38abc321abc987abc334
const char* oweatherURL = "http://api.openweathermap.org/data/2.5/weather?lat=YOUR_LATITUDE&lon=YOUR_LONGITUDE&units=metric&appid=YOUR_API_KEY";
