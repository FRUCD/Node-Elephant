#include <stdint.h>
#include <stdbool.h>
#include "pedal_control.h"
#include "EEPROM_util.h"
#include "pedal_state.h"
#include "CAN_invertor.h"
#include "pedal_monitor.h"
#include "pedal_monitor_status.h"

#define USED_EEPROM_SECTOR                      (1u)
#define CALIBRATION_DATA_BASE_ADDRESS          ((USED_EEPROM_SECTOR * CYDEV_EEPROM_SECTOR_SIZE) + 0x00)

#define CALIBRATION_COUNT (10)

/**
 * @brief [brief description]
 * @details We need these two values for ADC because the maximunm and minimum values are 12 bits 2's compliment
 * stuffed into 16 bit int
 */
#define TWELVE_BIT_MAX (0x7FF)
#define TWELVE_BIT_MIN (0x000)


static int16_t MIN_THROTTLE1 = 0;
static int16_t MIN_THROTTLE2 = 0;
static int16_t MAX_THROTTLE1 = 0;
static int16_t MAX_THROTTLE2 = 0;

static int16_t MIN_BRAKE1 = 0;
static int16_t MIN_BRAKE2 = 0;
static int16_t MAX_BRAKE1 = 0;
static int16_t MAX_BRAKE2 = 0;

//static int16_t STEER_LEFT = 0;
//static int16_t STEER_RIGHT = 0;

static int32_t MIN_THROTTLE1_MV = 0;
static int32_t MIN_THROTTLE2_MV = 0;
static int32_t MAX_THROTTLE1_MV = 0;
static int32_t MAX_THROTTLE2_MV = 0;

static int32_t MIN_BRAKE1_MV = 0;
static int32_t MIN_BRAKE2_MV = 0;
static int32_t MAX_BRAKE1_MV = 0;
static int32_t MAX_BRAKE2_MV = 0;

//static int32_t STEER_LEFT_MV = 0;
//static int32_t STEER_RIGHT_MV = 0;

static int16_t throttle1 = 0;
static int16_t throttle2 = 0;
static int16_t brake1 = 0;
static int16_t brake2 = 0;
//static int16_t steering = 0;

static int32_t throttle1_mv = 0;
static int32_t throttle2_mv = 0;
static int32_t brake1_mv = 0;
static int32_t brake2_mv = 0;
//static int32_t steering_mv = 0;

#define PEDAL_MEASURE_TOLERANCE_THROTTLE_MV_1 ((MAX_THROTTLE1_MV - MIN_THROTTLE1_MV) * PEDAL_MEASURE_TOLERANCE)
#define PEDAL_MEASURE_TOLERANCE_BRAKE_MV_1 ((MAX_BRAKE1_MV - MIN_BRAKE1_MV) * PEDAL_MEASURE_TOLERANCE)
#define PEDAL_MEASURE_TOLERANCE_THROTTLE_MV_2 ((MAX_THROTTLE2_MV - MIN_THROTTLE2_MV) * PEDAL_MEASURE_TOLERANCE)
#define PEDAL_MEASURE_TOLERANCE_BRAKE_MV_2 ((MAX_BRAKE2_MV - MIN_BRAKE2_MV) * PEDAL_MEASURE_TOLERANCE)

typedef enum
{
    EEPROM_T1_MIN = 0,
    EEPROM_T2_MIN = 1,
    EEPROM_T1_MAX = 2,
    EEPROM_T2_MAX = 3,
    EEPROM_B1_MIN = 4,
    EEPROM_B2_MIN = 5,
    EEPROM_B1_MAX = 6,
    EEPROM_B2_MAX = 7
} EEPROM_INDEX;

typedef enum
{
    ADC_CHANNEL_T1 = 0,
    ADC_CHANNEL_T2 = 1,
    ADC_CHANNEL_B1 = 2,
    ADC_CHANNEL_B2 = 3
} ADC_CHANNEL;

void pedal_set_CAN()
{
    CAN_invertor_set_throttle_ptr(&throttle1, &MIN_THROTTLE1, &MAX_THROTTLE1);
}


