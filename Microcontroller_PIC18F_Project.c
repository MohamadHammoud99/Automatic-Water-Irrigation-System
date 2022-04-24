#include <stdio.h>
#include <p18f458.h>

#pragma config WDT = OFF

#define ldata PORTD            //PORTD = LCD data pins
#define   rs   PORTEbits.RE0   //rs = PORTE.0
#define   rw   PORTEbits.RE1   //rw = PORTE.1
#define   en   PORTEbits.RE2   //en = PORTE.2

#define   c1   PORTBbits.RB0
#define   c2   PORTBbits.RB1
#define   c3   PORTBbits.RB2

#define   r1   PORTBbits.RB3
#define   r2   PORTBbits.RB4
#define   r3   PORTBbits.RB5
#define   r4   PORTBbits.RB6

#define   PUMP1    PORTCbits.RC0
#define   PUMP2    PORTCbits.RC1
#define   GLED     PORTCbits.RC2
#define   RLED     PORTCbits.RC3

#define   ESENSOR   PORTAbits.RA4
#define   FSENSOR   PORTAbits.RA5

//Functions Definitions

void MSDelay(unsigned int);    

void lcdcmd(unsigned char);
void lcddata(unsigned char);
void lcd_ini (void);
void LCD_write_string(static char *str);

unsigned int  Temperature (void);
unsigned int  MOISTURE (void);

void chk_tank (void);
void IRRIGATION_AUTO2 (unsigned int );
void IRRIGATION_MAN (void);

void Stop (void);
void SError (void);
void ALLError(void);

void main()
{
    unsigned int e;
    unsigned char msg1[] = "1:AUTOMATIC";   
    unsigned char msg2[] = "2:MANUAL 3:STOP";
    //unsigned char msg3[] = "PROCESS OFF";
    //unsigned char msg4[] = "SENSOR ERROR";  
    //unsigned char msg5[] = "FIILING TANK";
    //unsigned char msg6[] = "IRRIGATING";
    //unsigned char msg7[] = "RESET SYSTEM";
    //unsigned char msg8[] = "AFTER SOLVING";
  
    TRISA = 0b11111111;     //Make PORTA input
    TRISB = 0b10000000;     //Make PORTB output  
    TRISC = 0b00000000;     //Make PORTB output
    TRISD = 0b00000000;     //Make PORTD output
    TRISE = 0b00000000;     //Make PORTE output
    
    PORTB = 0b11111111;     //Initialize PORTB at ONES
    PORTC = 0b00000000;     //Initialize PORTB at ZEROS
        
    ADCON1 = 0x0F;          //Configuring the PORTE pins as digital I/O
   
    while(1)                //LOOP
    {
    PORTB = 0xFF;           //Or PORTB=11111111 C1=C2=C3=1 are initialized by the Keypad
    r1 = r2 = r3 = r4 = 0;  //Initializing the Rows
    //Or PORTB = 0x0F
 
    GLED = 1;       //Turn ON Green LED (Process ON)
    RLED = 0;       //Turn OFF Red LED (Process ON)
 
    MSDelay(25);                //Waits for a short amount of time to for a key to be pressed
    if (c1 == 0)                //Detect 1st column (key '1' of Keypad) ---> Automatic Mode
    { 
      chk_tank();               //Call the chk_tank function to check the tank level and fill its initially before the irrigation process 
      MSDelay(250);             //Call the MSDelay function
        while(1)                //Loop for the Automatic Irrigation process
        {   
            e =  MOISTURE() ;           //Call the MOISTURE function that displays the moisture value and compare it with 70% and
                                        //take the convenient decision 
            IRRIGATION_AUTO2(e);    //Call the IRRIGATION_AUTO function
            MSDelay(10);                
        }  
    }
    MSDelay(400);
    PORTB = 0xFF;               //Re-initializing the Columns (of Keypad)
    r1 = r2 = r3 = r4 = 0;      //Initializing the Rows
    //Or PORTB = 0x0F
    
    GLED = 1;               //Turn ON Green LED (Process ON)
    if (c2 == 0)            //Detect 2nd column (key '2' of Keypad) ---> Manual Mode
        {  
            IRRIGATION_MAN();   //Call the IRRIGATION_MAN function
            MSDelay(10);
        }
    MSDelay(50);
    PORTB = 0xFF;               //Initializing the Columns
    r1 = r2 = r3 = r4 = 0;      //Initializing the Rows
                            
    if (c3 == 0) {              //Detect 3rd column (key '3' of Keypad) ---> Stop all processes
                   Stop();      //Call the Stop function to stop all Processes
                 }
 
    lcd_ini();                  //Initializing the LCD
    LCD_write_string(msg1);     //Call the LCD_write_string function to write "1:AUTOMATIC" on the Screen
    lcdcmd(192);                //Set the Cursor on 2nd line in LCD screen
    LCD_write_string(msg2);     //Call the LCD_write_string function to write "2:MANUAL 3:STOP" on the Screen
    MSDelay(1500);              //Delay to display these messages for more time to the user
    }
}

