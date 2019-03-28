#include "pedal_monitor_status.h"
#include <project.h>
#include <stdio.h>
#include "uart-terminal.h"

 /**
  * @brief ASCII control code
  */
#define BEL (7)
#define BS (8)
#define DEL (127)
#define CR (13)
#define ESC (27)
#define FF (12)

static pedal_state currentState = pedal_state_neutral;
static char statusBuf[100];
static bool EEPROM_isGood = true;

const int16_t* MIN_THROTTLE1 = NULL;
const int16_t* MIN_THROTTLE2 = NULL;
const int16_t* MAX_THROTTLE1 = NULL;
const int16_t* MAX_THROTTLE2 = NULL;

const int16_t* MIN_BRAKE1 = NULL;
const int16_t* MAX_BRAKE1 = NULL;

const int32_t* MIN_THROTTLE1_MV = NULL;
const int32_t* MIN_THROTTLE2_MV = NULL;
const int32_t* MAX_THROTTLE1_MV = NULL;
const int32_t* MAX_THROTTLE2_MV = NULL;

const int32_t* MIN_BRAKE1_MV = NULL;
const int32_t* MAX_BRAKE1_MV = NULL;

const int16_t* throttle1 = NULL;
const int16_t* throttle2 = NULL;
const int16_t* brake1 = NULL;

const int32_t* throttle1_mv = NULL;
const int32_t* throttle2_mv = NULL;
const int32_t* brake1_mv = NULL;

void monitor_status_printState();
void monitor_status_printThrottle1();
void monitor_status_printThrottle2();
void monitor_status_printThrottleOffValue();
void monitor_status_printBrake1();
void monitor_status_printCANStatus();
void monitor_status_printEEPROMStatus();