void pedal_set_monitor() {
monitor_setT1Ptrs(
    &MIN_THROTTLE1, &MAX_THROTTLE1,
    &MIN_THROTTLE1_MV, &MAX_THROTTLE1_MV,
    &throttle1, &throttle1_mv);
monitor_setT2Ptrs(
    &MIN_THROTTLE2, &MAX_THROTTLE2,
    &MIN_THROTTLE2_MV, &MAX_THROTTLE2_MV,
    &throttle2, &throttle2_mv);
monitor_setB1Ptrs(
    &MIN_BRAKE1, &MAX_BRAKE1,
    &MIN_BRAKE1_MV, &MAX_BRAKE1_MV,
    &brake1, &brake1_mv);
}

void pedal_calibrate(void)           //calibrate all sensors
{
    uint8 i = 0;        //counter for for loop 
    char buff[50];
    monitor_calibrate_update(monitor_calibrate_printTitle, 0, 0, 0);
    for (i = 0; i < 4; i++)
    {
        LCD_ClearDisplay();
        LCD_Position(0,0);
        LCD_PrintString("Wait...");
        CyDelay(1000);              //Delay because button value resets slower than loop runs
        LCD_ClearDisplay();
        monitor_calibrate_update(monitor_calibrate_waiting, 0, 0, 0);
        switch (i)
        {
            case 0:  //Throttle 1
            {
                int16_t ADC_low = TWELVE_BIT_MAX, ADC_high = TWELVE_BIT_MIN;
                for (;;)
                {
                    double volts = 0;
                    int16_t ADC_value = 0;
                    if (ADC_SAR_IsEndConversion(ADC_SAR_WAIT_FOR_RESULT))
                    {
                        ADC_value = ADC_SAR_GetResult16(ADC_CHANNEL_T1);
                        volts = ADC_SAR_CountsTo_Volts(ADC_value);
                        if (ADC_value < ADC_low)
                            ADC_low = ADC_value;
                        if (ADC_value > ADC_high)
                            ADC_high = ADC_value;
                        LCD_Position(0,0);
                        sprintf(buff, "T1: %0.4fv", volts);
                        LCD_PrintString(buff); 
                        LCD_Position(1u, 0u);
                        sprintf(buff, "$%04X$, $%04X$", ADC_low, ADC_high);
                        LCD_PrintString(buff); 
                        monitor_calibrate_update(monitor_calibrate_t1, volts, ADC_low, ADC_high);
                    
                        if (Button_Read() == 0)
                        {
                            MAX_THROTTLE1 = ADC_high;
                            MIN_THROTTLE1 = ADC_low;
                            MAX_THROTTLE1_MV = ADC_SAR_CountsTo_mVolts(MAX_THROTTLE1);
                            MIN_THROTTLE1_MV = ADC_SAR_CountsTo_mVolts(MIN_THROTTLE1);
                            EEPROM_set(MAX_THROTTLE1, CALIBRATION_DATA_BASE_ADDRESS, EEPROM_T1_MAX);
                            EEPROM_set(MIN_THROTTLE1, CALIBRATION_DATA_BASE_ADDRESS, EEPROM_T1_MIN);
                            break;
                        }
                    }

                }
                break;
            }
            case 1: //Throttle 2
            {
                int16_t ADC_low = TWELVE_BIT_MAX, ADC_high = TWELVE_BIT_MIN;
                for (;;)
                {
                    double volts = 0;
                    int16_t ADC_value = 0;
                    if (ADC_SAR_IsEndConversion(ADC_SAR_WAIT_FOR_RESULT))
                    {
                        ADC_value = ADC_SAR_GetResult16(ADC_CHANNEL_T2);
                        volts = ADC_SAR_CountsTo_Volts(ADC_value);
                        if (ADC_value < ADC_low)
                            ADC_low = ADC_value;
                        if (ADC_value > ADC_high)
                            ADC_high = ADC_value;
                        LCD_Position(0,0);
                        sprintf(buff, "T2: %0.4fv", volts);
                        LCD_PrintString(buff); 
                        LCD_Position(1u, 0u);
                        sprintf(buff, "$%04X$, $%04X$", ADC_low, ADC_high);
                        LCD_PrintString(buff); 
                        monitor_calibrate_update(monitor_calibrate_t2, volts, ADC_low, ADC_high);
                    
                        if (Button_Read() == 0)
                        {
                            MAX_THROTTLE2 = ADC_high;
                            MIN_THROTTLE2 = ADC_low;
                            MAX_THROTTLE2_MV = ADC_SAR_CountsTo_mVolts(MAX_THROTTLE2);
                            MIN_THROTTLE2_MV = ADC_SAR_CountsTo_mVolts(MIN_THROTTLE2);
                            EEPROM_set(MAX_THROTTLE2, CALIBRATION_DATA_BASE_ADDRESS, EEPROM_T2_MAX);
                            EEPROM_set(MIN_THROTTLE2, CALIBRATION_DATA_BASE_ADDRESS, EEPROM_T2_MIN);
                            break;
                        }
                    }

                }
                break;
            }
            case 2: //Brake 1
            {
                int16_t ADC_low = TWELVE_BIT_MAX, ADC_high = TWELVE_BIT_MIN;
                for (;;)
                {
                    double volts = 0;
                    int16_t ADC_value = 0;
                    if (ADC_SAR_IsEndConversion(ADC_SAR_WAIT_FOR_RESULT))
                    {
                        ADC_value = ADC_SAR_GetResult16(ADC_CHANNEL_B1);
                        volts = ADC_SAR_CountsTo_Volts(ADC_value);
                        if (ADC_value < ADC_low)
                            ADC_low = ADC_value;
                        if (ADC_value > ADC_high)
                            ADC_high = ADC_value;
                        LCD_Position(0,0);
                        sprintf(buff, "B1: %0.4fv", volts);
                        LCD_PrintString(buff); 
                        LCD_Position(1u, 0u);
                        sprintf(buff, "$%04X$, $%04X$", ADC_low, ADC_high);
                        LCD_PrintString(buff); 
                        monitor_calibrate_update(monitor_calibrate_b1, volts, ADC_low, ADC_high);
                    
                        if (Button_Read() == 0)
                        {
                            MAX_BRAKE1 = ADC_high;
                            MIN_BRAKE1 = ADC_low;
                            MAX_BRAKE1_MV = ADC_SAR_CountsTo_mVolts(MAX_BRAKE1);
                            MIN_BRAKE1_MV = ADC_SAR_CountsTo_mVolts(MIN_BRAKE1);
                            EEPROM_set(MAX_BRAKE1, CALIBRATION_DATA_BASE_ADDRESS, EEPROM_B1_MAX);
                            EEPROM_set(MIN_BRAKE1, CALIBRATION_DATA_BASE_ADDRESS, EEPROM_B1_MIN);
                            break;
                        }
                    }

                }
                break;
            }
            case 3: //Brake 2
            {
                int16_t ADC_low = TWELVE_BIT_MAX, ADC_high = TWELVE_BIT_MIN;
                for (;;)
                {
                    double volts = 0;
                    int16_t ADC_value = 0;
                    if (ADC_SAR_IsEndConversion(ADC_SAR_WAIT_FOR_RESULT))
                    {
                        ADC_value = ADC_SAR_GetResult16(ADC_CHANNEL_B1);
                        volts = ADC_SAR_CountsTo_Volts(ADC_value);
                        if (ADC_value < ADC_low)
                            ADC_low = ADC_value;
                        if (ADC_value > ADC_high)
                            ADC_high = ADC_value;
                        LCD_Position(0,0);
                        sprintf(buff, "B2: %0.4fv", volts);
                        LCD_PrintString(buff); 
                        LCD_Position(1u, 0u);
                        sprintf(buff, "$%04X$, $%04X$", ADC_low, ADC_high);
                        LCD_PrintString(buff); 
                        monitor_calibrate_update(monitor_calibrate_b2, volts, ADC_low, ADC_high);
                    
                        if (Button_Read() == 0)
                        {
                            MAX_BRAKE2 = ADC_high;
                            MIN_BRAKE2 = ADC_low;
                            MAX_BRAKE2_MV = ADC_SAR_CountsTo_mVolts(MAX_BRAKE2);
                            MIN_BRAKE2_MV = ADC_SAR_CountsTo_mVolts(MIN_BRAKE2);
                            EEPROM_set(MAX_BRAKE2, CALIBRATION_DATA_BASE_ADDRESS, EEPROM_B2_MAX);
                            EEPROM_set(MIN_BRAKE2, CALIBRATION_DATA_BASE_ADDRESS, EEPROM_B2_MIN);
                            break;
                        }
                    }

                }
                break;
            }
            default: LCD_PrintString("Error in loop");
                break;
        }

    }
    
    LCD_ClearDisplay();
    LCD_Position(0,0);
    LCD_PrintString("Calibration");
    LCD_Position(1,0);
    LCD_PrintString("Complete");
    monitor_calibrate_update(monitor_calibrate_done, 0, 0, 0);
    CyDelay(2000);
    monitor_calibrate_update(monitor_calibrate_notCalibrating, 0, 0, 0);
    Button_ClearInterrupt();
    return;
}


