<p>Repository name - WakeupFromSensor2</p>

<p>
Author - experienced dad and programmer, new to Arduino, C++, got a couple ttgo 2020s for my 12 yo son and me. We like the retro look. A flick of the wrist wakes up the watch from light sleep. Battery lasts all day. While awake, if screen is touched, Wifi connection updates time, then displays weather in a text readout, then back to sleep
<br>
<br>
Instructions if you are new to Arduino:<br>
1. Create a New Folder named WakeupFromSensor2, put all of the repository files in it (.ino and .h files)
<br>
2. Open the folder and launch WakeupFromSensor2.ino in the Arduino IDE.
<br>
3. In config.h, to connect to wifi and check weather, replace the 4 placeholder variables with your info, see comments for details
<br>

<p>
Video demo<br>
https://youtu.be/tKbF3VBxxFI<br>
</p>

<p>
Repository<br>
strengths - sleep and wake functions, light-weight state management<br>
weaknesses - err checking, compartmentalizing / reusing code, data-types..need to use more char's<br>
</p>

<p>
vvv--I'm using bits of code from these brilliant folks and helpful resources--vvv<br>
</p>

The "official" watch library<br>
https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library
<br>
<br>
Fonts to support glyphs and latin characters<br>
In the ttgo library, it's in examples > U8g2_for_TFT_eSPI > Unicode<br>
https://github.com/olikraus/u8g2/wiki
<br>
<br>
Dan Geiger's watch code<br>
https://www.instructables.com/Lilygo-T-Watch-2020-Arduino-Framework/
<br>
<br>
DIY Projects - Fr / En<br>
https://diyprojects.io/t-watch-sleep-and-wake-up-with-bma423-accelerometer-or-esp32-button/#.X_NqVOlKjlw 
<br>
<br>
J Hershey's OpenWeatherOneCall library<br>
https://github.com/JHershey69/OpenWeatherOneCall 
<br>
<br>
WF Dudley - t-watch 2020<br>
https://github.com/wfdudley/T-watch-2020
<br>
<br>
Bald Engineer - the F() Macro explained<br>
https://www.baldengineer.com/arduino-f-macro.html
<br>
<br>
Date time formatting in C++<br>
http://www.cplusplus.com/reference/ctime/strftime/<br>
<br>
Data types in C++<br>
https://www.tutorialspoint.com/cplusplus/cpp_data_types.htm
</p>