//FUNCTIONS BODY

void chk_tank (void)            //Function that check if the tank is full or no in order to fill it
   {    
        unsigned char msg5[] = "FIILING TANK";
        
        if(FSENSOR != 1 || ESENSOR !=1)     //Check if the Full or Empty Sensor switches detect water    
        {
        while(FSENSOR != 1)     //Check if the Full Sensor switch detects water
            {
            PUMP1 = 1;              //Activate the Tank Filling Pump
            PUMP2 = 0;              //Deactivate the Irrigation Pump
            lcd_ini();              //Initializing the LCD  
            LCD_write_string(msg5); //Call the LCD_write_string function to write "FIILING TANK" on the Screen   
            MSDelay(400);           //Delay to display these messages for more time to the user
            }
            PUMP1 = 0;              //Deactivate the Tank Filling Pump the Full Sensor detects water
            MSDelay(25);  
 
        if(c2 == 0) {               //This condition is related to the IRRIGATION_MAN function to re-check tank in some conditions
                        PUMP2 = 0;          //Deactivate the Irrigation Pump
                        MSDelay(250);    
                        IRRIGATION_MAN();   //Re-Call the IRRIGATION_MAN function
                    }           
            PUMP1 = 0;  //Deactivate the Tank Filling Pump the Full Sensor detects water
        }
        else
            PUMP1 = 0;  //Deactivate the Tank Filling Pump the Full Sensor detects water
   }

void IRRIGATION_AUTO2 (unsigned int t)      //Function that is responsible for the Automatic Water Irrigation Process, Controlled by several sensors 
{                                           //'t' is the actual Moisture value;
    unsigned int t1, m1;
    unsigned char msg6[] = "IRRIGATING";   

    ALLError();        //Call the ALLError Function to detect ALL possible Errors(Sensors errors and Warning to high temperature)
    if( t < 50 && ESENSOR == 1 )          //'t' is the actual Moisture value;
    {
        MSDelay(100); 
        PORTB = 0xFF;               //Initializing the Columns (of Keypad)
        r1 = r2 = r3 = r4 = 0;      //Initializing the Rows
        //Or PORTB = 0x0F
        if (c3 == 0) { 
                        Stop();     //Call the Stop function to stop all Processes
                    }        
        MSDelay(100);
        PORTB = 0xFF;               //Re-initializing the Columns (of Keypad)
        r1 = r2 = r3 = r4 = 0;      //Initializing the Rows
        //Or PORTB = 0x0F
        MSDelay(100);
        PUMP2 = 1;          //Activate the Irrigation Pump
        GLED = 1;           //Turn ON Green LED (Process ON)
        RLED = 0;           //Turn OFF Red LED (Process ON)
        lcd_ini();          //Initializing the LCD
        MSDelay(20);
        LCD_write_string(msg6);     //Call the LCD_write_string function to write "IRRIGATING" on the Screen
        MSDelay(400);               //Delay to display these messages for more time to the user
    }
    if(t > 70 || ESENSOR != 1 )      //'t' is the actual Moisture value;
    {   
        MSDelay(100);
        PORTB = 0xFF;               //Initializing the Columns (of Keypad)
        r1 = r2 = r3 = r4 = 0;      //Initializing the Rows
        //Or PORTB = 0x0F
        
        if (c3 == 0) { 
                        Stop();     //Call the Stop function to stop all Processes
                    } 
        MSDelay(100);
        PORTB = 0xFF;               //Re-initializing the Columns (of Keypad)
        r1 = r2 = r3 = r4 = 0;      //Initializing the Rows
        //Or PORTB = 0x0F
        
        MSDelay(100);
        PUMP2  = 0 ;            //Deactivate the Irrigation Pump
        MSDelay(100);
        ALLError();             //Call the ALLError Function to detect ALL possible Errors(Sensors errors and Warning to high temperature)
        chk_tank();             //Call the chk_tank function to check the tank level and fill it if it is required
        t1 = Temperature();     //Call the Temperature function that displays the temperature value
    }
    
        m1 = MOISTURE();
        lcd_ini();              //Initializing the LCD
        t1 = Temperature();     //Call the Temperature function that displays the temperature value
        MSDelay(20);
}

