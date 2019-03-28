#include <project.h>
#include <stdbool.h>
#include <stdio.h>
#include "CAN_invertor.h"

static int16_t* throttle_ptr;
static int16_t* throttle_min;
static int16_t* throttle_max;
static pedal_state state;
volatile uint8_t can_buffer[8];
char LCDBuffer[50];



CY_ISR(isr_CAN_handler)
{
    //if (state == pedal_state_driving)
    //{
        can_buffer[0] = 0;
        can_buffer[1] = 0;

        can_buffer[4] = 0;
        can_buffer[5] = 0;
        can_buffer[6] = 0;
        can_buffer[7] = 0;
        //*********************************************************************************
        //*********************************************************************************
        //*********************************************************************************
        //*********************************************************************************
        //TODO: MAKE SURE this doesn't overflow!!!!!!
    	uint16_t scaled_value = 0;
    	//scaled_value = (*throttle_ptr - *throttle_min) * 0x7FFF / (*throttle_max - *throttle_min) + 0x7FFF;
	    //can_buffer[2] = scaled_value & 0xFF;
	    //can_buffer[3] = (scaled_value >> 8) & 0xFF;
        can_buffer[2]=(*throttle_ptr)>>8 & 0xff;
        can_buffer[3]=(*throttle_ptr) & 0xff;
        CAN_SendMsgPDO1();


        can_buffer[0] = 0x00;
        can_buffer[1] = 0x00;
        can_buffer[2] = 0x00;
        can_buffer[3] = 0x00;
        can_buffer[4] = 0x00;
        can_buffer[5] = 0x00;
        can_buffer[6] = 0x00;
        can_buffer[7] = 0x00;
        CAN_SendMsgPDO2();
    //}

}

void CAN_invertor_init(void)
{
    //CAN_timer_Start();
    isr_CAN_StartEx(isr_CAN_handler);
    LCDBuffer[0] = '\0';
}


void CAN_invertor_pause() {
    // CAN_timer_Stop();
    sendNMT(NMT_command_enterPreOp);
}

void CAN_invertor_resume() {
    sendNMT(NMT_command_startRemoteNode);
    // CAN_timer_Enable();
}

void CAN_invertor_set_throttle_ptr(int16_t* _throttle_ptr, int16_t* _throttle_min, int16_t* _throttle_max)
{
    throttle_ptr = _throttle_ptr;
    throttle_max = _throttle_max;
    throttle_min = _throttle_min;
}

void CAN_invertor_update_pedal_state(pedal_state _state)
{
	state = _state;
}

/**
 * NMT start here
 */

void sendNMT(NMT_command targetCommand) {
    can_buffer[0] = targetCommand;
    can_buffer[1] = INVERTER_ID;

    can_buffer[2] = 0x00;
    can_buffer[3] = 0x00;
    can_buffer[4] = 0x00;
    can_buffer[5] = 0x00;
    can_buffer[6] = 0x00;
    can_buffer[7] = 0x00;
    //Change CAN ID of NMT to 0x000
    CAN_CLEAR_TX_IDE(1);
    CAN_SET_TX_ID_STANDARD_MSG(1, 0x000);
    CAN_SendMsgNMT();
}

void sendFakePDO() {
        can_buffer[0] = 0;
        can_buffer[1] = 0;

        can_buffer[4] = 0;
        can_buffer[5] = 0;
        can_buffer[6] = 0;
        can_buffer[7] = 0;
        //*********************************************************************************
        //*********************************************************************************
        //*********************************************************************************
        //*********************************************************************************
        //TODO: MAKE SURE this doesn't overflow!!!!!!
        can_buffer[2] = 0xFF;
        can_buffer[3] = 0xFF;
        CAN_SendMsgPDO1();


        can_buffer[0] = 0x00;
        can_buffer[1] = 0x00;
        can_buffer[2] = 0x00;
        can_buffer[3] = 0x00;
        can_buffer[4] = 0x00;
        can_buffer[5] = 0x00;
        can_buffer[6] = 0x00;
        can_buffer[7] = 0x00;
        CAN_SendMsgPDO2();
}