void pedal_fetch_data(void)
{
    int16_t* data_list[5] =
    {
        &throttle1,
        &throttle2,
        &brake1,
        &brake2//,
        // &steering
    };

    int32_t* converted_data_list[5] =
    {
        &throttle1_mv,
        &throttle2_mv,
        &brake1_mv,
        &brake2_mv//,
        // &steering_mv
    };
    
    uint8_t i = 0;
    for (i = 0; i< 4; i++)
    {
        if (ADC_SAR_IsEndConversion(ADC_SAR_WAIT_FOR_RESULT))
        {
           *(data_list[i]) = ADC_SAR_GetResult16(i); 
           *(converted_data_list[i]) = ADC_SAR_CountsTo_mVolts(*(data_list[i]));
        }
    }
}

uint8_t pedal_is_pedal_reading_matched(double* brake_percentage_diff, double* throttle_percentage_diff)
{

    float brake1_percent = (float)(brake1_mv - MIN_BRAKE1_MV) / (float)(MAX_BRAKE1_MV - MIN_BRAKE1_MV);
    float brake2_percent = (float)(brake2_mv - MIN_BRAKE2_MV) / (float)(MAX_BRAKE2_MV - MIN_BRAKE2_MV);

    double percentDiff = fabs(brake1_percent - brake2_percent);
    bool fault_occurred = false;
    *brake_percentage_diff = percentDiff;

    if (percentDiff > PEDAL_INCREPENCY_PERCENT)
    {
        fault_occurred = true;
    }

    float throttle1_percent = (float)(throttle1_mv - MIN_THROTTLE1_MV) / (float)(MAX_THROTTLE1_MV - MIN_THROTTLE1_MV);
    float throttle2_percent = (float)(throttle2_mv - MIN_THROTTLE2_MV) / (float)(MAX_THROTTLE2_MV - MIN_THROTTLE2_MV);
    percentDiff = fabs(throttle1_percent - throttle2_percent);
    *throttle_percentage_diff = percentDiff;
    
    if (percentDiff > PEDAL_INCREPENCY_PERCENT)
    {
        fault_occurred = true;
    }

    if (fault_occurred)
    {
        return pedal_brake_plausible_torque;
    }
    return pedal_brake_plausible_yes;
}

