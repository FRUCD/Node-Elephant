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

// NOTE: OUTPUT SHOULD BE 0 TO 0x7FFF
// NOTE: PADEL OUTPUT is from HIGH to LOW!

#include <project.h>
#include <stdio.h>
#include <stdbool.h>
#include "EEPROM_util.h"



typedef enum pedal_state_t{
    INIT,
    DRIVING,
    CALIBRATING,
    CALIBRATING_HELPER
}pedal_state_t;

int16 throttle1=0;
int16 throttle2=0;
volatile int16 throttle=0;
int16 brake1=0;
int16 brake2=0;
volatile int16 brake=0;
int16 steering=0;
int16 throttle1Max=0;
int16 throttle1Min=0;
int16 throttle2Max=0;
int16 throttle2Min=0;
int16 per_throttle1;
int16 per_throttle2;
int16 brakeMax=0;
int16 brakeMin=0;
volatile uint8_t can_buffer[8];
volatile pedal_state_t pedal_state = INIT;
uint8_t DIFF_FAULT_BIT=0x00;
volatile uint8_t calibration_done_flag=false;
volatile uint8_t force_stop = false;

void update_ADC_SAR();

// data processing and error checking occurs in interrupt service routine
// 
CY_ISR(isr_CAN_Handler){
    if (pedal_state != DRIVING){
        return;
    }
    int16 temp_throttle = 0;
    int16 temp_brake = 0;
    int brake_range = brakeMax-brakeMin;
    int throttle_range = throttle1Max-throttle1Min;
    
    //send PDO1 (throttle)
    temp_throttle = throttle1;//((throttle1Max-throttle1Min)/20);

    // check boundry conditions of throttle input
    if (temp_throttle>throttle1Max){
        temp_throttle=throttle1Max;
    }
    if (temp_throttle<throttle1Min){
        temp_throttle=throttle1Min;
    }
    temp_throttle = (int32)(temp_throttle-throttle1Min)*0x7FFF / (throttle1Max-throttle1Min);//((throttle1Max-throttle1Min)/16));
    
    // subtract dead zone 15%
    temp_brake = brake -((brakeMax-brakeMin)/6);
    if (temp_brake>brakeMax){
        temp_brake=brakeMax;
    }
    if (temp_brake<brakeMin){
        temp_brake=brakeMin;
    }
    temp_brake = (int32)(temp_brake-brakeMin)*100 / (brakeMax-brakeMin);
    
    //check EV2.5.1
    if (temp_throttle < throttle_range * 0.25 && temp_brake==0){
        force_stop = false;
    }
     
    // check for soft BSPD
    if(temp_brake > 0 && temp_throttle > throttle_range * 0.25) {
        force_stop = true;
        temp_throttle = 0;
    }
    
    //check EV2.5
    if (temp_brake>0 && temp_throttle>0x1FFF){
        force_stop = true;
        temp_throttle = 0;
    }
    
    // If brake is below threshold
    if (ADC_SAR_CountsTo_Volts(brake) < .4)
    {
        force_stop = true;   
        temp_throttle = 0;
    }
    
    if (force_stop){
        temp_throttle = 0;
    }
    
    
    //send throttle
    if(temp_throttle>=0x7fff){
        temp_throttle = 0x7fff;
    }
    if(temp_throttle<0){
        temp_throttle = 0;
    }
    
    // when this is received in dash turns into pedalOK
    can_buffer[0]= DIFF_FAULT_BIT;      
    can_buffer[1]= (uint16)(temp_throttle)>>8 & 0xff;
    can_buffer[2]= (uint16)(temp_throttle) & 0xff;
    can_buffer[3]= 0x00;
    can_buffer[4]= (uint8)(per_throttle1) & 0xff;
    can_buffer[5]= (uint8)(per_throttle2) & 0xff;
    can_buffer[6]= 0x00;
    can_buffer[7]= 0x00;
    CAN_SendMsgPDO1();
    
    //send PDO2 (brake)
    can_buffer[0]= (temp_brake>10 ? 0x01:0x00);     // must slam brake to enter drive
    can_buffer[1]= (uint16)(temp_brake)>>8 & 0xff;
    can_buffer[2]= (uint16)(temp_brake) & 0xff;
    can_buffer[3]= 0x00;
    can_buffer[4]= 0x00;
    can_buffer[5]= 0x00;
    can_buffer[6]= (uint16)(brake1)>>8 &0xff;
    can_buffer[7]= (uint16)(brake1) & 0xff;
    CAN_SendMsgPDO2();
    
}
    
CY_ISR(isr_calibration_Handler){
    isr_calibration_ClearPending();
    Button_ClearInterrupt();
    pedal_state = CALIBRATING;
}

CY_ISR(isr_cali_done_Handler){
    calibration_done_flag=true;
}

