/*
 * APP.c
 *
 * Created: 11-Dec-22 9:5007:5009 AM
 *  Author: Mahmoud Ahmed Ibrahim
 */ 
#include "APP.h"

/* Declare The PushButton */
#define BUTTON_PIN INT0
	
/* Declare Cars LEDs */
ST_Led_t CarsGreenLED = {PORT_A, PIN0};
ST_Led_t CarsYellowLED = {PORT_A, PIN1};
ST_Led_t CarsRedLED = {PORT_A, PIN2};

/* Declare Pedestrian LEDs */	
ST_Led_t PedestriansGreenLED = {PORT_B, PIN0};
ST_Led_t PedestriansYellowLED = {PORT_B, PIN1};
ST_Led_t PedestriansRedLED = {PORT_B, PIN2};
 
/* Declare some global variables to be used */
uint8_t blink_cntr = 0;	// counter for blinking yellow LEDs
uint8_t pressed = 0;	// defines whether the button is already pressed (we still in ISR)
uint8_t state;			// current state which is one from four states ( CarsPeriod, FromCarsToPedestrian, PestrianPeriod, FromPedestrianToCars)
extern uint32_t rest;	// Last value of the rest before the interrupt happens

/* Initialize the Inputs and Outputs */
void initialize(void)
{
	// Initialize cars LEDs
	LED_init(&CarsRedLED);
	LED_init(&CarsYellowLED);
	LED_init(&CarsGreenLED);
	
	// Initialize pedestrians LEDs
	LED_init(&PedestriansRedLED);
	LED_init(&PedestriansYellowLED);
	LED_init(&PedestriansGreenLED);
		
	//Enable global Interrupt
	GINT_Enable();
	// Initialize Button
	BUTTON_interrupt(BUTTON_PIN, RISING_EDGE);
}

void NormalMode(void)
{
	CarsPeriod();
	FromCarsToPedestrian();
	PestrianPeriod();
	FromPedestrianToCars();
}

void PedestrianMode(void)
{
	uint32_t old_rest = rest;
	uint8_t doublePress = 0;
	// if the button already pressed and this is second press in short period
	if (pressed == 1)
		doublePress = 1;
	else
		doublePress = 0;

	if(doublePress == 0)
	{
		pressed = 1;
		if (state == 1)
		{
			// If we are in state 1 we jump to state 2, then 3, then 4
			FromCarsToPedestrian();
			PestrianPeriod();
			FromPedestrianToCars();
			
			/* Before the ISR is called the main program was executing CarsPeriod function
			 * So before we return back, we will execute first part of it so that 
			 * when we return to main and the CarsPeriod function execution completes 
			 * it will appear that the program continues running normally 
			 */
			LED_turnON(&CarsGreenLED);
			LED_turnON(&PedestriansRedLED);
			TIMER0_delay_ms(5000-old_rest);
		}
		else if (state == 2)
		{
			FromCarsToPedestrian();
			PestrianPeriod();
			FromPedestrianToCars();
			CarsPeriod();
			
			/* Before the ISR is called the main program was executing FromCarsToPedestrian function
			 * So before we return back, we will execute first part of it so that 
			 * when we return to main and the FromCarsToPedestrian function execution completes 
			 * it will appear that the program continues running normally 
			 */
			LED_turnOFF(&CarsGreenLED);
			LED_turnON(&PedestriansRedLED);
			LED_turnON(&CarsYellowLED);
			LED_turnON(&PedestriansYellowLED);
			for (blink_cntr=0; blink_cntr<9; blink_cntr++)
			{
				TIMER0_delay_ms(500);
				LED_toggle(&CarsYellowLED);
				LED_toggle(&PedestriansYellowLED);
			}
		}
		else if (state == 3 || state == 4)
		{
			/* Do nothing */
		}
		pressed = 0;
	}
	rest = old_rest;
}


/* Five seconds Cars->Green	Pedestrian->Red */
void CarsPeriod(void)
{
	state = 1;
	LED_turnON(&CarsGreenLED);
	LED_turnON(&PedestriansRedLED);
	TIMER0_delay_ms(5000);
	LED_turnOFF(&CarsGreenLED);
	LED_turnOFF(&PedestriansRedLED);
}

/* Five seconds both yellow blink, Pedestrian->Red */
void FromCarsToPedestrian(void)
{
	state = 2;
	LED_turnOFF(&CarsGreenLED);
	LED_turnON(&PedestriansRedLED);
	LED_turnON(&CarsYellowLED);
	LED_turnON(&PedestriansYellowLED);
	for (blink_cntr = 0; blink_cntr<10; blink_cntr++)
	{
		TIMER0_delay_ms(500);
		LED_toggle(&CarsYellowLED);
		LED_toggle(&PedestriansYellowLED);
	}
	LED_turnOFF(&CarsYellowLED);
	LED_turnOFF(&PedestriansYellowLED);
	LED_turnOFF(&PedestriansRedLED);
	
}

/* Five seconds Cars->Red	  Pedestrian->Green */
void PestrianPeriod(void)
{
	state = 3;
	LED_turnON(&CarsRedLED);
	LED_turnON(&PedestriansGreenLED);
	TIMER0_delay_ms(5000);
	LED_turnOFF(&CarsRedLED);
	LED_turnOFF(&PedestriansGreenLED);
}

/* Five seconds both yellow blink, Pedestrian->Green */
void FromPedestrianToCars(void)
{
	state = 4;
	LED_turnON(&PedestriansGreenLED);
	LED_turnON(&CarsYellowLED);
	LED_turnON(&PedestriansYellowLED);
	for (blink_cntr = 0; blink_cntr<10; blink_cntr++)
	{
		TIMER0_delay_ms(500);
		LED_toggle(&CarsYellowLED);
		LED_toggle(&PedestriansYellowLED);
	}
	LED_turnOFF(&CarsYellowLED);
	LED_turnOFF(&PedestriansYellowLED);
	LED_turnOFF(&PedestriansGreenLED);
}

/* Interrupt service routine for INT0 interrupt */
ISR(INT0_vect)
{
	PedestrianMode();
}