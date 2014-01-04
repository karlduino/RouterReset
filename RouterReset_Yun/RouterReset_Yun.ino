/* Testing an Arduino Yun: light up green or red when internet is on
 *
 * red LED (internet down): pin 6
 * green LED (internet up): pin 5
 * yellow LED (testing):    pin 3
 * blue LED (router on):    pin 8
 * power switch tail:       pin 9
 * switch:                  pin 12
 *
 * needs checkInternet.py installed in /usr/bin in Yun Linino
 */

#include <Process.h>

const int redLED     = 6;
const int greenLED   = 5;
const int yellowLED  = 3;
const int routerPin1 = 8;
const int routerPin2 = 9;
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
    pinMode(routerPin1, OUTPUT);
    pinMode(routerPin2, OUTPUT);
    digitalWrite(routerPin1, HIGH);
    digitalWrite(routerPin2, HIGH);
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
    digitalWrite(routerPin1, LOW);
    digitalWrite(routerPin2, LOW);
    for(int i=0; i<resetRouterTimeSec; i++) {
      if(digitalRead(switchPin) == LOW) break;
      delay(1000);
    }
    digitalWrite(routerPin1, HIGH);
    digitalWrite(routerPin2, HIGH);
    delay(resetRouterTimeSec*1000);
  }
}

String pingGoogle(void) {
  Process internetCheck;
  String status;

  digitalWrite(yellowLED, HIGH);

  internetCheck.runShellCommand("/usr/bin/pingGoogle.py");

  status = "";
  while(internetCheck.available() > 0) {
    char c = internetCheck.read();
    if(c != '\n') status += c;
  }

  digitalWrite(yellowLED, LOW);

  return(status);
}
