# esp-rc-gps-speedometer
Speedometer to track max speed and 0 to X time for rc cars using ESP8266, Oled display and a NEO GPS.

Wemos D1 mini pin out 
-D1 and D2, 3.3V and GND > OLED 
-D6 and D7 , 5v and GND > GPS (wemos D6 to tx on gps module)(wemos D7 to Rx on gps module)
-D3 and GND > Button

Parts:
Wemos D1 Mini and its matching 64x48 OLED sheild.
GPS module was an U-blox NEO 6M part number GY-GPS6MV2.

Required libraries include:
Adafruit Graphics
https://github.com/adafruit/Adafruit-GFX-Library
A modified version of Adafruit's SSD1306 library (This is important as the standard library won't display on our tiny diplay correctly)
https://github.com/mcauser/Adafruit_SSD1306
Tiny GPS++
http://arduiniana.org/libraries/tinygpsplus
Font 
provided in project folder
Notes on using different fonts https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts


Button use:
//Short(1s)= reset status, time and max speed
//Long(3s) = change target speed for 0 to target(increments by 10)

<p align="center">
  <img src="https://github.com/lbento27/esp-rc-gps-speedometer/blob/main/Resources/IMG_3372.JPG?raw=true" width="700">
</p>