// rules state that both sensors must be within 10% of each other
bool check_differential(){
    //check throttle
    // calculate percentage of throttle 1
    int16_t temp_throttle1 = throttle1;
    if (temp_throttle1 > throttle1Max){
        temp_throttle1 = throttle1Max;
    }else if(temp_throttle1<throttle1Min){
        temp_throttle1 = throttle1Min;
    }
    per_throttle1 = (temp_throttle1-throttle1Min) *100 / (throttle1Max-throttle1Min);
    
    // calculate percentage of throttle 2
    int16_t temp_throttle2 = throttle2;
    if (temp_throttle2 > throttle2Max){
        temp_throttle2 = throttle2Max;
    }else if(temp_throttle2<throttle2Min){
        temp_throttle2 = throttle2Min;
    }
    per_throttle2 = (temp_throttle2-throttle2Min) *100 / (throttle2Max-throttle2Min);
    
    if (abs(per_throttle1-per_throttle2)>3){
        return false;
    }else{
        return true;
    }
    
    return false;

}

// read previous calibration values from ROM
void restore_EEPROM(){
    // get throttleMax
    throttle1Max = (int16)EEPROM_get(0x0, 0);
    // get throttleMin
    throttle1Min = (int16)EEPROM_get(0x0, 2);
        // get throttleMax
    throttle2Max = (int16)EEPROM_get(0x0, 4);
    // get throttleMin
    throttle2Min = (int16)EEPROM_get(0x0, 6);
    // get brakeMax
    brakeMax = (int16)EEPROM_get(0x0, 8);
    // get brakeMin
    brakeMin = (int16)EEPROM_get(0x0, 10);

}

// write new pedal calibration values to ROM on psoc
void save_EEPROM(){
    // save throttleMax
    EEPROM_set(throttle1Max, 0, 0);
    // save throttleMin
    EEPROM_set(throttle1Min, 0x0, 2);
        // save throttleMax
    EEPROM_set(throttle2Max, 0, 4);
    // save throttleMin
    EEPROM_set(throttle2Min, 0x0, 6);
    // save brakeMax
    EEPROM_set(brakeMax, 0x0, 8);
    // save brakeMin
    EEPROM_set(brakeMin, 0x0, 10);
}

void update_ADC_SAR(){
    int16_t* data_list[5] =
    {
        &throttle1,
        &throttle2,
        &brake1,
        &brake2,
        &steering
    };

    
    uint8_t i = 0;
    for (i = 0; i< 4; i++)
    {
        if (ADC_SAR_IsEndConversion(ADC_SAR_WAIT_FOR_RESULT))
        {
           *(data_list[i]) = ADC_SAR_GetResult16(i); 
        }
    }
    
    
    //brake = (brake1+brake2)/2;  //We only use brake1
    brake = (brake1);
}

int main(){
    ADC_SAR_Start();
    ADC_SAR_StartConvert();
    EEPROM_Start();
    CAN_timer_Start();
    cali_timer_Start();
    cali_timer_Sleep();
    CAN_Init();
    CAN_Start();
    isr_CAN_StartEx(isr_CAN_Handler);
    isr_calibration_StartEx(isr_calibration_Handler);
    isr_cali_done_StartEx(isr_cali_done_Handler);
    
    CyGlobalIntEnable;          //enable global interrupts
    
    for(;;){
        switch(pedal_state){
            case INIT:
                restore_EEPROM();
                calibration_done_flag = false;
                pedal_state = DRIVING;
                CyDelay(1000);
                break;
            case DRIVING:
                update_ADC_SAR();
                if (check_differential()==false){
                    DIFF_FAULT_BIT = 0x01;
                }else{
                    DIFF_FAULT_BIT = 0x00;
                }
                break;
            case CALIBRATING:
                throttle1Max = 0;
                throttle1Min = 0x7FFF;
                throttle2Max = 0;
                throttle2Min = 0x7FFF;
                brakeMax = 0;
                brakeMin = 0x7FFF;
                calibration_done_flag = false;
                cali_timer_Wakeup();
                cali_timer_WriteCounter(0);
                pedal_state = CALIBRATING_HELPER;
            case CALIBRATING_HELPER:  
                update_ADC_SAR();
                // throttle1Max
                if (throttle1>throttle1Max){
                    throttle1Max = throttle1;
                }
                // throttle1Min
                if (throttle1<throttle1Min){
                    throttle1Min = throttle1;
                }
                // throttle2Max
                if (throttle2>throttle2Max){
                    throttle2Max = throttle2;
                }
                // throttle2Min
                if (throttle2<throttle2Min){
                    throttle2Min = throttle2;
                }
                // brakeMax
                if (brake>brakeMax){
                    brakeMax = brake;
                }
                // brakeMin
                if (brake<brakeMin){
                    brakeMin = brake;
                }
                
                if (calibration_done_flag){
                    save_EEPROM();
                    cali_timer_Sleep();
                    pedal_state = DRIVING;
                }
                break;
            default:
                break;
        
        }
    }
}



/* [] END OF FILE */