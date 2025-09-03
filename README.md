# robot-line-following-fuzzycontroller-fpga
DESIGN OF  FUZZY LOGIC CONTROLLER FOR LINE FOLLOWER ROBOT ON FPGA PLATFORM

This project was carried out as part of my studies in Computer Engineering Technology.
The goal was to design and implement an autonomous line-following robot using an FPGA platform combined with a fuzzy logic controller for smooth navigation on curved paths.
Objective: To design and build an autonomous line-following robot using FPGA, combined with a fuzzy logic controller to ensure accurate movement along curved paths.

##Main Features:

  -Controller: FPGA (Cyclone IV E: EP4CE6F17C8)
  
  -Algorithm: Fuzzy Logic Control
  
  -Sensors: Infrared sensors for line detection
  
  -Motor Control: PWM-based speed control

  -Node-RED User Interface (Start/Stop control, Speed reference adjustment, Real-time position monitoring (mapview), User authentication required, Real-time speed monitoring)
  
##Images:

![Robot demo](https://github.com/user-attachments/assets/490cc25d-d87f-484e-aeae-83f3e02b5d64)

##Node-RED User Interface:

-user authentication

![Node-RED Dashboard 1](https://github.com/user-attachments/assets/4c4c78c9-e1ae-40e7-ac71-ea2e127e6ebf)  

-map view

![Node-RED Dashboard 2](https://github.com/user-attachments/assets/e49a2138-b46d-4d37-b29c-0560e550f03e)  

-user interface

![Node-RED Dashboard 3](https://github.com/user-attachments/assets/07e96d71-61f3-4da9-be40-4b92c86df026)  

##Result:

PWM Generation: The PWM system operates stably at 200 Hz, ensuring effective motor speed control.

Fuzzy Logic Control: Experimental results show that the fuzzy controller enables the robot to accurately follow various track shapes (right angles, circles, triangles, curves). The robot automatically reduces speed when entering turns for smoother navigation.

Control Interface: The Node-RED interface is user-friendly, integrating essential functions such as robot control, real-time charts (measured vs. reference speed), and position tracking on a map.

Data Communication: Real-time communication is achieved via the MQTT protocol. Control signals are delivered promptly, though network instability may cause minor delays.

Data Monitoring: Speed data is logged and visualized on ThingSpeak. Although there is a ~1 second delay, it does not significantly affect system performance.

Power Supply: The integrated charge/discharge circuit allows the robot to be powered on with a single button and supports USB Type-C charging.

GPS Functionality: Using the Neo-6M V2 module, GPS data is continuously updated on the map. Accuracy depends on the environment—outdoor areas provide higher precision, while indoor use is not supported.

##Author 

Nguyen Hoang Long - Computer Engineering Technology Student - Industrial University of Ho Chi Minh City

Nguyen Thanh Duc - Computer Engineering Technology Student - Industrial University of Ho Chi Minh City
