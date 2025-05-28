#ifndef PULSE_SENSOR_H
#define PULSE_SENSOR_H

#include <avr/io.h>

#define GRAPH_WIDTH 128
#define GRAPH_HEIGHT 64

typedef struct {
    float filtered_value;
    float prev_value;
    float raw_values[GRAPH_WIDTH];
    int bpm;
    unsigned long last_peak_time;
    int waiting_for_fall;
    float alpha;
    float detection_threshold;
    int measurement_active;
} pulse_sensor_t;

void pulse_sensor_init(pulse_sensor_t* sensor);
void pulse_sensor_update(pulse_sensor_t* sensor, uint16_t raw_adc);
void pulse_sensor_process_bpm(pulse_sensor_t* sensor);
void pulse_sensor_get_graph_data(pulse_sensor_t* sensor, float* min_val, float* max_val);
void pulse_sensor_start_stop(pulse_sensor_t* sensor);

#endif // PULSE_SENSOR_H