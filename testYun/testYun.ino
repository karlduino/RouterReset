/* Testing an Arduino Yun: light up green or red when internet is on
 *
 * red LED (internet down): pin 3
 * green LED (internet up): pin 5
 * yellow LED (testing):    pin 6
 *
 * needs checkInternet.py installed in /usr/bin in Yun Linino
 */

#include <Process.h>

const int redLED = 3;
const int greenLED = 5;
const int yellowLED = 6;

void setup() {
    pinMode(redLED, OUTPUT);
    pinMode(greenLED, OUTPUT);
    pinMode(yellowLED, OUTPUT);

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

    while(internetCheck.available() > 0)
    {
        char c = internetCheck.read();

        if(c == '1')
        {
            digitalWrite(redLED, LOW);
            digitalWrite(greenLED, HIGH);
            break;
        }
        else if(c == '0')
        {
            digitalWrite(redLED, HIGH);
            digitalWrite(greenLED, LOW);
            break;
        }
    }
    
    while(internetCheck.available() > 0) // read everything else
        internetCheck.read();

    digitalWrite(yellowLED, LOW);

    delay(5000);
}
