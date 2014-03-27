#include <ANT.h>
#include <LCD_5110.h>

LCD_5110 screen(49,47,45,43,41);

void setup()
{
	ANT_Init(0,57600);
	ANT_ResetSystem();
	
	digitalWrite(51,HIGH);
	digitalWrite(53,HIGH);
    
    screen.begin();
    screen.clearDisplay();
      // text display tests
      screen.setTextSize(1);
      screen.setTextColor(BLACK);
     screen.VLOG("step 1:");
     screen.VLOG("step 2:");
     screen.VLOG("step 3:");
     screen.VLOG("step 4:");
     screen.VLOG("step 5:");
     screen.VLOG("step 6:");
     screen.VLOG("step 7:");
     screen.VLOG("step 8:");

	 screen.display();
     delay(2000);
	
}
void loop()
{
}