uint8_t pedal_is_brake_plausible(double* brake_percentage_ptr, double* throttle_percentage_ptr)
{
    if (brake1_mv >= MIN_BRAKE1_MV && brake1_mv <= MAX_BRAKE1_MV)          
    {
        double brake_percentage = (double)(brake1_mv - MIN_BRAKE1_MV) / (MAX_BRAKE1_MV - MIN_BRAKE1_MV);
        double throttle_percentage = (double)(throttle1_mv - MIN_THROTTLE1_MV) / (MAX_THROTTLE1_MV - MIN_THROTTLE1_MV);
        *brake_percentage_ptr = brake_percentage;
        *throttle_percentage_ptr = throttle_percentage;
        if (
            brake_percentage > (PEDAL_BRAKE_IMPLAUSIBLE_BRAKE_PERCENT - PEDAL_MEASURE_TOLERANCE) && 
            throttle_percentage > (PEDAL_BRAKE_IMPLAUSIBLE_THROTTLE_PERCENT + PEDAL_MEASURE_TOLERANCE)
            )
        {
            return pedal_brake_plausible_brake;
        }
        return pedal_brake_plausible_yes;
    }
    else
    {
        return pedal_brake_plausible_brake;
    }
    return pedal_brake_plausible_yes;
}


bool pedal_is_brake_pressed()
{
    double brake_percentage = (double)(brake1_mv - MIN_BRAKE1_MV) / (MAX_BRAKE1_MV - MIN_BRAKE1_MV);
    if (brake_percentage > PEDAL_CAR_START_BRAKE_PERCENT - PEDAL_MEASURE_TOLERANCE)
    {
        return true;
    }
    return false;
}

