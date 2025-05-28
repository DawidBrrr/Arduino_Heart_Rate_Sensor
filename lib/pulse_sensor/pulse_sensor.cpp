#include "pulse_sensor.h"
#include "timer.h"
#include <string.h>

void pulse_sensor_init(pulse_sensor_t* sensor) {
    sensor->filtered_value = 0;
    sensor->prev_value = 0;
    sensor->bpm = 0;
    sensor->last_peak_time = 0;
    sensor->waiting_for_fall = 0;
    sensor->alpha = 0.2; // Low-pass filter coefficient
    sensor->detection_threshold = 1.0; // Adjusted threshold
    sensor->measurement_active = 0;
    
    // Initialize raw values array
    memset(sensor->raw_values, 0, sizeof(sensor->raw_values));
}

void pulse_sensor_update(pulse_sensor_t* sensor, uint16_t raw_adc) {
    if (!sensor->measurement_active) return;
    
    // Apply low-pass filter
    sensor->filtered_value = sensor->alpha * raw_adc + (1 - sensor->alpha) * sensor->filtered_value;
    
    // Shift array left
    for (int i = 0; i < GRAPH_WIDTH - 1; i++) {
        sensor->raw_values[i] = sensor->raw_values[i + 1];
    }
    sensor->raw_values[GRAPH_WIDTH - 1] = sensor->filtered_value;
}

void pulse_sensor_process_bpm(pulse_sensor_t* sensor) {
    if (!sensor->measurement_active) return;
    
    unsigned long now = millis();
    float delta = sensor->filtered_value - sensor->prev_value;
    sensor->prev_value = sensor->filtered_value;
    
    // Peak detection
    if (!sensor->waiting_for_fall && delta > sensor->detection_threshold) {
        sensor->waiting_for_fall = 1;
        
        if (sensor->last_peak_time > 0) {
            unsigned long interval = now - sensor->last_peak_time;
            
            // Valid heart rate range: 30-200 BPM
            if (interval > 300 && interval < 2000) {
                sensor->bpm = 60000 / interval;
            }
        }
        sensor->last_peak_time = now;
    }
    else if (sensor->waiting_for_fall && delta < -sensor->detection_threshold) {
        sensor->waiting_for_fall = 0;
    }
    
    // Reset BPM if no peak detected for 3 seconds
    if (now - sensor->last_peak_time > 3000) {
        sensor->bpm = 0;
    }
}

void pulse_sensor_get_graph_data(pulse_sensor_t* sensor, float* min_val, float* max_val) {
    *min_val = sensor->raw_values[0];
    *max_val = sensor->raw_values[0];
    
    for (int i = 1; i < GRAPH_WIDTH; i++) {
        if (sensor->raw_values[i] < *min_val) *min_val = sensor->raw_values[i];
        if (sensor->raw_values[i] > *max_val) *max_val = sensor->raw_values[i];
    }
    
    // Ensure we have some range to work with
    if (*max_val == *min_val) {
        *max_val = *min_val + 1;
    }
}

void pulse_sensor_start_stop(pulse_sensor_t* sensor) {
    sensor->measurement_active = !sensor->measurement_active;
    if (!sensor->measurement_active) {
        sensor->bpm = 0;
        sensor->last_peak_time = 0;
        sensor->waiting_for_fall = 0;
    }
}