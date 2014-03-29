#include <ANT.h>
#include <LCD_5110.h>

#define ANTPLUS_NETWORK_KEY  {0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45}

UCHAR aucNetKey[8] = ANTPLUS_NETWORK_KEY;
LCD_5110 screen(49,47,45,43,41);
uint8_t  ch;


void setup()
{
	ANT_Init(0,57600);
	delay(1000);
	ANT_ResetSystem();
	
	digitalWrite(51,HIGH);
	digitalWrite(53,HIGH);
    
    screen.begin();
    screen.clearDisplay();
    screen.VLOG("Init:");

	screen.display();
    delay(2000);
    ANT_SetNetworkKey(0,aucNetKey);
	
}
void loop()
{
	ANT_SetNetworkKey(0,aucNetKey);
	delay(1000);
	/*
	if(ANT_Read(ch))
	{
		screen.VLOG("Rx:");
		screen.Append(ch);
		screen.display();
	}
	*/
}
