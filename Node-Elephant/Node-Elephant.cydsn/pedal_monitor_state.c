#include "pedal_monitor_state.h"
#include <project.h>

static pedal_state currentState = pedal_state_neutral;

bool monitor_state_routine() {
    while(USBUART_CDCIsReady() == 0u);
    USBUART_PutString("\r");
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
	return terminal_detectESC();
}

void monitor_update_vechicle_state(pedal_state state) {
	currentState = state;
}
