/*** RouterReset: test if internet is working; if not, reset router
 *
 * Karlduino, Dec 2013
 *
 * Parts taken from Arduino wifi examples:
 *   WifiWebClient by dlf, srl, and Tom Igoe
 *   WifiUdpNtpClient by Michael Margolis and Tom Igoe
 *   SD/Datalogger by Tom Igoe
 *
 ***/

#include <stdio.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Time.h>
#include <SD.h>

// private info [made generic here]
char ssid[] = "SSID";
char pass[] = "password";
// end of private info

int status = WL_IDLE_STATUS;
char server[] = "www.google.com";
//IPAddress server(74,125,225,113); // numeric IP for Google
//IPAddress server(192, 168, 1, 155); // numeric IP that doesn't work

WiFiClient client;

// checking connection
unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 10*1000; // wait 10 seconds
boolean canConnect = false;
boolean canConnect_prev = false;

// setting time
unsigned int localPort = 2390;      // local port to listen for UDP packets
const int TIMEZONE = -6;            // central standard time
// const int TIMEZONE = -5;         // central daylight time
IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];
WiFiUDP Udp;
char time_as_string[] = "12/20/1969 00:00:00";

// LED pins
const int redLed = 3;
const int greenLed = 5;
const int yellowLed = 6;
const int routerPin = 8;
const int resetRouterTime = 30 * 1000; // wait 30 seconds

// SD card
const int chipSelect = 4;
File logfile;

void setup() {
  // Initial LEDs
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(routerPin, OUTPUT);
  digitalWrite(routerPin, OUTPUT);

  flash_3leds(redLed, greenLed, yellowLed, 5, 100);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    while (true); // stop
  }

  String fv = WiFi.firmwareVersion();

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);

    delay(1000);
  }
  Udp.begin(localPort);

  sendNTPpacket(timeServer); // send an NTP packet to a time server
  delay(500);
  if( Udp.parsePacket() ) get_and_set_time();

  // setup SD card and log file
  pinMode(10, OUTPUT);
  while(!SD.begin(chipSelect)) flash_3leds(redLed, greenLed, yellowLed, 5, 400);
  char filename[] = "wifi_log.txt";
  logfile = SD.open(filename, FILE_WRITE);
  time_to_string(now(), time_as_string);
  logfile.print("\nStart up - ");
  logfile.println(time_as_string);
  logfile.flush();

  flash_3leds(redLed, greenLed, yellowLed, 5, 100);
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    lastConnectionTime=millis();
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

  // log change in connection status
  if(canConnect != canConnect_prev) {
    time_to_string(now(), time_as_string);
    logfile.print(time_as_string);
    logfile.print(" - ");
    if(canConnect)
      logfile.println("connected.");
    else
      logfile.println("NOT CONNECTED!");
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


void flash_3leds(int pin1, int pin2, int pin3, int number, int time)
{
  int i, j;
  int pins[] = {pin1, pin2, pin3};

  for(i=0; i<number; i++) {
    for(j=0; j<3; j++)
      digitalWrite(pins[j], HIGH);
    delay(time);
    for(j=0; j<3; j++)
      digitalWrite(pins[j], LOW);
    delay(time);
  }
}

// send an NTP request to the time server
unsigned long sendNTPpacket(IPAddress& address)
{
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  Udp.beginPacket(address, 123);
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

void get_and_set_time(void) {
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;

    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears;
    setTime(epoch);
}

void time_to_string(unsigned long epoch, char* time_as_string) {
    epoch = now() + TIMEZONE * 3600;
    sprintf(time_as_string, "%02d/%02d/%4d %d:%02d:%02d",
          month(epoch), day(epoch), year(epoch),
          hour(epoch), minute(epoch), second(epoch));
}

void resetRouter(void) {
  flash_3leds(redLed, greenLed, yellowLed, 5, 100);
  digitalWrite(routerPin, LOW);
  delay(resetRouterTime);
  digitalWrite(routerPin, HIGH);
  delay(resetRouterTime);
  flash_3leds(redLed, greenLed, yellowLed, 5, 100);
}