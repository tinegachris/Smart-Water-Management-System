/* *****SMART WATER MANAGEMENT SYSTEM*****

  This project aggregates 3 flow sensors, sends consumption data to ThingSpeak IoT platform for visualization,
  controls water flow based on actionable decisions and detects water leakage.

  Author(s): Chrispine Tinega
          Thoenes Kimani

  Last Edited: Monday November 22, 2021

  All Rights Reserved.
*/

#include <ESP8266WiFi.h> // ESP8266WiFi library
#include <ESP8266WebServer.h> // WebServer library
#include <ESP8266mDNS.h> // mDNS library
#include <ThingSpeak.h> // ThingSpeak library

// Pin Definitions
#define LED_BUILTIN 16 // GPIO 16 i.e. D0
#define FLOW_SENSOR_1 5 // flowsensor of meter 1 connected to D1 i.e. GPIO 5
#define RELAY_VALVE_1 4 // relay input signal pin connected to D2 i.e GPIO 4

// Timing Constants
const int MEASURING_INTERVAL = 100; // measure flow every 0.1 second
const int POSTING_INTERVAL = 15000; // post data every 15 seconds

// Calibration Factor
const float CALIBRATION_FACTOR_1 = 0.1;

// Wi-Fi Settings
const char* SSID = "whisperygeek"; // wireless network name (SSID)
const char* PASSWORD = "qwerty9516"; // Wi-Fi network password

// ThingSpeak Settings
const int CHANNEL_ID = 1583388;
const String WRITE_API_KEY = "Y7A8COY3CXGDW83A";
const char* SERVER = "api.thingspeak.com";

// Control solenoid valve
const int CONTROL_CHANNEL_ID = 1583388;
const unsigned int VALVE_1 = 4;

// Global Variables
long currentMillis = 0, previousMillis = 0;
volatile byte pulseCount_1;
byte pulse1Sec_1 = 0;
float flowRate_1;
unsigned long flowMilliLitres_1, totalMilliLitres_1;
float flowLitres_1, totalLitres_1;

WiFiClient client; // create a Wi-Fi client to connect to ThingSpeak

void IRAM_ATTR pulseCounter_1() {
  pulseCount_1++;
}

void connectToWiFi() {
  WiFi.begin(SSID, PASSWORD); // Connect to the WiFi network
  Serial.print("Connecting to ");
  Serial.print(SSID); Serial.println("...");

  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
   delay(500);
  }

  Serial.println("WiFi connected!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  Serial.print("Netmask:\t");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway:\t");
  Serial.println(WiFi.gatewayIP());
}

void setupMDNS() {
  if (MDNS.begin("esp8266")) {
   Serial.println("mDNS responder started");
  } else {
   Serial.println("Error setting up MDNS responder!");
  }
}

void setup() {
  Serial.begin(115200); // Start the Serial communication
  delay(100);

  pulseCount_1 = 0;
  flowRate_1 = 0.0;
  flowMilliLitres_1 = 0;
  totalMilliLitres_1 = 0;

  pinMode(FLOW_SENSOR_1, INPUT_PULLUP);
  pinMode(RELAY_VALVE_1, OUTPUT);
  digitalWrite(RELAY_VALVE_1, LOW);

  Serial.println('\n');

  connectToWiFi();
  setupMDNS();

  // Create a webserver object that listens for HTTP request on port 80
  ESP8266WebServer server(80);
  server.begin(); // Actually start the server
  Serial.println("HTTP server started");

  // Create interrupt and call a function when change is detected
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_1), pulseCounter_1, FALLING);

  ThingSpeak.begin(client);
}

void sendDataToThingSpeak() {
  if (client.connect(SERVER, 80)) {
   // Measure Signal Strength (RSSI) of Wi-Fi connection
   long rssi = WiFi.RSSI();
   String strgth = String(rssi);
   Serial.println("Signal strength: " + strgth + "dB\n");

   // Construct API request body
   String body = "field1=" + String(float(flowRate_1)) +
            "&field2=" + String(totalMilliLitres_1) +
            "&field3=" + strgth + "\r\n\r\n";

   client.print("POST /update HTTP/1.1\n");
   client.print("Host: " + String(SERVER) + "\n");
   client.print("Connection: close\n");
   client.print("X-THINGSPEAKAPIKEY: " + WRITE_API_KEY + "\n");
   client.print("Content-Type: application/x-www-form-urlencoded\n");
   client.print("Content-Length: " + String(body.length()) + "\n\n");
   client.print(body);
   client.print("\n\n");
   Serial.println("Data sent\n");

   // Get the last data of the fields
   int valve_1 = ThingSpeak.readFloatField(CONTROL_CHANNEL_ID, VALVE_1);

   if (valve_1 == 1) {
    digitalWrite(RELAY_VALVE_1, HIGH); // Open solenoid valve
    Serial.println("Solenoid valve is Closed!\n");
   } else if (valve_1 == 0) {
    digitalWrite(RELAY_VALVE_1, LOW); // Close solenoid valve
    Serial.println("Solenoid valve is Open!\n");
   }

   client.stop();
  }
}

void loop() {
  currentMillis = millis();
  // Calculate after every measuring interval
  if (currentMillis - previousMillis > MEASURING_INTERVAL) {
   pulse1Sec_1 = pulseCount_1;
   pulseCount_1 = 0;

   // Calculate flow rate in litres per min
   flowRate_1 = ((1000.0 / (millis() - previousMillis)) * pulse1Sec_1) / CALIBRATION_FACTOR_1;
   previousMillis = millis();

   // Flow rate in millilitres per second
   flowMilliLitres_1 = (flowRate_1 / 60) * 1000;

   // Flow rate in litres per second
   flowLitres_1 = (flowRate_1 / 60);

   // Add volume flowed in this second to cumulative totals
   totalMilliLitres_1 += flowMilliLitres_1;
   totalLitres_1 += flowLitres_1;

   // Print the flow rate 1 for this second in litres / minute
   Serial.print("Flow rate 1: ");
   Serial.print(flowMilliLitres_1);
   Serial.print(" mL/s\t");

   // Print the cumulative total of litres flowed through meter 1 since starting
   Serial.print("Volume through meter 1: ");
   Serial.print(totalMilliLitres_1);
   Serial.print(" mL\n");

   sendDataToThingSpeak();

   Serial.print("Waiting ");
   Serial.print(POSTING_INTERVAL / 1000);
   Serial.print(" seconds to post again...\n");

   // Wait and then post again
   delay(POSTING_INTERVAL);
  }
}
