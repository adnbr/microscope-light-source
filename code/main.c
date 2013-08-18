/* ---------------------------------------------------------------------
 * PWM LED Brightness control for ATtiny13.
 * 
 * Pin configuration -
 * PB1/OC0B: LED output (Pin 6)
 * PB2/ADC1: Potentiometer input (Pin 7)
 * 
 * Find out more: 
 * http://www.adnbr.co.uk/articles/upgrading-a-to-led-illumination-pt-1
 * -------------------------------------------------------------------*/

// 9.6 MHz, built in resonator
#define F_CPU 9600000
 
#include <avr/io.h>

// Define led ext output pin on PB2
#define LED PB1 

int led_brightness;

int main(void)
{
    // LED is an output.
    DDRB |= (1 << LED);
    
    // ADC CODE
    // -----------------------------------------------------------------
    
    // Set ADC prescaler to clock/128.
    // At 9.6 MHz this is 75 kHz.
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    
    // ADC input is PB2/ADC1. || Only use ADCH, essential an 8 bit ADC.
    ADMUX |= (1 << MUX0) | (1 << ADLAR);
    
    // ADC is in auto-triggering free-running mode. || ADC is enabled.
    ADCSRA |= (1 >> ADATE) | (1 << ADEN);

    // Start ADC conversions.
    ADCSRA |= (1 << ADSC) | (1 << ADIF); 
    
    // PWM CODE
    // -----------------------------------------------------------------
    
    // Set Timer 0 prescaler to clock/8
    TCCR0B |= (1 << CS00);
    
    // Set to 'Fast PWM' mode
    TCCR0A |= (1 << WGM01) | (1 << WGM00);

    TCCR0A |= (1 << COM0B1) | (1 << COM0B0);
    
    // Set PWM to 0 brightness.
    OCR0B = 0;
    
    
    
    // Infinite loop
    while (1) {

        // Get the latest ADC reading
        led_brightness = ADCH;
        
        // We want an "off" position to supplement the physical off
        // power switch we added. This range roughly matches the "off" 
        // position on the original intensity control knob.
        if (led_brightness <= 20) {
            led_brightness = 0;
        }
        
        OCR0B = led_brightness;
    }
}
