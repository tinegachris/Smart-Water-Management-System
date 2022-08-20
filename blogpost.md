---
title: Smart Water Management System
domain: blog.chrispinetinega.com 
tags: Smart water system, Chrispine Tinega, IoT
subtitle: Final Project
/* The pathname of your article url 
* Ex. In https://engineering.hashnode.com/how-we-autodetect-spam-using-googles-vertex-ai 
"how-we-autodetect-spam-using-googles-vertex-ai" is the slug */ 
slug: smart-water-system
/* You need to upload your image to https://hashnode.com/uploader 
and use the uploaded image URL as COVER_IMAGE_URL */ 
cover: https://cdn.hashnode.com/res/hashnode/image/upload/v1660997561622/8FiyUe9cX.jpeg?auto=compress
/* When you have a team publication and a publication member has created an article */ 
/* Please note that this param is only supported while creating an article and not updating */ 
publishAs: Chrispine Tinega
/* When your article is republished and you want to use 
 the origin url as canonical url */ 
canonical: CANONICAL_URL_OF_ARTICLE 
/* Only if you don't want this article to be published yet */ 

---
## Abstract
The Smart Water Management System project aggregates three meters to enable remote and automatic real-time measurement of water consumption. Based on actionable decisions such as water leakage and consumption metrics, water supply through the pipes is controlled using solenoid valves. The consumer and the supplier can monitor, visualize consumption metrics and control the system through the designed android interface.

