# Smart-Water-Management-System
> ### **Final Project 2021**

![system pic](https://user-images.githubusercontent.com/72353423/143723893-957b3019-5a33-4039-9fa1-18d72518ff67.jpeg)

## Acknowledgement
I acknowledge the efforts of my project partner Mr Thoenes Kimani and my project supervisor Mr Samson N. Njoroge, for ensuring this project was successful. Many thanks to all our friends and family for the encouragement and support throughout the period.

## Abstract
The Smart Water Management System project aggregates three meters to enable remote and automatic real-time measurement of water consumption. Based on actionable decisions such as water leakage and consumption metrics, water supply through the pipes is controlled using solenoid valves. The consumer and the supplier can monitor, visualize consumption metrics and control the system through the designed android interface.

## System Overview
The system consists of four functional modules, namely: the physical device, the gateway through which the device communicates with the backend service, the backend that allows storage of data, analysis, data access and notifications and the interface that allows visualization and control.

![image](https://user-images.githubusercontent.com/72353423/143724646-886294ce-60d6-4c2c-a9ac-f0be3e5e747e.png)

## System Diagram
The system consists of three meters: the supply meter, 1 and customer meters, 2 and 3 as shown below.

![image](https://user-images.githubusercontent.com/72353423/143724678-79026a4c-a063-4c91-a661-bc4c0a2afb88.png)

## Flow Chart
The system’s flow chart diagram is as shown in figure below.

![image](https://user-images.githubusercontent.com/72353423/143724711-998635d7-13cf-4d9c-80db-d8c720d86887.png)

## System Components

1. **Three NodeMCU ESP8266** - [Component Overview](https://components101.com/development-boards/nodemcu-esp8266-pinout-features-and-datasheet)
2. **Three YF-S201 Hall Effect Water Flow Sensor** - [Component Overview](https://components101.com/sensors/yf-s201-water-flow-measurement-sensor)
3. **Three 2 Way, 2 Position, 12 VDC, 0.5 Inch, NC Solenoid Valve** - [Particular](https://www.twinschip.com/Solenoid_Valve_DC12V_0.5inch_Normally_Closed) ~~*Not an affiliate link~~
4. **Three SRD-05VDC-SL-C Relay Module** - [Component Overview](https://elearn.ellak.gr/mod/book/view.php?id=4568&chapterid=2440)
5. **Three Power Supply Adapter 12V 2A**
6. **0.5 Inch Clear hose pipe - approx. 3 meters**
7. **One 0.5 Inch Plastic tap**
8. **Appropriate pipe fittings**

## Schematic Diagram
The schematic diagram of the system was designed using Autodesk's Eagle EDA software illustrating how the various components are interconnected. Custom libraries for the solenoid valve and the water flow sensor were created.

![image](https://user-images.githubusercontent.com/72353423/143725199-f4645274-cabb-4db8-8399-71c8b1603733.png)

## Firmware Walkthrough


