#ifndef PEDAL_MONITOR_STATUS_H
#define PEDAL_MONITOR_STATUS_H

#include <stdbool.h>
#include <stdint.h>
#include "pedal_state.h"

#define MONITOR_STATUS_ROUTINE "status"

bool monitor_checkStatus_routine();

void monitor_status_update_vehicle_state(pedal_state state);
void monitor_setT1Ptrs(const int16_t* min, const int16_t* max, 
	const int32_t* min_mv, const int32_t* max_mv, 
	const int16_t* t1, const int32_t* t1_mv);
void monitor_setT2Ptrs(const int16_t* min, const int16_t* max, 
	const int32_t* min_mv, const int32_t* max_mv, 
	const int16_t* t2, const int32_t* t2_mv);
void monitor_setB1Ptrs(const int16_t* min, const int16_t* max, 
	const int32_t* min_mv, const int32_t* max_mv, 
	const int16_t* b1, const int32_t* b1_mv);
void monitor_setEEPROMStatus(bool isGood);

#endif
