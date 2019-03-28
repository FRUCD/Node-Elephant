#ifndef PEDAL_MONIOR_STATE_H
#define PEDAL_MONIOR_STATE_H

#include "pedal_monitor.h"
#include <stdbool.h>
#include "pedal_state.h"

#define MONITOR_STATE_ROUTINE "vechicle state"

bool monitor_state_routine();
void monitor_update_vechicle_state(pedal_state state);

#endif