void IRRIGATION_MAN (void)      //Function that is responsible for the Manual Irrigation Process, Controlled by the user
{    
    unsigned char msg6[] = "IRRIGATING";
    unsigned int t1 , m2 ;
        
    while(ESENSOR  == 1)        //Tank is not fully Empty
    {   
        SError();               //Call the SError Function to detect Sensors Errors
        if(c2 == 0){            //Check if the key '2' of Keypad is pressed ---> Manual Mode
                     if(FSENSOR == 1) PUMP1 = 0;         //Deactivate the Tank Filling Pump if the Full Sensor detects water
                    }                          
        MSDelay(100);
        PORTB=0xFF;                 //Re-initializing the Columns (of Keypad)
        r1 = r2 = r3 = r4 = 0;      //Initializing the Rows
        //OR PORTB = 0x0F
        if (c3 == 0) Stop();        //Check if the key '3' of Keypad is pressed ---> Stop all processes, Turn OFF Green LED,
                                    //Turn ON Red LED and Print "PROCESS OFF" On the Screen
        MSDelay(100);
        PORTB=0xFF;                 //Re-initializing the Columns (of Keypad)
        r1 = r2 = r3 = r4 = 0;      //Initializing the Rows
        //OR PORTB = 0x0F
        
        
        PUMP2 = 1;      //Activate the Irrigation Pump
        GLED = 1;       //Turn ON Green LED (Process ON)
        RLED = 0;       //Turn OFF Red LED (Process ON)
        
        lcd_ini();      //Initializing the LCD
        MSDelay(15);
        LCD_write_string(msg6);     //Call the LCD_write_string function to write "IRRIGATING" on the Screen
        MSDelay(200);               //Delay to display these messages for more time to the user
        m2 = MOISTURE ();           //Call the MOISTURE function that displays the moisture value and compare it with 70% and
                                    //take the convenient decision       
        MSDelay(80);
        lcdcmd(192);                //Set the Cursor on 2nd line in LCD screen
        t1 = Temperature();         //Call the Temperature function that displays the temperature value 
    }
  if(ESENSOR != 1)      //Tank is fully Empty
    {   SError();       //Call the SError Function to detect Sensors Errors
        chk_tank();     //Call the chk_tank function to check the tank level and fill it if it is required 
        }
  }


    unsigned int Temperature (void)         //Function that reads from Temperature Sensor, Display his value on the screen
                                            //and returns his value(We can compare it with a certain value and
                                            //take decisions if it is required from the user)
    {                                       
        unsigned char y, z = 0;
        unsigned int h_byte, l_byte, temp, b9, p, b8;
        unsigned char name[] = "Temperature";
        unsigned int a, b, x0, x2;
        unsigned char c = 0x30;     //For Conversion to ASCII
    
        lcd_ini();                  //Initializing the LCD
        for(y=192;y<203;y++)        //Change (increment) positions of Cursor from 0xC0 - D '192'(2nd line) to finishing the word "Temperature"
        {
        lcdcmd(0xC0);           //Set the Cursor on 2nd line in LCD screen
        MSDelay(15);
        lcdcmd(y);              //Set Cursor before printing   
        MSDelay(15);
        lcddata(name[z]);       //Print "TEMPERATURE" On LCD
        z++;
                        }
        lcdcmd(203);            //Set the Cursor after the word "Temperature" 
        MSDelay(15);
        lcddata('#');           //Print "#" On LCD
        TRISAbits.TRISA1 = 1;   //RA1(AN1) Analog INPUT
        TRISAbits.TRISA3 = 1;   //RA3 Used FOR Vref(external) 
        ADCON0 = 0b10001001;    //Programming ADCON0
        ADCON1 = 0xC5;          //Programming ADCON1
        MSDelay(250);

        MSDelay(1);             //Taq ---> signal stable
        ADCON0bits.GO = 1;              //Start Conversion
        while(ADCON0bits.DONE == 1);    //Wait the Conversion Process to be finished
        l_byte = ADRESL;                //then, we put the result (10 bits) in low byte (ADRESL) and high byte (ADRESH),                                                                                                                  
        h_byte = ADRESH;                //then we put the 2 least significant bits of the high byte in b8 and b9 (unsigned int).
        b8 = h_byte & 0x01;             //then, we check the values of these two bits (1 or 0).
        b9 = h_byte & 0x02;             //then, we add them all in temp variable (unsigned int).
        if(b8 > 0)                      //after that, we use a convenient coefficient to get real result (Hexadecimal).
            b8 = 256;     //2^8                  //.....
        else
            b8 = 0;
        
        if(b9 > 0)
            b9 = 512;     //2^9 
        else
            b9 = 0; 
        
        temp = b9 + b8 + l_byte;
        temp = temp * 0.1465;       //.............//
        p = temp ;
        a = temp;                   //Separating the Temperature value to Tens and Ones to
        b = a / 10;                 //display it character-by-character
        x0 = a % 10;
        x0 = c | x0;
        x2 = c | b;
    
        MSDelay(15);
        lcdcmd(205);
        MSDelay(15);
        MSDelay(15);
        lcddata(x2);
        MSDelay(15);                //Print Temperature value with its Unit (C)
        lcdcmd(206);
        MSDelay(15);
        lcddata(x0);
        MSDelay(15);
        lcdcmd(207);
        lcddata('C');
        MSDelay(15);
        MSDelay(1000);
        
        return p ;
    }