## Acknowledgement
I acknowledge the efforts of my project partner Mr [Thoenes Kimani](https://www.linkedin.com/in/thoenes-kimani-607682186/) and my project supervisor Mr [Samson N. Njoroge](https://www.linkedin.com/in/samson-n-njoroge-23a77817/?originalSubdomain=ke), for ensuring this project was successful. Many thanks to all our friends and family for the encouragement and support throughout the period.

## System Overview
The system consists of four functional modules, namely: the physical device, the gateway through which the device communicates with the backend service, the backend that allows storage of data, analysis, data access and notifications and the interface that allows visualization and control.

>![image](https://user-images.githubusercontent.com/72353423/143724646-886294ce-60d6-4c2c-a9ac-f0be3e5e747e.png)

## System Diagram
The system consists of three meters: the supply meter, 1 and customer meters, 2 and 3 as shown below.

>![image](https://user-images.githubusercontent.com/72353423/143724678-79026a4c-a063-4c91-a661-bc4c0a2afb88.png)

## Flow Chart
The system’s flow chart diagram is as shown in the figure below.

>![image](https://user-images.githubusercontent.com/72353423/143724711-998635d7-13cf-4d9c-80db-d8c720d86887.png)

## System Components

1. **Three NodeMCU ESP8266** - [Component Overview](https://components101.com/development-boards/nodemcu-esp8266-pinout-features-and-datasheet)
2. **Three YF-S201 Hall Effect Water Flow Sensor** - [Component Overview](https://components101.com/sensors/yf-s201-water-flow-measurement-sensor)
3. **Three 2 Way, 2 Position, 12 VDC, 0.5 Inch, NC Solenoid Valve** - [Particular](https://www.twinschip.com/Solenoid_Valve_DC12V_0.5inch_Normally_Closed) ~~Not an affiliate link~~
4. **Three SRD-05VDC-SL-C Relay Module** - [Component Overview](https://elearn.ellak.gr/mod/book/view.php?id=4568&chapterid=2440)
5. **Three Power Supply Adapter 12V 2A**
6. **0.5 Inch Clear hose pipe - approx. 3 meters**
7. **One 0.5 Inch Plastic tap**
8. **Appropriate pipe fittings**

## Schematic Diagram

The schematic diagram of the system was designed using Autodesk's Eagle EDA software illustrating how the various components are interconnected. 

Custom libraries for the solenoid valve and the water flow sensor were created.

>![sch](https://user-images.githubusercontent.com/72353423/145267859-94879ac2-70a4-43a9-8a8a-849b70cd0a8e.jpg)

## PCB Design

Design of the board from the schematic diagram was done as shown below. 

>![brd](https://user-images.githubusercontent.com/72353423/145268059-74bae5a3-d1b7-4ab7-b7ba-0f6ab30b571d.jpg)

## Firmware Walkthrough

- **Libraries**


The ESP8266WiFi, ESP8266WebServer, ESP8266mDNS and the ThingSpeak libraries were included.

```
#include <ESP8266WiFi.h> // ESP8266WiFi library
#include<ESP8266WebServer.h> // WebServer library
#include <ESP8266mDNS.h> // mDNS library
#include<ThingSpeak.h> // ThingSpeak library
```
- **Variables and Constants**

The variables and constants were defined as follows.
```
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
const int controlChannelID = XXXXXXX; //ThingSpeak channel used for control
unsigned int valve1 = 4;
```

- **Wi-Fi Settings**

The SSID and password are set to allow the NodeMCU to connect to a network
```
const char* ssid = "mySSID"; // wireless network name (SSID)
const char* password = "myPassword"; // Wi-Fi network password
```

- **ThingSpeak Settings**

ThingSpeak credentials and API key are set to allow sending data to specific channel.
```
// ThingSpeak Settings
const int channelID = YYYYYYY;
String writeAPIKey = "XXXXXXXXXXXXXXXX";
const char* server = "api.thingspeak.com";
```

- **Serial Communication**

The baud rate for serial communication  set was to 115200.
```
  void setup() {
  Serial.begin(115200); // Start the Serial communication
  delay(100);
```

- **Define pins and initialize variables**

```
  pulseCount_1 = 0;
  flowRate_1 = 0.0;
  flowMilliLitres_1 = 0;
  totalMilliLitres_1 = 0;
  
  pinMode(flowSensor_1, INPUT_PULLUP);  
  pinMode(relayValve_1, OUTPUT);
  digitalWrite(relayValve_1, 0);
```

- **Connect to Wi-Fi network**

The NodeMCU waits until it is connected to a network then sends the IP address, Netmask and Gateway to the serial monitor.
```
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
```

- **Measure Wi-Fi Strength**

The Wi-Fi strength is measured and sent to the serial monitor.
```
// Measure Signal Strength (RSSI) of Wi-Fi connection
long rssi = WiFi.RSSI();
String strgth = String(rssi);
Serial.println("signal strength: " + strgth + "dB\n");
```

- **HTTP server**

The HTTP server that listens for HTTP request on port 80. The mDNS responder is also started to resolve IP address to a local domain name.
```
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
```

- **FLow Sensor Interrrupt**

An interrupt and a call function is created to detect water flow when the falling edge of the pulse is received.
```
//create interrupt and call a function when change is detected
  attachInterrupt(digitalPinToInterrupt(flowSensor_1), pulseCounter_1, FALLING);
```
The pulse count is incremented for every pulse that is detected.
```
void IRAM_ATTR pulseCounter_1(){
  pulseCount_1++;
}
```

- **Flowrate and Volume Calculation**

The flowrate and volume through the meter is measured and calculated after every measuring interval. The total volume is the cumulative volume in the entire measuring time.
```
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
```

- **API request body**

The body of API request with consumption data are constructed waiting to be posted to ThingSpeak on specific fields.
```
      // Construct API request body
      String body = "field1=";
      body += String(float(flowRate_1));
      body += "&field2=";
      body += String(totalMilliLitres_1);
      body += "&field3=";
      body += strgth;
      body += "\r\n\r\n";
```

- **Post to ThingSpeak**

The API requests bodies are written to the corresponding fields on ThingSpeak on the specified channel. Feedback on if the data was sent successfully is sent to the serial monitor.
```
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
```

- **Solenoid Valve Status**

The status of solenoid valve i.e. if it is open or closed is checked from the control feed and status sent to the serial monitor.
```
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
```

- **Wait and Post Again**

```
    client.stop();
    
    Serial.print("Waiting ");
    Serial.print(postingInterval/1000);
    Serial.print(" seconds to post again...\n");
    
    // wait and then post again
    delay(postingInterval);
```

## **ThingSpeak Apps**

- ### MATLAB Analysis 

The MATLAB Analysis app was used to perform loss analysis as shown below.

>![image](https://user-images.githubusercontent.com/72353423/143726415-56e53196-edf4-4f54-a19c-f3d33ab226ca.png)

- ### ThingTweet

The ThingTweet app is utilized to send alert consumers via a tweet that the supply meter valve is closed due to any predetermined circumstance is encountered. The tweet is configured as below.

>![image](https://user-images.githubusercontent.com/72353423/143726436-f2adc767-de1d-419e-afd9-4d635c46bcc0.png)

- ### TimeControl

TimeControl app feature was used in loss analysis calculation where losses due to leakages was calculated every 5 minutes as shown in the configuration below.

>![image](https://user-images.githubusercontent.com/72353423/143726462-ee1d3176-6987-41e1-803f-9ecf218db4f1.png)

- ### React

React app feature was also used in the project in several instances such as sending email, SMS and VoIP calls when a certain condition is encountered.

>![image](https://user-images.githubusercontent.com/72353423/143726570-775248b6-6de2-47a6-9685-e00a263ea586.png)

## **Interface**
The android app used as interface to visualize consumption data and control water supply remotely was designed using [MIT App Inventor](https://appinventor.mit.edu/).

The [.aia source code](https://github.com/tinegachris/Smart-Water-Management-System/blob/main/Smart_Water_App.aia) file is attached. 

Download and install the [android app](https://github.com/tinegachris/Smart-Water-Management-System/blob/main/Smart_Water_App.app)

> ![login](https://user-images.githubusercontent.com/72353423/143765739-63cd2895-2d36-458e-8adc-cd486289b141.jpeg)                   ![3](https://user-images.githubusercontent.com/72353423/143766149-68ce4067-780e-4de3-880f-32bce4c8cbf7.jpeg)

## Results

The system utilized IoT solutions to enable remote and automatic and real-time measurement of water consumption data which is transmitted to a remote server for manipulation and analysis. 

>![image](https://user-images.githubusercontent.com/72353423/143767085-4a4b431b-d1fb-4f2b-97e7-3d9114339855.png)

The data was then displayed on an interface where consumption metrics, over a period, was be viewed. 

> ![1](https://user-images.githubusercontent.com/72353423/143765805-cafc4de6-42e1-433c-a72a-d6aa19b62829.jpeg)                        ![2](https://user-images.githubusercontent.com/72353423/143765811-6a02cca4-9843-4eb9-853c-c3ec72c49c56.jpeg)

The system developed aggregated multiple meters and performed loss analysis hence detecting water leakage. SMS, call, email and [tweet](https://twitter.com/smart_water_app/status/1463631460607344641?t=qJkpamW6DDiH7IFD41dn9Q&s=19) alerts allowed appropriate actions to be taken. 

> ![tweet](https://user-images.githubusercontent.com/72353423/145271730-a4b6c52e-1eab-4cfb-8faf-6397be69628d.jpeg)        ![image](https://user-images.githubusercontent.com/72353423/143766895-02cd9f7e-3cf2-47e9-b354-a3d82ffe1718.png)       ![image](https://user-images.githubusercontent.com/72353423/143768181-25a4407a-3a68-4cc4-81e2-e6e915688a84.png)

Users could control water supply remotely from the interface based on actionable decisions.

> ![closed](https://user-images.githubusercontent.com/72353423/145272087-e0d91bce-a0c5-4418-b2a8-77307b85ffaa.jpeg)
> ![opened](https://user-images.githubusercontent.com/72353423/145272118-303abfb3-8c7b-46ad-967f-6e103b692afe.jpeg)
> ![image](https://user-images.githubusercontent.com/72353423/143767154-0f6dbd26-84ef-49ef-926b-4a0874bd9bb8.png)

## Conclusion

The Smart Water Management System solution would be revolutionary for home and industrial systems by enabling remote and automatic measurement of consumption data, convenient monitoring and control of water systems and reducing water leakage - reducing non-revenue water hence saving on cost.

##