uint8_t pedal_get_out_of_range_flag(void)
{
    uint8_t error_flag = pedal_out_of_range_none;

    if (
        throttle1_mv < (MIN_THROTTLE1_MV - PEDAL_MEASURE_TOLERANCE_THROTTLE_MV_1)
         || throttle1_mv > (MAX_THROTTLE1_MV + PEDAL_MEASURE_TOLERANCE_THROTTLE_MV_1)
         )
        error_flag |= pedal_out_of_range_throttle1;
    
    if (
        throttle2_mv < (MIN_THROTTLE2_MV - PEDAL_MEASURE_TOLERANCE_THROTTLE_MV_2) 
        || throttle2_mv > (MAX_THROTTLE2_MV + PEDAL_MEASURE_TOLERANCE_THROTTLE_MV_2)
        )
        error_flag |= pedal_out_of_range_throttle2;
    
    if (
        brake1_mv < (MIN_BRAKE1_MV - PEDAL_MEASURE_TOLERANCE_BRAKE_MV_1) 
        || brake1_mv > (MAX_BRAKE1_MV + PEDAL_MEASURE_TOLERANCE_BRAKE_MV_1)
        )
        error_flag |= pedal_out_of_range_brake1;
    
    if (
        brake2_mv < (MIN_BRAKE2_MV - PEDAL_MEASURE_TOLERANCE_BRAKE_MV_2) 
        || brake2_mv > (MAX_BRAKE2_MV + PEDAL_MEASURE_TOLERANCE_BRAKE_MV_2)
        )
        error_flag |= pedal_out_of_range_brake2;

    return error_flag;
}

void pedal_restore_calibration_data(void)
{   
    MIN_THROTTLE1 = EEPROM_get(CALIBRATION_DATA_BASE_ADDRESS, 0);
    MIN_THROTTLE2 = EEPROM_get(CALIBRATION_DATA_BASE_ADDRESS, 1);
    MAX_THROTTLE1 = EEPROM_get(CALIBRATION_DATA_BASE_ADDRESS, 2);
    MAX_THROTTLE2 = EEPROM_get(CALIBRATION_DATA_BASE_ADDRESS, 3);
    MIN_BRAKE1 = EEPROM_get(CALIBRATION_DATA_BASE_ADDRESS, 4);
    MIN_BRAKE2 = EEPROM_get(CALIBRATION_DATA_BASE_ADDRESS, 5);
    MAX_BRAKE1 = EEPROM_get(CALIBRATION_DATA_BASE_ADDRESS, 6);
    MAX_BRAKE2 = EEPROM_get(CALIBRATION_DATA_BASE_ADDRESS, 7);
    // STEER_LEFT = EEPROM_get(CALIBRATION_DATA_BASE_ADDRESS, 8);
    // STEER_RIGHT = EEPROM_get(CALIBRATION_DATA_BASE_ADDRESS, 9);

    MIN_THROTTLE1_MV = ADC_SAR_CountsTo_mVolts(MIN_THROTTLE1);
    MIN_THROTTLE2_MV = ADC_SAR_CountsTo_mVolts(MIN_THROTTLE2);
    MAX_THROTTLE1_MV = ADC_SAR_CountsTo_mVolts(MAX_THROTTLE1);
    MAX_THROTTLE2_MV = ADC_SAR_CountsTo_mVolts(MAX_THROTTLE2);
    MIN_BRAKE1_MV = ADC_SAR_CountsTo_mVolts(MIN_BRAKE1);
    MIN_BRAKE2_MV = ADC_SAR_CountsTo_mVolts(MIN_BRAKE2);
    MAX_BRAKE1_MV = ADC_SAR_CountsTo_mVolts(MAX_BRAKE1);
    MAX_BRAKE2_MV = ADC_SAR_CountsTo_mVolts(MAX_BRAKE2);
    // STEER_LEFT_MV = ADC_SAR_CountsTo_mVolts(STEER_LEFT);
    // STEER_RIGHT_MV = ADC_SAR_CountsTo_mVolts(STEER_RIGHT);
}

/* [] END OF FILE */
