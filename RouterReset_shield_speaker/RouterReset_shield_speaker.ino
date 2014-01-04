/*** RouterReset: test if internet is working; if not, reset router
 *
 * Karlduino, Dec 2013
 *
 * Parts taken from Arduino wifi examples:
 *   WifiWebClient by dlf, srl, and Tom Igoe
 *   SD/Datalogger by Tom Igoe
 *
 ***/

#include <SPI.h>
#include <WiFi.h>
#include <SD.h>

// private info
char ssid[] = "Br0man";
char pass[] = "4106620554";
// end of private info

char server[] = "www.google.com";
//IPAddress server(74,125,225,113); // numeric IP for google
//IPAddress server(192,168,1,155);  // numeric IP that doesn't work

WiFiClient client;

// checking connection
unsigned long lastConnectionTime = 0;
#define postingInterval 10000
boolean canConnect = false;
boolean canConnect_prev = false;

// LED pins
#define redLed          3
#define greenLed        5
#define yellowLed       6
#define routerPin1      8
#define routerPin2      9
#define resetRouter_nTones 30
#define speakerPin      2
#define errorFreq     220

// SD card
#define chipSelect 4
File logfile;

void setup() {
  // Initial LEDs
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(routerPin1, OUTPUT);
  pinMode(routerPin2, OUTPUT);
  digitalWrite(routerPin1, OUTPUT);
  digitalWrite(routerPin2, OUTPUT);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    while (true); // stop
  }

  String fv = WiFi.firmwareVersion();

  // attempt to connect to Wifi network:
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) delay(1000);

  // setup SD card and log file
  pinMode(10, OUTPUT);
  while(!SD.begin(chipSelect)) error_tone(2, 250, 250);
  char filename[] = "wifi_log.txt";
  logfile = SD.open(filename, FILE_WRITE);
  logfile.println("\nStart up -");
  logfile.flush();

  error_tone(4, 250, 250);
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    client.stop();
  }

  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    digitalWrite(yellowLed, HIGH);
    connect_to_server();
    digitalWrite(yellowLed, LOW);
  }

  if(canConnect) {
    digitalWrite(greenLed, HIGH);
    digitalWrite(redLed, LOW);
  }
  else {
    digitalWrite(greenLed, LOW);
    digitalWrite(redLed, HIGH);

    resetRouter();
  }

  if(canConnect != canConnect_prev) {
    if(!canConnect) logfile.print("not ");
    logfile.println("connected");
    logfile.flush();

    canConnect_prev = canConnect;
  }
}

void connect_to_server(void) {
  if (client.connect(server, 80)) {
    canConnect = true;
    client.println("GET /search?q=arduino HTTP/1.1");
    client.println("Host: www.google.com");
    client.println("Connection: close");
    client.println();
  }
  else {
    client.stop();
    canConnect = false;
  }
  lastConnectionTime=millis();
}

void resetRouter(void) {
  digitalWrite(routerPin1, LOW);
  digitalWrite(routerPin2, LOW);
  error_tone(resetRouter_nTones, 700, 300);
  digitalWrite(routerPin1, HIGH);
  digitalWrite(routerPin2, HIGH);
  error_tone(resetRouter_nTones, 700, 300);
}

void error_tone(unsigned int n_times, unsigned int on_time, unsigned int off_time)
{
  for(int i=0; i<n_times; i++) {
    tone(speakerPin, errorFreq);
    delay(on_time);
    noTone(speakerPin);
    delay(off_time);
  }
}
