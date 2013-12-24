/*** RouterReset: test if internet is working; if not, reset router
***/

#include <SPI.h>
#include <WiFi.h>

// private info [made generic here]
char ssid[] = "SSID";
char pass[] = "password";
// end of private info

int status = WL_IDLE_STATUS;
char server[] = "www.google.com";
//IPAddress server(74,125,225,113); // numeric IP for Google
//IPAddress server(192, 168, 1, 155); // numeric IP that doesn't work

WiFiClient client;

unsigned long lastConnectionTime = 0;
boolean lastConnected = false;
const unsigned long postingInterval = 10*1000; // wait 10 seconds
boolean canConnect = false;

const int redLed = 3;
const int greenLed = 5;
const int yellowLed = 6;


void setup() {
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

    delay(3000);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();
}

void loop() {
  flash_3leds(redLed, greenLed, yellowLed, 5, 100);

  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    Serial.println("Client avaliable.");
    Serial.println("Disconnecting...");
    lastConnectionTime=millis();
    client.stop();
  }

  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    connect_to_server();
  }
  
  lastConnected = client.connected();
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

  for(j=0; j<3; j++)
    pinMode(pins[j], OUTPUT);

  for(i=0; i<number; i++) {
    for(j=0; j<3; j++)
      digitalWrite(pins[j], HIGH);
    delay(time);
    for(j=0; j<3; j++)
      digitalWrite(pins[j], LOW);
    delay(time);
  }
}