unsigned int  MOISTURE (void)           //Function that reads from Moisture Sensor,
 {                                      //display his actual value on the screen and returns this value to compare it later on
    unsigned int d, y, z = 0;
    unsigned char name[] = "Moisture";
    unsigned int h_byte, l_byte, temp, u, b9, b8;
    unsigned char i,Hundreds,Tens,Ten;
     
    TRISAbits.TRISA0 = 1;       //RA0(AN0) Analog Input
    ADCON0=0b10000001;          //Programming ADCON0
    ADCON1=0b11000100;          //Programming ADCON1
    MSDelay(250);
    ADCON0bits.GO = 1;          //Start Conversion
    while(ADCON0bits.DONE == 1);        
    l_byte = ADRESL;
    h_byte = ADRESH;
    b8 = h_byte & 0x01;
    b9 = h_byte & 0x02;
    if(b8 > 0)
      b8 = 256;     //2^8
    else                         //Similar as Temperature Function but now for Moisture
      b8 = 0;
    if(b9 > 0)
      b9 = 512;     //2^9
    else
      b9 = 0; 
    temp = b9 + b8 + l_byte;
    d = (temp/(1.024));
    lcd_ini();
    for(y=128;y<139;y++)
    {
    lcdcmd(y);
    MSDelay(15);
    lcddata(name[z]);           //Print "MOISTURE" On LCD
    z++;
    }
    lcdcmd(0x89);               //Goto 9th place on first line of LCD
    MSDelay(15);
    i = d / 100;
    u = d / 10 ;                //This action is for take the integer value of the Moisture and compare it with 70(%) 
    Hundreds = i + 0x30;        
    lcddata(Hundreds);
    MSDelay(15);
    i = (d % 100 ) / 10;        //Separating the Moisture value to Tens and Ones to
    Ten = i + 0x30;             //display it character-by-character
    Tens = Ten + 1;
    lcddata (Tens);
    MSDelay(15);
    lcddata ('%');              //Display %
    MSDelay(1000);
    
    return u;                   //Returns the actual Moisture value to compare it later on
}

