/* Testing an Arduino Yun: light up green or red when internet is on
 *
 * red LED (internet down): pin 3
 * green LED (internet up): pin 5
 * yellow LED (testing):    pin 6
 * 
 *
 * needs checkInternet.py installed in /usr/bin in Yun Linino
 */

#include <Process.h>

const int redLED     = 3;
const int greenLED   = 5;
const int yellowLED  = 6;
const int routerPin1 = 8;
const int routerPin2 = 9;
const int resetRouterTime = 30000;
const int timeBetweenPings = 5000;

String internetStatus;

void setup() {
    pinMode(redLED, OUTPUT);
    pinMode(greenLED, OUTPUT);
    pinMode(yellowLED, OUTPUT);
    pinMode(routerPin1, OUTPUT);
    pinMode(routerPin2, OUTPUT);
    digitalWrite(routerPin1, HIGH);
    digitalWrite(routerPin2, HIGH);

    Bridge.begin();  // make contact with the linux processor

    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, HIGH);
    digitalWrite(yellowLED, HIGH);
    delay(5000);
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, LOW);
}

void loop() {
    Process internetCheck;

    digitalWrite(yellowLED, HIGH);
    internetCheck.runShellCommand("/usr/bin/checkInternet.py");

    internetStatus = "";
    while(internetCheck.available() > 0) {
      char c = internetCheck.read();
      if(c != '\n') internetStatus += c;
    }

    if(internetStatus[0] == '1')
    {
        digitalWrite(redLED, LOW);
        digitalWrite(greenLED, HIGH);
    }
    else if(internetStatus[0] == '0')
    {
        digitalWrite(redLED, HIGH);
        digitalWrite(greenLED, LOW);
    }
    
    digitalWrite(yellowLED, LOW);

    if(internetStatus[0] == '0')
        resetRouter();
    else
        delay(timeBetweenPings);
}

void resetRouter(void) {
  digitalWrite(routerPin1, LOW);
  digitalWrite(routerPin2, LOW);
  delay(resetRouterTime);
  digitalWrite(routerPin1, HIGH);
  digitalWrite(routerPin2, HIGH);
}
