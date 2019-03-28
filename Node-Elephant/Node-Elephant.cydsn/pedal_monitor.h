#ifndef PEDAL_MONITOR_H
#define PEDAL_MONITOR_H

#include "uart-terminal.h"
#include <stdint.h>

typedef enum {
	monitor_calibrate_notCalibrating,
	monitor_calibrate_waiting,
	monitor_calibrate_printTitle,
	monitor_calibrate_t1,
	monitor_calibrate_t2,
	monitor_calibrate_b1,
	monitor_calibrate_b2,
	monitor_calibrate_done
} monitor_calibrate_item;

void monitor_init();
void monitor_calibrate_update(monitor_calibrate_item item, double volts, int16_t ADC_low, int16_t ADC_high);

#endif