void MSDelay(unsigned int itime)   //Function used to make time Delay
{
     unsigned int i, j;
     for(i=0;i<itime;i++)
        for(j=0;j<135;j++);
}

void lcdcmd(unsigned char c)    //Function used to send command to the display
{
    ldata = c;          //PORTD = LCD data pins
    rs = 0;             //To send command
    rw = 0;             //To write on LCD
    en = 1;             //HIGH-to_LOW pulse for the E pin in the case of writing
    MSDelay(1);
    en = 0;
}

void lcddata(unsigned char d)    //Function used to send data to the display and print it.
{
    ldata = d;          //PORTD = LCD data pins
    rs = 1;             //To send data
    rw = 0;             //To write on LCD
    en = 1;             //HIGH-to_LOW pulse for the E pin in the case of writing
    MSDelay(1);
    en = 0;
}

void lcd_ini (void)    //Function used to initialize the LCD
{
    MSDelay(250);
    en = 0;
    lcdcmd(0x01);       //Clears the LCD display 
    MSDelay(15);
    lcdcmd(0x38);       //Make 2 lines and 5 x 7 matrix in LCD
    MSDelay(15);
    lcdcmd(0x0E);       //Make Cursor blinking ON
}

void LCD_write_string(static char *str)     //Function used to Print String (Message) on the Screen of the LCD
{
    unsigned int i = 0;
    while (str[i] != 0)
        {
            lcddata(str[i]);        //Sending Data on LCD byte-by-byte
            MSDelay(15);
            i++;
        }
    MSDelay(250);
}
    
void Stop (void)            //Function used to check if the key '3' of Keypad is pressed ---> Stop all processes, Turn OFF Green LED,
                            //Turn ON Red LED and Print "PROCESS OFF" On the Screen
{
    unsigned char msg3[] = "PROCESS OFF";
     if (c3 == 0)    //When key '3' is pressed, both pumps were be deactivated
   {
        PUMP2 = 0;      //Deactivate the Irrigation Pump
        PUMP1 = 0;      //Deactivate the Tank Filling Pump if the Full Sensor detects water
        RLED = 1;       //Turn ON Red LED (Process OFF)
        GLED = 0;       //Turn OFF Green LED (Process OFF)
        lcd_ini();                  //Initializing the LCD
        LCD_write_string(msg3);     //Call the LCD_write_string function to write "PROCESS OFF" on the Screen
        MSDelay(8000);              //Delay to display these messages for more time to the user
    }
}

