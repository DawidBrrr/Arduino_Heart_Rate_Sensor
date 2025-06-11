#include <avr/io.h>
#include <avr/interrupt.h>

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
    DDRE &= ~(1 << PE4);     // PE4 Entry
    PORTE |= (1 << PE4);     // Pull-up on PE4

    // INT4 Falling edge
    EICRB &= ~(1 << ISC40);  // ISC40 = 0
    EICRB |= (1 << ISC41);   // ISC41 = 1

    EIFR |= (1 << INTF4);    // CLear INT4 flag
    EIMSK |= (1 << INT4);    // Enable INT4
}

ISR(INT4_vect) {
    static unsigned long last_interrupt_time = 0;
    unsigned long current_time = millis();

    if (current_time - last_interrupt_time > 200) {
        pulse_sensor_start_stop(&pulse_sensor);
        uart_puts("Button pressed\r\n");
        last_interrupt_time = current_time;
    }
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


    
    sei();
    
}

int main(void) {
    setup();
    
    unsigned long last_display_update = 0;    
    while (1) {
        unsigned long current_time = millis();
        
        //if measurment is active, read the sensor value
        uint16_t sensor_value = adc_read(SENSOR_PIN);
        if (pulse_sensor.measurement_active) {
            pulse_sensor_update(&pulse_sensor, sensor_value);
            pulse_sensor_process_bpm(&pulse_sensor);
        }
        
        //update screen every 40ms
        if (current_time - last_display_update >= 40) {
            
            if (pulse_sensor.measurement_active) {
                display_clear();
                float min_val, max_val;
                pulse_sensor_get_graph_data(&pulse_sensor, &min_val, &max_val);
                
                //check if we have valid min and max values
                if (max_val > min_val) {
                    for (int i = 0; i < GRAPH_WIDTH-1; i++) {
                        // turn y values upside down for display
                        int y1 = GRAPH_HEIGHT - (int)((pulse_sensor.raw_values[i] - min_val) * (GRAPH_HEIGHT-10) / (max_val - min_val));
                        int y2 = GRAPH_HEIGHT - (int)((pulse_sensor.raw_values[i+1] - min_val) * (GRAPH_HEIGHT-10) / (max_val - min_val));
                        
                        // Limit y values to the display height
                        y1 = (y1 > GRAPH_HEIGHT-1) ? GRAPH_HEIGHT-1 : (y1 < 0) ? 0 : y1;
                        y2 = (y2 > GRAPH_HEIGHT-1) ? GRAPH_HEIGHT-1 : (y2 < 0) ? 0 : y2;
                        
                        display_line(i, y1, i+1, y2, 1);
                    }
                }
            } else {
                display_clear();
                display_string(0, 20, "PRZYCISNIJ PRZYCISK");
                display_string(0,40,"ABY ROZPOCZAC POMIAR");
            }
            
            // BPM display
            display_string(0, 0, "BPM:");
            display_number(30, 0, pulse_sensor.bpm);    
            display_update();
            
            last_display_update = current_time;
        }
        //UART DO DEBUGOWANIA
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