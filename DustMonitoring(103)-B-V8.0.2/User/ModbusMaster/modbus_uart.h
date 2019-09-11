#ifndef _MODBUS_UART_H_
#define _MODBUS_UART_H_

#include "bsp.h"

void InitModbusUART(void);

void ModbusUART_WriteBuffer(const uint8_t *pBuffer,uint32_t NumByteToWrite);

#endif