bool monitor_checkStatus_routine() {
    while (USBUART_CDCIsReady() == 0u);
    USBUART_PutString("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	monitor_status_printState();
	monitor_status_printThrottle1();
	monitor_status_printThrottle2();
	monitor_status_printThrottleOffValue();
	monitor_status_printBrake1();
	monitor_status_printCANStatus();
	monitor_status_printEEPROMStatus();

	return terminal_detectESC();
}

void monitor_status_update_vehicle_state(pedal_state state) {
	currentState = state;
}

void monitor_setT1Ptrs(const int16_t* min, const int16_t* max, 
	const int32_t* min_mv, const int32_t* max_mv, 
	const int16_t* t1, const int32_t* t1_mv) {
	MIN_THROTTLE1 = min;
	MAX_THROTTLE1 = max;
	MIN_THROTTLE1_MV = min_mv;
	MAX_THROTTLE1_MV = max_mv;
	throttle1 = t1;
	throttle1_mv = t1_mv;
}

void monitor_setT2Ptrs(const int16_t* min, const int16_t* max, 
	const int32_t* min_mv, const int32_t* max_mv, 
	const int16_t* t2, const int32_t* t2_mv) {
	MIN_THROTTLE2 = min;
	MAX_THROTTLE2 = max;
	MIN_THROTTLE2_MV = min_mv;
	MAX_THROTTLE2_MV = max_mv;
	throttle2 = t2;
	throttle2_mv = t2_mv;
}

void monitor_setB1Ptrs(const int16_t* min, const int16_t* max, 
	const int32_t* min_mv, const int32_t* max_mv, 
	const int16_t* b1, const int32_t* b1_mv) {
	MIN_BRAKE1 = min;
	MAX_BRAKE1 = max;
	MIN_BRAKE1_MV = min_mv;
	MAX_BRAKE1_MV = max_mv;
	brake1 = b1;
	brake1_mv = b1_mv;
}

void monitor_setEEPROMStatus(bool isGood) {
	EEPROM_isGood = isGood;
}


/**
 * Private functions start here
 */

void monitor_status_printState() {
    while(USBUART_CDCIsReady() == 0u);
    USBUART_PutString("Vehicle State: ");

	char* ptr = "\0";
	switch (currentState) {
		case pedal_state_neutral:
			ptr = (char*)lit_neutral;
			break;
		case pedal_state_driving:
			ptr = (char*)lit_driving;
			break;
		case pedal_state_calibrating:
			ptr = (char*)lit_calibrating;
			break;
		case pedal_state_out_of_range:
			ptr = (char*)lit_out_of_range;
			break;
		case pedal_state_discrepency:
			ptr = (char*)lit_discrepency;
			break;
		case pedal_state_implausible:
			ptr = (char*)lit_implausible;
			break;
		default:
			break;
	}
    while(USBUART_CDCIsReady() == 0u);
    USBUART_PutString(ptr);
    while(USBUART_CDCIsReady() == 0u);
    USBUART_PutChar('\n');
}

void monitor_status_printThrottle1() {
    while (USBUART_CDCIsReady() == 0u);
    USBUART_PutString("Trottle 1: ");
    //TODO: Throttle percent is inverted, this is subject to change
    sprintf(statusBuf, "min: $%04X$, %dmv, max: $%04X$, %dmv, current: $%04X$, %dmv, at %0.2f%%\n",
    	(int)*MIN_THROTTLE1, (int)*MIN_THROTTLE1_MV, (int)*MAX_THROTTLE1, (int)*MAX_THROTTLE1_MV, (int)*throttle1, (int)*throttle1_mv, 
    	((float)(*throttle1_mv - *MIN_THROTTLE1_MV) / (*MAX_THROTTLE1_MV - *MIN_THROTTLE1_MV)) * 100);
    while (USBUART_CDCIsReady() == 0u);
    USBUART_PutString(statusBuf);
}

void monitor_status_printThrottle2() {
    while (USBUART_CDCIsReady() == 0u);
    USBUART_PutString("Trottle 2: ");
    //TODO: Throttle percent is inverted, this is subject to change
    sprintf(statusBuf, "min: $%04X$, %dmv, max: $%04X$, %dmv, current: $%04X$, %dmv, at %0.2f%%\n",
    	(int)*MIN_THROTTLE2, (int)*MIN_THROTTLE2_MV, (int)*MAX_THROTTLE2, (int)*MAX_THROTTLE2_MV, (int)*throttle2, (int)*throttle2_mv, 
    	((float)(*throttle2_mv - *MIN_THROTTLE2_MV) / (*MAX_THROTTLE2_MV - *MIN_THROTTLE2_MV)) * 100);
    while (USBUART_CDCIsReady() == 0u);
    USBUART_PutString(statusBuf);
}

void monitor_status_printThrottleOffValue() {
	float t1 = (float)(*throttle1_mv - *MIN_THROTTLE1_MV) / (*MAX_THROTTLE1_MV - *MIN_THROTTLE1_MV);
	float t2 = (float)(*throttle2_mv - *MIN_THROTTLE2_MV) / (*MAX_THROTTLE2_MV - *MIN_THROTTLE2_MV);
	sprintf(statusBuf, "T1 and T2 is now off by %0.2f%%\n", (t1 - t2) * 100);
    while (USBUART_CDCIsReady() == 0u);
    USBUART_PutString(statusBuf);
}

void monitor_status_printBrake1() {
    while (USBUART_CDCIsReady() == 0u);
    USBUART_PutString("Brake 1: ");
    sprintf(statusBuf, "min: $%04X$, %dmv, max: $%04X$, %dmv, current: $%04X$, %dmv, at %0.2f%%\n",
    	(int)*MIN_BRAKE1, (int)*MIN_BRAKE1_MV, (int)*MAX_BRAKE1, (int)*MAX_BRAKE1_MV, (int)*brake1, (int)*brake1_mv, 
    	((float)(*brake1_mv - *MIN_BRAKE1_MV) / (*MAX_BRAKE1_MV - *MIN_BRAKE1_MV)) * 100);
    while (USBUART_CDCIsReady() == 0u);
    USBUART_PutString(statusBuf);
}

void monitor_status_printCANStatus() {

}

void monitor_status_printEEPROMStatus() {
	if (EEPROM_isGood) {
	    while (USBUART_CDCIsReady() == 0u);
	    USBUART_PutString("EEPROM: Operational\n");
	} else {
	    while (USBUART_CDCIsReady() == 0u);
	    USBUART_PutString("EEPROM: Error\n");
	}
}
 