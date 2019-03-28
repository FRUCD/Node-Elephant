#include "pedal_monitor.h"
#include "pedal_monitor_state.h"
#include "pedal_monitor_status.h"

#define ROUTINE_NAME "calibrationData"
static uint8_t calibrationID = 0;
static uint8_t stateID = 0;
static uint8_t statusID = 0;

static monitor_calibrate_item currentCalibrationState = monitor_calibrate_notCalibrating;

static double current_volts = 0.0;
static int16_t t1_low = 0;
static int16_t t2_low = 0;
static int16_t b1_low = 0;
static int16_t b2_low = 0;
static int16_t t1_high = 0;
static int16_t t2_high = 0;
static int16_t b1_high = 0;
static int16_t b2_high = 0;

bool monitor_showCalibRoutine();

void monitor_init() {
	calibrationID = terminal_registerCommand(ROUTINE_NAME, &monitor_showCalibRoutine);
	stateID = terminal_registerCommand(MONITOR_STATE_ROUTINE, &monitor_state_routine);
	statusID = terminal_registerCommand(MONITOR_STATUS_ROUTINE, &monitor_checkStatus_routine);
}

bool monitor_showCalibRoutine() {
	if (currentCalibrationState == monitor_calibrate_notCalibrating) {
	    while(USBUART_CDCIsReady() == 0u);
	    USBUART_PutString("\n");
		return true;
	}
	if (currentCalibrationState == monitor_calibrate_done) {
	    while(USBUART_CDCIsReady() == 0u);
	    USBUART_PutString("\nCalibration Complete\n");
		terminal_ringBell();
		terminal_ringBell();
	    return false;
	}
	char buf[100];
	buf[0] = '\0';
	switch (currentCalibrationState) {
		case monitor_calibrate_waiting:
			terminal_ringBell();
			sprintf(buf, "\rWaiting...");
			break;
		case monitor_calibrate_printTitle:
			sprintf(buf, "\n  Voltage:      low     high       low     high       low     high       low     high\n");
			break;
		case monitor_calibrate_t1:
			sprintf(
				buf, 
				"\rv: %0.4fv *t1:$%04X$, $%04X$  t2:$%04X$, $%04X$  b1:$%04X$, $%04X$  b2:$%04X$, $%04X$",
				current_volts,
				t1_low,
				t1_high,
				t2_low,
				t2_high,
				b1_low,
				b1_high,
				b2_low,
				b2_high
				);
			break;
		case monitor_calibrate_t2:
			sprintf(
				buf, 
				"\rv: %0.4fv  t1:$%04X$, $%04X$ *t2:$%04X$, $%04X$  b1:$%04X$, $%04X$  b2:$%04X$, $%04X$",
				current_volts,
				t1_low,
				t1_high,
				t2_low,
				t2_high,
				b1_low,
				b1_high,
				b2_low,
				b2_high
				);
			break;
		case monitor_calibrate_b1:
			sprintf(
				buf, 
				"\rv: %0.4fv  t1:$%04X$, $%04X$  t2:$%04X$, $%04X$ *b1:$%04X$, $%04X$  b2:$%04X$, $%04X$",
				current_volts,
				t1_low,
				t1_high,
				t2_low,
				t2_high,
				b1_low,
				b1_high,
				b2_low,
				b2_high
				);
			break;
		case monitor_calibrate_b2:
			sprintf(
				buf, 
				"\rv: %0.4fv  t1:$%04X$, $%04X$  t2:$%04X$, $%04X$  b1:$%04X$, $%04X$ *b2:$%04X$, $%04X$",
				current_volts,
				t1_low,
				t1_high,
				t2_low,
				t2_high,
				b1_low,
				b1_high,
				b2_low,
				b2_high
				);
			break;
		default:
			break;
	}
    while(USBUART_CDCIsReady() == 0u);
    USBUART_PutString(buf);
    return false;
}

void monitor_calibrate_update(monitor_calibrate_item item, double volts, int16_t ADC_low, int16_t ADC_high) {
	currentCalibrationState = item;
	current_volts = volts;
	switch (item) {
		case monitor_calibrate_t1:
			t1_low = ADC_low;
			t1_high = ADC_high;
			break;
		case monitor_calibrate_t2:
			t2_low = ADC_low;
			t2_high = ADC_high;
			break;
		case monitor_calibrate_b1:
			b1_low = ADC_low;
			b1_high = ADC_high;
			break;
		case monitor_calibrate_b2:
			b2_low = ADC_low;
			b2_high = ADC_high;
			break;
		default:
			break;
	}
	terminal_executeCommand(calibrationID);
}