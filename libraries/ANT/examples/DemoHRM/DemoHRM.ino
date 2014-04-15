
#include <ANT.h>
#include <LCD_5110.h>

#define ANTPLUS_NETWORK_KEY  {0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45}
#define HRM_DEVICETYPE   0x78
#define HRM_RFFREQUENCY  0x39   //Set the RF frequency to channel 57 - 2.457GHz
#define HRM_MESSAGEPERIOD  8070    //Set the message period to 8070 counts specific for the HRM


UCHAR aucNetKey[8] = ANTPLUS_NETWORK_KEY;
LCD_5110 screen(49,47,45,43,41);
uint8_t  ch;
char stepN=1;



void step(char stepN)
{
	switch(stepN)
	{
	case 1:
		ANT_SetNetworkKey(0,aucNetKey);
		break;
	case 2:
		ANT_AssignChannel(0,0,0);
		break;
	case 3:
		ANT_SetChannelId(0, 0, HRM_DEVICETYPE, 0);
		break;
	case 4:
		ANT_SetChannelRFFreq(0, HRM_RFFREQUENCY/*USER_RADIOFREQ*/);
		break;
	case 5:
		ANT_SetChannelPeriod(0,HRM_MESSAGEPERIOD);
		break;
	case 6:
		ANT_OpenChannel(0);
		break;
	default:
		break;
		
	}	
}

void setup()
{

	pinMode(14,OUTPUT);
	
	digitalWrite(51,HIGH);
	digitalWrite(53,HIGH);
	
	digitalWrite(14,LOW);
	delay(500);
	digitalWrite(14,HIGH);

	ANT_Init(0,57600);
	delay(1000);
	ANT_ResetSystem();
	    
    screen.begin();
    screen.clearDisplay();
    screen.VLOG("Init:");

	screen.display();
    delay(8000);
    
    step(1);
    stepN++;
    /*
    ANT_SetNetworkKey(0,aucNetKey);
    delay(5000);
    ANT_AssignChannel(0,0,0);
    
    delay(5000);
    ANT_SetChannelId(0, 0, HRM_DEVICETYPE, 0);
    
    delay(5000);
    ANT_SetChannelRFFreq(0, HRM_RFFREQUENCY);
    
    delay(5000);
    ANT_SetChannelPeriod(0,HRM_MESSAGEPERIOD);
    
    delay(5000);
    ANT_OpenChannel(0);
    */
    
    screen.VLOG("Rx:");
    screen.VLOG("");
    screen.display();
	
}
void loop()
{
	delay(20);	
	while(ANT_Read(ch))                                                                             
	{
		screen.Append(String(ch,HEX));
		screen.Append(',');
		screen.display();
	}
	delay(1000);
	if(stepN<=6)
	{
	  step(stepN);stepN++;
	}
	
}
