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
char lastStatus = ' '; // 0 = down, 1 = up

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

    logEvent("startup");
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
  }
}

String pingGoogle(void) {
  Process p;
  String result;

  digitalWrite(yellowLED, HIGH);

  p.runShellCommand("/root/Python/pingGoogle.py");

  result = "";
  while(p.available() > 0) {
    char c = p.read();
    if(c != '\n') result += c;
  }

  if(result[0] != lastStatus) {
    if(result[0] == '1') logEvent("up");
    else logEvent("down");
    lastStatus = result[0];
  }

  digitalWrite(yellowLED, LOW);

  return(result);
}

void logEvent(String status) {
  Process p;

  p.begin("/root/Python/logEvent.py");
  p.addParameter(status);
  p.run();
}
