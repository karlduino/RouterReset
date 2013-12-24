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
#include <SPI.h>
#include <WiFiUdp.h>
#include <Time.h>


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
boolean lastConnected = false;
const unsigned long postingInterval = 10*1000; // wait 10 seconds
boolean canConnect = false;

// setting time
unsigned int localPort = 2390;      // local port to listen for UDP packets
const int TIMEZONE = -6;            // central standard time
// const int TIMEZONE = -5;         // central daylight time
IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];
WiFiUDP Udp;
char time_as_string[] = "12/20/1969 00:00:00";

const int redLed = 3;
const int greenLed = 5;
const int yellowLed = 6;

void setup() {
  // Initial LEDs
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);

  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  flash_3leds(redLed, greenLed, yellowLed, 5, 100);
  while (!Serial) { ; } // wait for serial port to connect.
  flash_3leds(redLed, greenLed, yellowLed, 5, 400);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true); // stop
  }

  String fv = WiFi.firmwareVersion();
  if ( fv != "1.1.0" )
    Serial.println("Please upgrade the firmware");
  Serial.print("Wifi firmware: ");
  Serial.println(fv);

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);

    delay(1000);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();

  Serial.println("Connecting to time server.");
  Udp.begin(localPort);

  Serial.println("Getting time.");
  sendNTPpacket(timeServer); // send an NTP packet to a time server
  delay(500);
  if(  Udp.parsePacket() ) get_and_print_time();
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    Serial.println("Client avaliable.");
    Serial.println("Disconnecting...");
    lastConnectionTime=millis();
    client.stop();
  }

  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    digitalWrite(yellowLed, HIGH);
    connect_to_server();
    digitalWrite(yellowLed, LOW);

    Serial.print("Current time: ");
    time_to_string(now(), time_as_string);
    Serial.println(time_as_string);
  }

  lastConnected = client.connected();
  if(canConnect) {
    digitalWrite(greenLed, HIGH);
    digitalWrite(redLed, LOW);
  }
  else {
    digitalWrite(greenLed, LOW);
    digitalWrite(redLed, HIGH);
  }

}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void connect_to_server(void) {
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    canConnect = true;
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /search?q=arduino HTTP/1.1");
    client.println("Host: www.google.com");
    client.println("Connection: close");
    client.println();
  }
  else {
    client.stop();
    Serial.println("CONNECTION PROBLEM");
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

// send an NTP request to the time server at the given address
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

void get_and_print_time(void) {
    Serial.println("packet received");
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;

    // now convert NTP time into everyday time:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    setTime(epoch);

    Serial.print("Current time: ");
    time_to_string(epoch, time_as_string);
    Serial.println(time_as_string);
}

void time_to_string(unsigned long epoch, char* time_as_string) {

  epoch = epoch + TIMEZONE * 3600;

  sprintf(time_as_string, "%02d/%02d/%4d %d:%02d:%02d",
          month(epoch), day(epoch), year(epoch),
          hour(epoch), minute(epoch), second(epoch));
}
