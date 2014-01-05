/* Router Reset for Arduino Yun
 *
 * red LED (internet down): pin 6
 * green LED (internet up): pin 5
 * yellow LED (testing):    pin 3
 * blue LED (router on):    pin 8
 * power switch tail:       pin 9
 * switch:                  pin 12
 *
 * needs ~/Python/pingGoogle.py on Yun Linino
 */

#include <Process.h>

const int redLED     = 6;
const int greenLED   = 5;
const int yellowLED  = 3;
const int routerPin = 8;
const int routerLED = 9;
const int routerLEDlevel = 100;
const int resetRouterTimeSec = 30;
const int secBetweenPings_Up = 5;
const int secBetweenPings_Down = 1;
const int startUpDelaySec = 5;
const int switchPin = 12;

String internetStatus;

void setup() {
    pinMode(redLED, OUTPUT);
    pinMode(greenLED, OUTPUT);
    pinMode(yellowLED, OUTPUT);
    pinMode(routerPin, OUTPUT);
    pinMode(routerLED, OUTPUT);
    digitalWrite(routerPin, HIGH);
    analogWrite(routerLED, routerLEDlevel);
    pinMode(switchPin, INPUT);
    digitalWrite(switchPin, HIGH);

    Bridge.begin();  // make contact with the linux processor

    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, HIGH);
    digitalWrite(yellowLED, HIGH);
    delay(startUpDelaySec*1000);
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, LOW);
}

void loop() {
  internetStatus = pingGoogle();

  if(internetStatus[0] == '1') {
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
  }
  else if(internetStatus[0] == '0') {
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);

    // wait 1 sec and check again
    delay(secBetweenPings_Down*1000);
    internetStatus = pingGoogle();
  }
    
  if(internetStatus[0] == '0') {
    resetRouter();
    delay(secBetweenPings_Down*1000);
  }
  else
    delay(secBetweenPings_Up*1000);
}

void resetRouter() {
  if(digitalRead(switchPin) == HIGH) {

    digitalWrite(routerPin, LOW);
    digitalWrite(routerLED, LOW);

    for(int i=0; i<resetRouterTimeSec; i++) {
      if(digitalRead(switchPin) == LOW) break;
      delay(1000);
    }

    digitalWrite(routerPin, HIGH);
    analogWrite(routerLED, routerLEDlevel);

    delay(resetRouterTimeSec*1000);
  }
}

String pingGoogle(void) {
  Process internetCheck;
  String result;

  digitalWrite(yellowLED, HIGH);

  internetCheck.runShellCommand("~/Python/pingGoogle.py");

  result = "";
  while(internetCheck.available() > 0) {
    char c = internetCheck.read();
    if(c != '\n') result += c;
  }

  digitalWrite(yellowLED, LOW);

  return(result);
}
