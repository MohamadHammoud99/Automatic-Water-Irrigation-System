# Automatic-Water-Irrigation-System

- PIC18F458 Microcontroller 
- Team of 3 students

•	In this project, our objective was to program a PIC18F458 microcontroller to obtain an automatic (smart) irrigation system. We used the programming language 'C' with the software 'MPLAB X' and we simulated the code (the complete circuit of the system - including all components) with the software 'Proteus'.

•	Inputs:
  1.	Moisture sensor
  2.	Temperature sensor
  3.	Empty sensor
  4.	Full sensor
  5.	Keypad

•	Outputs:
  1.	Tank Filling Pump
  2.	Irrigation pump
  3.	LCD
  4.	LEDs

•	This project works in dual mode: Automatic & Manual
  1.	Automatic mode:
    o	At the beginning, the system checks the tank and fills it if necessary (activation of the tank filling pump)
    o	Then, if the humidity is < (50%) and the temperature < (40°C), the system starts to irrigate until the humidity becomes > (70%), then, the system goes into standby until the humidity becomes less than 50% again.
    o	The irrigation process stops if the user presses on the stop key (key ‘3’ on the keypad) or if a problem occurs (error or warning).
    o	During the process, the humidity and temperature are always displayed on the LCD screen.
    o	In this mode, when the temperature becomes > (40°C), the process stops and a warning message is displayed on the LCD.
    o	Note: (Both pumps are never active at the same time for electricity saving and other related details…).
  2.	Manual mode:
    o	In this mode, the process is completely controlled by the user, independently of the humidity and temperature values displayed on the LCD screen.
    o	The process stops when the user presses the Stop button or when an error occurs (Sensor error, etc.).
    o	This Mode can be very useful in the case of extremely hot days where the temperature > (40°C), if we want to irrigate more than the threshold programmed in the automatic mode (70%).
    o	When the tank becomes empty due to irrigation, after refilling it, press the '2' key to re-irrigate in this mode.
    o	Note: In manual mode, the priority is for irrigation while in Automatic mode, it is for filling the tank.

•	Messages are permanently displayed to facilitate the interface with the user and to inform him of the humidity and temperature values, of the system status (Irrigation, Tank Filling, Process OFF…), of the activation mode choices (Manual, Automatic, Stop), of the errors or warnings that may occur (Sensor error, High Temperature, …), and so on…

•	Output of the project:
  -	Report / Presentation
  -	Proteus file
  -	MPLAB code C file

•	Skills token from this project:
  -	Microcontroller (registers, timers, ADC, etc.)
  -	C coding 
  -	MPLAB X
  -	Proteus
  -	Components: 
      o	PIC18F458	                              
      o	2 Pumps	                                
o	Full & Empty Limit Switch sensors	      
o	Moisture & Temperature sensors	        
o	Keypad (4x3)
o	LCD (16x2)
o	Resistors
o	Battery
o	LEDs
o	Wires

![image](https://user-images.githubusercontent.com/85926752/164973658-c240ff05-5ad3-4d76-94a7-d58973eb0d5f.png)
