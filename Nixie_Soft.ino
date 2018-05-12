/*
 * 
 * Product:  Nixie Clock
 * 
 * Description: This is the firmware to run the nixie clock system
 * Important information for this is as follows:
 * 
 * BCD assignments
 *    
 *     D2 Minute
 *     B0 N1-A
 *     B1 N1-B
 *     B2 N1-C
 *     B3 N1-D
 *     
 *     D1 Minute
 *     C0 N2-A
 *     C1 N2-B
 *     C2 N2-C
 *     C3 N2-D
 *     
 *     D2 Hour
 *     D4 N3-A
 *     D5 N3-B
 *     D6 N3-C
 *     D7 N3-D
 *     
 *     D1 hour
 *     D0 N4-A
 *     D1 N4-B
 *     D2 N4-C
 *     D3 N4-D
 * 
 * 
 * Author: M. Sperry
 * Date: 02/14/2018
 */

 #include <Wire.h>
 #include "RTClib.h"

 RTC_DS1307 rtc;

 #define HOUR 13
 #define MIN 12

 int disp=0, buttonstate = 0;
 int Hour_D1 = 0, Hour_D2 = 0, Min_D1 = 0, Min_D2 = 0;
 int timer = 0;

int translate(int xfer_num)
{
  //perform translation.
  switch (xfer_num)
  {
    case 0:
        return 1;
    case 1:
        return 0;
    case 2:
        return 9;
    case 3:
        return 8;
    case 4:
        return 7;
    case 5:
        return 6;
    case 6:
        return 5;
    case 7:
        return 4;
    case 8:
        return 3;
    case 9:
        return 2;
    default:
        return 0;   
  }
}

void DisplayTime(int GetTimeHour, int GetTimeMin)
{
  //this will dipslay the time by splitting the two digit numbers into the different
  //dispay variables for displaying on each port
  if (GetTimeHour > 9)
  {
    Hour_D1 = GetTimeHour/10;
    Hour_D2 = GetTimeHour%10;
  }
  else
  {
    Hour_D1 = 0;
    Hour_D2 = GetTimeHour;
  }

  if (GetTimeMin > 9)
  {
    Min_D1 = GetTimeMin/10;
    Min_D2 = GetTimeMin%10;
  }
  else
  {
    Min_D1 = 0;
    Min_D2 = GetTimeMin;
  }

  //Display Values
  PORTB = translate(Min_D2);
  PORTC = translate(Min_D1);
  PORTD = translate(Hour_D2) <<4 | translate(Hour_D1);  //need to combine the two for the single port
}

void setup() {
  // put your setup code here, to run once:
  DDRD = 0xFF;
  DDRB = 0x0F;
  DDRC = 0xFF;
  PORTD = 0x00;
  PORTB = 0x00;

  rtc.begin();

  // following line sets the RTC to the date & time this sketch was compiled
  //rtc.adjust(DateTime(__DATE__, __TIME__));
  rtc.adjust(DateTime(2018, 5, 10, 21, 26));

  //Set pin modes for buttons
  pinMode(HOUR,INPUT);
  pinMode(MIN,INPUT);

  
}

void loop() {
  // put your main code here, to run repeatedly:

  int xfer_num = 0, GetTimeHour = 0, GetTimeMin = 0, PortCSave = 0, NewHour = 0, NewMin = 0, SetTimer = 0;
  int curr_month = 0, curr_day = 0, curr_year = 0;

  //Get the date and time
  DateTime now = rtc.now();

  GetTimeHour = now.hour();
  GetTimeMin = now.minute();
  
  //Display the numbers
  DisplayTime(GetTimeHour, GetTimeMin);

  //check to see if both buttons are pressed and enter program mode
  if (!digitalRead(HOUR) && !digitalRead(MIN))
  {
    //wait for buttons to be released
    while (!digitalRead(HOUR) || !digitalRead(MIN));
    
    //get the current time displayed
    NewHour = GetTimeHour;
    NewMin = GetTimeMin;

    //while the inactivity time out has not expired.
    while (SetTimer != 500)
    {
      //check to see if thehour button is pressed.
      if (!digitalRead(HOUR))
      {
        while(!digitalRead(HOUR));
        NewHour++;
        if (NewHour > 23)
        {
          NewHour = 0;
        }
        DisplayTime(NewHour, NewMin);
        SetTimer = 0;
      }
      //check if the minute button is pressed
      else if (!digitalRead(MIN))
      {
        while (!digitalRead(MIN));
        NewMin++;
        if (NewMin > 59)
        {
          NewMin = 0;
        }
        DisplayTime(NewHour, NewMin);
        SetTimer = 0;
      }
      //otherwise if no buttons are started add one to inactivity timer
      else
      {
        SetTimer++;
        delay(10);
      }
    }
    //remember current date so that does not change
    curr_year = now.year();
    curr_month = now.month();
    curr_day = now.day();
    //adjust the time accordingly.
    rtc.adjust(DateTime(curr_year,curr_month,curr_day,NewHour,NewMin));
  }

}
