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

#include <stdint.h>
#include <project.h>

int16_t EEPROM_get(uint16_t address, uint8_t offset) {
    int16_t temp = 0;

    temp = EEPROM_ReadByte(address + offset * 16) & 0xFF;
    temp <<= 8;
    temp |= EEPROM_ReadByte(address + offset * 16 + 8) & 0xFF;
    
    return temp;
}

void EEPROM_set(int16_t value, uint16_t address, uint8_t offset) {
	uint8_t upper = (value >> 8) & 0xFF;
	uint8_t lower = value & 0xFF;
	if (EEPROM_WriteByte(upper, address + offset * 16) != CYRET_SUCCESS)
    {
        EEPROM_ERROR_LED_Write(1);
        return;
    }
    if (EEPROM_WriteByte(lower, address + offset * 16 + 8) != CYRET_SUCCESS)
    {
        EEPROM_ERROR_LED_Write(1);
        return;
    }
}

/* [] END OF FILE */
