# esp-rc-gps-speedometer
Speedometer to track max speed and 0 to X time for rc cars using ESP8266, Oled display and a NEO GPS.


Wemos D1 mini pin out:
<ul>
<li>D1 and D2, 3.3V and GND > OLED </li>
<li>D6 and D7 , 5v and GND > GPS (wemos D6 to tx on gps module)(wemos D7 to Rx on gps module)</li>
<li>D3 and GND > Button</li>
</ul>
  
Parts:
<ul>
<li>Wemos D1 Mini and its matching 64x48 OLED sheild.</li>
<li>GPS module was an U-blox NEO 6M part number GY-GPS6MV2.</li>
</ul>

Required libraries include:
<ul>
<li>Adafruit Graphics</li>
<li>A modified version of Adafruit's SSD1306 library https://github.com/mcauser/Adafruit_SSD1306</li>
<li>Tiny GPS++</li>
<li>Font-provided in project folder</li>
</ul>
Notes on using different fonts https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts



Button use:
<ul>
<li>Short(1s)= reset status, time and max speed</li>
<li>Long(3s) = change target speed for 0 to target(increments by 10)</li>
</ul>
<br/>
<p align="left">
  <img src="https://github.com/lbento27/esp-rc-gps-speedometer/blob/main/Resources/IMG_3372.JPG?raw=true" width="700">
</p>
