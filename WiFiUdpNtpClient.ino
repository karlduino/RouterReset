/*

 Udp NTP Client

 Get the time from a Network Time Protocol (NTP) time server
 Demonstrates use of UDP sendPacket and ReceivePacket
 For more on NTP time servers and the messages needed to communicate with them,
 see http://en.wikipedia.org/wiki/Network_Time_Protocol

 created 4 Sep 2010
 by Michael Margolis
 modified 9 Apr 2012
 by Tom Igoe

 This code is in the public domain.

 */

#include <stdio.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Time.h>

int status = WL_IDLE_STATUS;
// private info [made generic here]
char ssid[] = "SSID";
char pass[] = "password";
// end of private info

unsigned int localPort = 2390;      // local port to listen for UDP packets

const int TIMEZONE = -6; // central standard time
// const int TIMEZONE = -5; // central daylight time

IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;

char time_as_string[] = "12/20/1969 00:00:00";

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if ( fv != "1.1.0" )
    Serial.println("Please upgrade the firmware");

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 3 seconds for connection:
    delay(3000);
  }

  Serial.println("Connected to wifi.");
  printWifiStatus();

  Serial.println("Connecting to time server.");
  Udp.begin(localPort);

  Serial.println("Getting time.");
  sendNTPpacket(timeServer); // send an NTP packet to a time server
  delay(1000);
  if(  Udp.parsePacket() ) get_and_print_time();
}

void loop()
{
  delay(1000);

  time_to_string(now(), time_as_string);
  Serial.print("Current time: ");
  Serial.println(time_as_string);
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










