### Router Reset

[Karlduino](http://github.com/karlduino)

The aim of this project is to sense that the internet connection over
wifi is working. If it's not, shut off the router and turn it back on
again.

- [Arduino Yun](http://arduino.cc/en/Main/arduinoBoardYun)
- Red LED with 470 Ohm resistor to pin 3
- Green LED with 470 Ohm resistor to pin 5
- Yellow LED with 470 Ohm resistor to pin 6
- [Power switch tail](https://www.sparkfun.com/products/10747) attached to pin 8
- Blue LED with 470 Ohm resistor to pin 9

Here's what this does:

- Connect to wifi
- Test connection to [Google](www.google.com); light yellow LED during
  test
- If connection, light the green LED
- If no connection, light the red LED

To be added:
- If no connection, turn the blue LED and power switch tail off for 30
  sec and then back on (to reset wireless router connected to the
  power switch tail)
- Log time and status to SD card, for viewing from the web

----

Licensed under the [MIT license](License.txt). ([More information](http://en.wikipedia.org/wiki/MIT_License).)
