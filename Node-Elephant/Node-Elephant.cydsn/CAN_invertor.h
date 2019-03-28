#ifndef CAN_INTERVOR_H
#define CAN_INTERVOR_H
    
#include <stdint.h>
#include "pedal_state.h"
#include "inverter_state.h"

void CAN_invertor_init(void);
void CAN_invertor_set_throttle_ptr(int16_t* throttle_ptr, int16_t* throttle_min, int16_t* throttle_max);
void CAN_invertor_update_pedal_state(pedal_state state);

void CAN_invertor_pause();
void CAN_invertor_resume();
void sendNMT(NMT_command targetCommand);
void sendFakePDO();

#endif
