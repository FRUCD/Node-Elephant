/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#ifndef PEDAL_STATE_H
#define PEDAL_STATE_H

typedef enum {
	pedal_state_neutral,
	pedal_state_driving,
	pedal_state_calibrating,
	pedal_state_out_of_range,
	pedal_state_discrepency,
	pedal_state_implausible
} pedal_state;

extern char* lit_neutral;
extern char* lit_driving;
extern char* lit_calibrating;
extern char* lit_out_of_range;
extern char* lit_discrepency;
extern char* lit_implausible;

#define PEDAL_BRAKE_IMPLAUSIBLE_BRAKE_PERCENT			(0.1)
#define PEDAL_BRAKE_IMPLAUSIBLE_THROTTLE_PERCENT 		(0.25)
#define PEDAL_INCREPENCY_PERCENT						(0.1)
#define PEDAL_BRAKE_IMPLAUSIBLE_EXIT_THROTTLE_PERCENT	(0.05)

#define PEDAL_CAR_START_BRAKE_PERCENT					(0.5)

//TODO: tolerance need to be adjusted
#define PEDAL_MEASURE_TOLERANCE							(0.1)

#endif

/* [] END OF FILE */
