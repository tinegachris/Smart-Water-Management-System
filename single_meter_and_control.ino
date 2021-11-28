/* *****SMART WATER MANAGEMENT SYSTEM*****

   This project aggregates 3 flow sensors, sends consumption data to ThingSpeak IoT platform for visualization,
   controls water flow based on actionable decisions and detects water leakage.

   Author(s): Chrispine Tinega
              Thoenes Kimani

   Last Edited: Monday November 22, 2021

   All Rights Reserved.
*/

#include <ESP8266WiFi.h> // ESP8266WiFi library
#include<ESP8266WebServer.h> // WebServer library
#include <ESP8266mDNS.h> // mDNS library
#include<ThingSpeak.h> // ThingSpeak library

#define LED_BUILTIN 16 // GPIO 16 i.e. D0
#define flowSensor_1 5 // flowsensor of meter 1 connected to D1 i.e. GPIO 5
#define relayValve_1 4 // relay input signal pin connected to D2 i.e GPIO 4

const int measuringInterval = 0.1 * 1000; // measure flow every 1 second
const int postingInterval = 15 * 1000; // post data every 15 seconds

long currentMillis = 0, previousMillis = 0;

float calibrationFactor_1 = 0.1;
volatile byte pulseCount_1;
byte pulse1Sec_1 = 0;
float flowRate_1;
unsigned long flowMilliLitres_1, totalMilliLitres_1;
float flowLitres_1, totalLitres_1;

// Control solenoid valve
const int controlChannelID = 1583388;
unsigned int valve1 = 4;

// Wi-Fi Settings
const char* ssid = "whisperygeek"; // wireless network name (SSID)
const char* password = "qwerty9516"; // Wi-Fi network password

WiFiClient client; //create a Wi-Fi client to connect to ThingSpeak

// ThingSpeak Settings
const int channelID = 1583388;
String writeAPIKey = "Y7A8COY3CXGDW83A";
const char* server = "api.thingspeak.com";

void IRAM_ATTR pulseCounter_1(){
  pulseCount_1++;
}

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200); // Start the Serial communication
  delay(100);
 
  pulseCount_1 = 0;
  flowRate_1 = 0.0;
  flowMilliLitres_1 = 0;
  totalMilliLitres_1 = 0;
  
  pinMode(flowSensor_1, INPUT_PULLUP);  
  pinMode(relayValve_1, OUTPUT);
  digitalWrite(relayValve_1, 0);

  Serial.println('\n');
  
  WiFi.begin(ssid, password);             // Connect to the WiFi network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println("...");

  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(500);
  }
  
  Serial.println("WiFi connected!"); 
  // Send the IP address of the ESP8266 to serial monitor 
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); 
  // Send the Netmask of the ESP8266        
  Serial.print("Netmask:\t");
  Serial.println(WiFi.subnetMask());  
  // Send the Gateway of the ESP8266
  Serial.print("Gateway:\t");
  Serial.println(WiFi.gatewayIP());

  // Create a webserver object that listens for HTTP request on port 80
  ESP8266WebServer server(80);

  // Start the mDNS responder for esp8266.local
  if (MDNS.begin("esp8266")) {              
  Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  server.begin(); // Actually start the server
  Serial.println("HTTP server started");
  
  //create interrupt and call a function when change is detected
  attachInterrupt(digitalPinToInterrupt(flowSensor_1), pulseCounter_1, FALLING);

  ThingSpeak.begin(client);
}

void loop() {
    
  currentMillis = millis();
  // calculate after every measuring interval
  if (currentMillis - previousMillis > measuringInterval) {
    
    pulse1Sec_1 = pulseCount_1;
    pulseCount_1 = 0;

     //calculate flow rate in litres per min
    flowRate_1 = ((1000.0 / (millis() - previousMillis)) * pulse1Sec_1) / calibrationFactor_1;
    previousMillis = millis();

    //flowrate in millilitres per second
    flowMilliLitres_1 = (flowRate_1 / 60) * 1000;

    //flowrate in litres per second
    flowLitres_1 = (flowRate_1 / 60);

    //add volume flowed in this second to cumulative totals
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
//    Serial.println("Volume through meter 1: "); //Uncomment for volume in litres
//    Serial.print(totalLitres_1);
//    Serial.println(" L\n");
 
    if (client.connect(server, 80)) {
      
      // Measure Signal Strength (RSSI) of Wi-Fi connection
      long rssi = WiFi.RSSI();
      String strgth = String(rssi);
      Serial.println("signal strength: " + strgth + "dB\n");
      
      // Construct API request body
      String body = "field1=";
      body += String(float(flowRate_1));
      body += "&field2=";
      body += String(totalMilliLitres_1);
      body += "&field3=";
      body += strgth;
      body += "\r\n\r\n";

      
      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: " + writeAPIKey + "\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(body.length());
      client.print("\n\n");
      client.print(body);
      client.print("\n\n");
      Serial.println("data sent\n");
    
      //get the last data of the fields
      int valve_1 = ThingSpeak.readFloatField(controlChannelID, valve1);
    
      if(valve_1 == 1){
        digitalWrite(relayValve_1, 1); // Open solenoid valve
        Serial.println("Solenoid valve is Closed!\n");
        }
      else if(valve_1 == 0){
        digitalWrite(relayValve_1, 0); //Close solenoid valve
        Serial.println("Solenoid valve is Open!\n");
        }

      }
    
    client.stop();
    
    Serial.print("Waiting ");
    Serial.print(postingInterval/1000);
    Serial.print(" seconds to post again...\n");
    
    // wait and then post again
    delay(postingInterval);
  }
}
