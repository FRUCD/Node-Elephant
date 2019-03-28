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

#include <project.h>
#include <stdio.h>
#include <math.h>

#define MIN_COUNT 409			// 409 = 0.4993V
#define MAX_COUNT 3686	        // 3686 = 4.4995V 

typedef enum
{
	pedal_out_of_range_none = 		0,
	pedal_out_of_range_throttle1 =	1 << 0,
	pedal_out_of_range_throttle2 = 	1 << 1,
	pedal_out_of_range_brake1 = 	1 << 2,
	pedal_out_of_range_brake2 = 	1 << 3,
	pedal_out_of_range_steering = 	1 << 4
} pedal_out_of_range_flag;

typedef enum
{
	pedal_brake_plausible_yes = 0,
    pedal_brake_plausible_torque = 1 << 5,
	pedal_brake_plausible_brake = 1 << 6
} pedal_brake_plausibility_flag;

/**
 * @brief Setup pointers for CAN bus to directly grab data from memory
 * @details This function will call CAN_invertor_set_throttle_ptr() in CAN_invertor.h. After the pointer is set, when the CAN bus
 * is trying to send data, it will directly use the value in the pointer.
 */
void pedal_set_CAN();

void pedal_set_monitor();

/**
 * @brief Calibrate data for all sensors.
 * 
 * @details This function call will override old calibration data.
 * This call will also synchronize the new calibration data with EEPROM, if a write error occur, the error LED will light up
 * and the write will abort.
 */
void pedal_calibrate(void);

/**
 * @brief Fetch data from all sensors
 */
void pedal_fetch_data(void);
// double torqueImp(uint16 sensor1, uint16 sensor2, volatile uint8_t* errMsg);     // torque implausibility function prototype
// double brakePlaus(uint16 brake1, uint16 brake2, uint16 throttle1, uint16 throttle2, volatile uint8_t* errMsg); // brake plausibility function prototype
uint8_t pedal_get_out_of_range_flag(void);
uint8_t pedal_is_pedal_reading_matched(double* brake_percentage_diff, double* throttle_percentage_diff);
uint8_t pedal_is_brake_plausible(double* brake_percentage, double* throttle_percentage);

bool pedal_is_brake_pressed();

/**
 * @brief Read calibration data for sensors from EEPROM.
 * 
 * @details This function need to be called when the main program start
 */
void pedal_restore_calibration_data(void);

/* [] END OF FILE */
