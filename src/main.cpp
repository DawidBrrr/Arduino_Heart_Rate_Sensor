#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart.h"  
#include "timer.h"  
#include "adc.h"
#include "i2c.h"
#include "display.h"
#include "pulse_sensor.h"   

#define BUTTON_PIN 2
#define SENSOR_PIN 0  // ADC0

pulse_sensor_t pulse_sensor;
volatile uint8_t button_pressed = 0;

void button_init(void) {
    // Set PIN2 as input with pull-up
    DDRD &= ~(1 << BUTTON_PIN);  // Input
    PORTD |= (1 << BUTTON_PIN);  // Pull-up enabled
    
    // Configure INT2 for falling edge trigger
    EICRA &= ~(1 << ISC20);     // Clear bit
    EICRA |= (1 << ISC21);      // Set bit for falling edge
    EIMSK |= (1 << INT2);       // Enable INT2
    
    // Clear any pending interrupt
    EIFR |= (1 << INTF2);
}

ISR(INT2_vect) {
    static unsigned long last_interrupt_time = 0;
    unsigned long current_time = millis();
    
    if (current_time - last_interrupt_time > 200) {
        pulse_sensor_start_stop(&pulse_sensor);
        uart_puts("Button pressed! Active: "); // Debug output
        uart_print_int(pulse_sensor.measurement_active);
        uart_puts("\r\n");
        last_interrupt_time = current_time;
    }
    
    EIFR |= (1 << INTF2);
    _delay_us(100);
}

void setup(void) {
    cli();
    
    uart_init();
    timer0_init();
    adc_init();
    i2c_init();
    display_init();
    pulse_sensor_init(&pulse_sensor);
    button_init();

    // Force active state at startup
    pulse_sensor.measurement_active = 1;
    uart_puts("Forcing active state\r\n");
    
    sei();
    
    uart_puts("System initialized\r\n");
}

int main(void) {
    setup();
    
    unsigned long last_display_update = 0;
    unsigned long last_uart_update = 0;
    
    while (1) {
        unsigned long current_time = millis();
        
        // Process sensor data only when measurement is active
        uint16_t sensor_value = adc_read(SENSOR_PIN);
        if (pulse_sensor.measurement_active) {
            pulse_sensor_update(&pulse_sensor, sensor_value);
            pulse_sensor_process_bpm(&pulse_sensor);
        }
        
        // Update display every 30ms
        if (current_time - last_display_update >= 10) {
            display_clear();
            
            if (pulse_sensor.measurement_active) {
                float min_val, max_val;
                pulse_sensor_get_graph_data(&pulse_sensor, &min_val, &max_val);
                
                // Add range check to prevent division by zero
                if (max_val > min_val) {
                    for (int i = 0; i < GRAPH_WIDTH-1; i++) {
                        // Invert the y coordinates by subtracting from GRAPH_HEIGHT
                        int y1 = GRAPH_HEIGHT - (int)((pulse_sensor.raw_values[i] - min_val) * (GRAPH_HEIGHT-10) / (max_val - min_val));
                        int y2 = GRAPH_HEIGHT - (int)((pulse_sensor.raw_values[i+1] - min_val) * (GRAPH_HEIGHT-10) / (max_val - min_val));
                        
                        // Limit y values to prevent overflow
                        y1 = (y1 > GRAPH_HEIGHT-1) ? GRAPH_HEIGHT-1 : (y1 < 0) ? 0 : y1;
                        y2 = (y2 > GRAPH_HEIGHT-1) ? GRAPH_HEIGHT-1 : (y2 < 0) ? 0 : y2;
                        
                        display_line(i, y1, i+1, y2, 1);
                    }
                }
            } else {
                display_string(0, 20, "Press button to start");
            }
            
            // Always show BPM
            display_string(0, 0, "BPM:");
            display_number(30, 0, pulse_sensor.bpm);
            display_update();
            
            last_display_update = current_time;
        }
        /*
        // Send debug data every 1000ms
        if (current_time - last_uart_update >= 1000) {
            uart_puts("BPM: ");
            uart_print_int(pulse_sensor.bpm);
            uart_puts(" Raw: ");
            uart_print_int((int)sensor_value);  // Print raw ADC value instead of filtered
            uart_puts(" Active: ");
            uart_print_int(pulse_sensor.measurement_active);
            uart_puts("\r\n");
            last_uart_update = current_time;
        }
        */
    }
    
    return 0;
}