void ALLError(void)         //Function that detect the Sensors Errors those are possible, Also the warning of high temperature in Automatic Mode;
                            //for example, if the Full Sensor detects water but the Empty Sensor does not detect it!!!
                            //Also if the temperature exceeds 40 degree Celsius ----> Warning!!!
    {
        unsigned char msg4[] = "SENSOR ERROR";
        unsigned char msg7[] = "RESET SYSTEM";
        unsigned char msg8[] = "AFTER SOLVING";
        unsigned char msg9[] = "SENSORS PROBLEM";  
        unsigned char msg10[] = "BE CAREFUL";
        unsigned char msg11[] = "HIGH TEMPERATURE";
        unsigned char msg12[] = "IRRIGATE MORE";
        unsigned char msg13[] = "MANUALLY BE SAFE";
        
        
        unsigned int t3, m3;
        
        if(FSENSOR == 1 && ESENSOR != 1)        //The Full Sensor detects water but the Empty Sensor does not detect it!!!
                {
                while(1){
                        c3 = 0;
                        Stop();         //Stop all processes
                        lcd_ini();      //Initializing the LCD
                        MSDelay(15);
                        LCD_write_string(msg4);     //Call the LCD_write_string function to write "SENSOR ERROR" on the Screen
                        MSDelay(200);               //Delay to display these messages for more time to the user
                        lcdcmd(192);
                        LCD_write_string(msg7);     //Call the LCD_write_string function to write "RESET SYSTEM" on the Screen
                        MSDelay(1000);              //Delay to display these messages for more time to the user
                        lcd_ini();                  //Initializing the LCD
                        MSDelay(15);
                        LCD_write_string(msg8);     //Call the LCD_write_string function to write "AFTER SOLVING" on the Screen
                        MSDelay(200);               //Delay to display these messages for more time to the user
                        lcdcmd(192);
                        LCD_write_string(msg9);     //Call the LCD_write_string function to write "SENSORS PROBLEM" on the Screen
                        MSDelay(1000);              //Delay to display these messages for more time to the user         
                            }
                }
        t3 = Temperature();
        m3 = MOISTURE();
        if (t3 > 40)
        {
            while(1){
                        c3 = 0;         //Force pressing on stop key (in program without pressing physically on the keypad)
                        Stop();         //Stop all processes
                        lcd_ini();      //Initializing the LCD
                        MSDelay(15);
                        LCD_write_string(msg10);     //Call the LCD_write_string function to write "BE CAREFUL" on the Screen
                        MSDelay(200);                //Delay to display these messages for more time to the user
                        lcdcmd(192);                 //Jump to the 2nd line (Cursor)
                        LCD_write_string(msg11);     //Call the LCD_write_string function to write "HIGH TEMPERATURE" on the Screen
                        MSDelay(1000);               //Delay to display these messages for more time to the user
                        lcd_ini();                   //Initializing the LCD
                        LCD_write_string(msg7);      //Call the LCD_write_string function to write "RESET SYSTEM" on the Screen
                        MSDelay(1000);               //Delay to display these messages for more time to the user
                        lcd_ini();                   //Initializing the LCD
                        LCD_write_string(msg12);     //Call the LCD_write_string function to write "IRRIGATE MORE" on the Screen                                
                        lcdcmd(192);                 //Jump to the 2nd line (Cursor)            
                        MSDelay(15);                          
                        LCD_write_string(msg13);     //Call the LCD_write_string function to write "MANUALLY BE SAFE" on the Screen
                        MSDelay(1000);               //Delay to display these messages for more time to the user
        }
    }
}

void SError (void)      //Function that detect only the Sensors Errors
{
    unsigned char msg4[] = "SENSOR ERROR";
    unsigned char msg7[] = "RESET SYSTEM";
    unsigned char msg8[] = "AFTER SOLVING";
    unsigned char msg9[] = "SENSORS PROBLEM";
    
    if(FSENSOR == 1 && ESENSOR != 1)        //The Full Sensor detects water but the Empty Sensor does not detect it!!!
                {
                while(1){
                        c3 = 0;         //Force pressing on stop key (in program without pressing physically on the keypad)
                        Stop();         //Stop all processes
                        lcd_ini();      //Initializing the LCD
                        MSDelay(15);
                        LCD_write_string(msg4);     //Call the LCD_write_string function to write "SENSOR ERROR" on the Screen
                        MSDelay(200);               //Delay to display these messages for more time to the user
                        lcdcmd(192);                //Jump to the 2nd line (Cursor)
                        LCD_write_string(msg7);     //Call the LCD_write_string function to write "RESET SYSTEM" on the Screen
                        MSDelay(1000);              //Delay to display these messages for more time to the user
                        lcd_ini();                  //Initializing the LCD
                        MSDelay(15);
                        LCD_write_string(msg8);     //Call the LCD_write_string function to write "AFTER SOLVING" on the Screen
                        MSDelay(200);               //Delay to display these messages for more time to the user
                        lcdcmd(192);                //Jump to the 2nd line (Cursor)
                        LCD_write_string(msg9);     //Call the LCD_write_string function to write "SENSORS PROBLEM" on the Screen
                        MSDelay(1000);              //Delay to display these messages for more time to the user         
                            }
                }
}
