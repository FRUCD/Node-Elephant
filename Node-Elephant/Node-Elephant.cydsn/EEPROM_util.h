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

#ifndef EEPROM_UTIL_H
#define EEPROM_UTIL_H

#include <project.h>

int16_t EEPROM_get(uint16_t address, uint8_t offset);
void EEPROM_set(int16_t value, uint16_t address, uint8_t offset);

#endif

/* [] END OF FILE */
