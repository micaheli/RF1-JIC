#include "includes.h"


//extern uartPort_t *spektrumUart;
//#define SRXLTELEM_PACKET_LEN 21
STR_SRXL_TELEM telemetry;
STR_SRXL_BIND bind;

extern STRU_TELE_LAPTIMER lap_timer;

DMA_InitTypeDef DMA_InitStructure;

uint8_t dma_count;
TELEMETRY_STATE telemetryState = TELEM_START;
UN_TELEMETRY sensorData;
extern uint32_t progMode;

#define UINT16_ENDIAN(a)  (((a) >> 8) | ((a) << 8) )

void InitSpektrumTelemetry(void) {
	return;
}

void sendSpektrumTelem(void)
{
	if (telemetryState >= NUM_TELEM_STATES)
	{
		telemetryState = TELEM_START;
	}
	
 //Telem structure  0xA5, Identifier (0x80 for telem packet), Length (21 for telem), 16 byte payload, 2 byte CRC   21 Byte total
	telemetry.packet.SRXL_ID = SPEKTRUM_SRXL_ID;
	telemetry.packet.identifier = SRXL_TELEM_ID;
	telemetry.packet.length = SRXL_TELEM_LENGTH;

	switch (telemetryState)
	{
	case NUM_TELEM_STATES: //stop compile error
		break;
	case TELEM_FLIGHTLOG:
		{
			//Rx will be handling this data internally until multiple receivers are supported
			//memset(&telemetry.packet.data, 0, 16);
			
			telemetry.packet.data.flightLog.id = FLIGHTLOG_ID;
			telemetry.packet.data.flightLog.A = 0xFFFF;
			telemetry.packet.data.flightLog.B = 0xFFFF;
			telemetry.packet.data.flightLog.L = 0xFFFF;
			telemetry.packet.data.flightLog.R = 0xFFFF;
			telemetry.packet.data.flightLog.F = 0xFFFF;
			telemetry.packet.data.flightLog.H = 0xFFFF;
			telemetry.packet.data.flightLog.rfu = 0;
      
			telemetry.packet.data.flightLog.rxVoltage = 0xFFFF;
			
			telemetryState++;
			break;
		}
    
	case TELEM_INTERNAL:
		{
			telemetry.packet.data.internalSensors.id = INTERNAL_ID;      
			//telemetry.packet.data.internalSensors.packVoltage = UINT16_ENDIAN(vbat * 10);

			//Currently we have no values to provide here. Send 0xFFFF so receiver will use its own values
			//RPM data is multipurposed with Lap Time data, so changing the value here will mean the receiver wont use its own measured RPM/Lap Time
			telemetry.packet.data.internalSensors.rpm = 0xFFFF;
			telemetry.packet.data.internalSensors.temperature = 0x7FFF;
			telemetry.packet.data.internalSensors.rfu = 0;
			memset(telemetry.packet.data.internalSensors.spare, 0, 8);

			telemetryState++;
			break;
		}

	case TELEM_XBUS:
		{
			xbus.sensorCount = 3; //The more of these there are, the longer it will take to send each TELEM_XBUS packet over telemetry
			if (xbus.sensorCount > 0)
			{
				switch (xbus.sensorPosition)
				{
				case 0:
					{
					/*
						sensorData.fpMAH.identifier = TELE_DEVICE_FP_MAH;
						sensorData.fpMAH.sID = 0;
						sensorData.fpMAH.current_A = amperage / 10;
						sensorData.fpMAH.chargeUsed_A = mAhDrawn;
						sensorData.fpMAH.temp_A = 0x7FFF;
						sensorData.fpMAH.current_B = 0x7FFF;
						sensorData.fpMAH.chargeUsed_B = 0x7FFF;
						sensorData.fpMAH.temp_B = 0x7FFF;
						*/
						break;
					}
				case 1:
					{
					
						//if (feature(FEATURE_SPM_LAPTIMER))
						//{
						//	memcpy(&sensorData, &lap_timer, 16);
						//}
						//else
						//{
							sensorData.user_16SU.identifier = TELE_DEVICE_USER_16SU;
							sensorData.user_16SU.sID = 0x00;
							sensorData.user_16SU.sField1 = 1;
							sensorData.user_16SU.sField2 = 2;
							sensorData.user_16SU.sField3 = 3;
							sensorData.user_16SU.uField1 = 4;
							sensorData.user_16SU.uField2 = 5;
							sensorData.user_16SU.uField3 = 6;
							sensorData.user_16SU.uField4 = 7;
						//}
						
						break;
					}
				case 2:
					{
						textMenuUpdate();
						//sensorData.user_text.text = tempString;
						/*
						sensorData.user_16SU32U.identifier = TELE_DEVICE_USER_16SU32U;
						sensorData.user_16SU32U.sID = 0x00;
						sensorData.user_16SU32U.sField1 = 8;
						sensorData.user_16SU32U.sField2 = 9;
						sensorData.user_16SU32U.uField1 = 10;
						sensorData.user_16SU32U.uField2 = 11;
						sensorData.user_16SU32U.uField3 = 12;
						sensorData.user_16SU32U.u32Field = 13;
						*/
						break;
					}
				case 3:
					{
					/*
						sensorData.user_16SU32S.identifier = TELE_DEVICE_USER_16SU32S;
						sensorData.user_16SU32S.sID = 0x00;
						sensorData.user_16SU32S.sField1 = 14;
						sensorData.user_16SU32S.sField2 = 15;
						sensorData.user_16SU32S.uField1 = 16;
						sensorData.user_16SU32S.uField2 = 17;
						sensorData.user_16SU32S.uField3 = 18;
						sensorData.user_16SU32S.s32Field = 19;
						*/
						break;
					}
				case 4:
					{
						sensorData.user_16U32SU.identifier = TELE_DEVICE_USER_16U32SU;
						sensorData.user_16U32SU.sID = 0x00;
						sensorData.user_16U32SU.uField1 = 20;
						sensorData.user_16U32SU.s32Field = 21;
						sensorData.user_16U32SU.u32Field1 = 22;
						sensorData.user_16U32SU.u32Field2 = 23;
						break;
					}
				}

				memcpy(&telemetry.packet.data, &sensorData, 16);

				// Advanced and wrap sensor index
				if (progMode)
					xbus.sensorPosition = 2;
				else
					xbus.sensorPosition++;

				xbus.sensorPosition = xbus.sensorPosition % xbus.sensorCount;
			}

			
			telemetryState++;
			break;
		}
	}

	uint16_t crc = 0x0000;

	for (uint8_t i = 0; i < 16; i++)
		crc = srxlCrc16(crc, telemetry.packet.data.raw[i], SRXL_POLY);
	
	telemetry.packet.crc = crc;

	sendSpektrumSRXL((uint32_t)&telemetry, SRXL_TELEM_LENGTH);
	
}

void sendSpektrumBind()
{
	bind.srxlID = SPEKTRUM_SRXL_ID;
	bind.subID = SRXL_BIND_ID;
	bind.length = SRXL_BIND_LENGTH;
	bind.data.request = SRXL_BIND_ENTER;
	bind.data.guid = 0;
	bind.data.type = 0;
	bind.data.rfID = 0;

	uint16_t crc = 0x0000;

	for (uint8_t i = 0; i < SRXLBIND_PAYLOAD_LEN; i++)
		crc = srxlCrc16(crc, bind.data.raw[i], SRXL_POLY);
	
	bind.crc = crc;

	sendSpektrumSRXL((uint32_t)&bind, SRXL_BIND_LENGTH);
}

void sendSpektrumSRXL(uint32_t baseAddress, uint8_t packetSize)
{
	(void)(packetSize);
	(void)(baseAddress);
	for (uint32_t serialNumber = 0;serialNumber<MAX_USARTS;serialNumber++)
	{
		if ( (board.serials[serialNumber].enabled) && (mainConfig.telemConfig.telemSpek) )
		{
			//callbackFunctionArray[FP_DMA1_S3] = SerialTxCallback;
			//HAL_UART_Transmit_DMA(&uartHandles[board.serials[serialNumber].usartHandle], (uint8_t *)baseAddress, packetSize);
			//HAL_UART_Transmit(&uartHandles[board.serials[serialNumber].usartHandle], (uint8_t *)baseAddress, packetSize, 10);
		}
	}
}

uint16_t srxlCrc16(uint16_t crc, uint8_t data, uint16_t poly)
{
	crc = crc ^ data << 8;
	for (int i = 0; i < 8; i++)
	{
		if (crc & 0x8000)
			crc = crc << 1 ^ poly;
		else
			crc = crc << 1;
	}
	return crc;
}


int32_t row;
int32_t column;
int32_t columnAxis;
int32_t vStickStatus;
int32_t hStickStatus;
float dataInc;
#define ROW_MAX 8
#define COLUMN_MAX 1

uint32_t toggleTime;
uint32_t blinkTime;
uint32_t currentTime;

char stringArray[9][12];
char axisTable[3][12] = { "Yaw", "Roll", "Pitch" };

char row1[12];
char row2[12];
char row3[12];
char row4[12];
char row5[12];
char row6[12];
char row7[12];
char row8[12];
char row9[12];

void textMenuUpdate(void)
{
	{
		currentTime = InlineMillis();

		if (progMode != 0)
		{
			//vertical stick
			if (rxData[2] > 1224 && vStickStatus != 1)
			{
				if (column == 0)
					row--;
				else
					dataInc = 1;
				//toggleTime = currentTime;

				vStickStatus = 1;
			}
			else if (rxData[2] < 824 && vStickStatus != -1)
			{
				if (column == 0)
					row++;
				else 
					dataInc = -1;
				//toggleTime = currentTime;
				vStickStatus = -1;
			}
			else if (rxData[2] > 924 && rxData[2] < 1124)
			{
				vStickStatus = 0;
			}

			//horizontal stick
			if (rxData[1] > 1224 && hStickStatus != 1)
			{
				column++;
				//toggleTime = currentTime;

				hStickStatus = 1;
			}
			else if (rxData[1] < 824 && hStickStatus != -1)
			{
				column--;
				//toggleTime = currentTime;

				hStickStatus = -1;
			}	
			else if (rxData[1] > 924 && rxData[1] < 1124)
			{
				hStickStatus = 0;
			}
		}
		else
		{
			//have these initialized so nothing happens when entering prog mode for the first time
			vStickStatus = -1;
			hStickStatus = -1;
		}
						

		if (row >= ROW_MAX)
			row = ROW_MAX;
		else if (row < 0)
			row = 0;

		if (column >= COLUMN_MAX)
			column = COLUMN_MAX;
		else if (column < 0 )
		{
			row = 0;
			column = 0;
			columnAxis = 0;
			progMode = 0;
			InitPid(); //Set PID's with new config PID's
		}
			

		strcpy(stringArray[0], "PID TUNING");
		strcpy(stringArray[1], "Calibrate 1");
		strcpy(stringArray[2], "Calibrate 2");
		strcpy(stringArray[3], axisTable[columnAxis]);
		strcpy(stringArray[4], "P: ");
		strcpy(stringArray[5], "I: ");
		strcpy(stringArray[6], "D: ");
		strcpy(stringArray[7], "C: ");
		strcpy(stringArray[8], "SAVE");

		if (row == 1 && dataInc)
			SetCalibrate1();
		if (row == 2 && dataInc)
		{
			if (SetCalibrate2())
				SaveConfig(ADDRESS_CONFIG_START);
		}

		if (row == 3)
		{
			columnAxis += dataInc;
			if (columnAxis > 2)
				columnAxis = 2;
			if (columnAxis < 0)
				columnAxis = 0;
		}
		
		if (row == 4)
			mainConfig.pidConfig[columnAxis].kp += dataInc * 10;
		if (row == 5)
			mainConfig.pidConfig[columnAxis].ki += dataInc * 10;
		if (row == 6)
			mainConfig.pidConfig[columnAxis].kd += dataInc * 10;
		if (row == 7)
			mainConfig.filterConfig[columnAxis].gyro.r += dataInc * 1;
		if (row == 8 && column == 1)
		{
			SaveConfig(ADDRESS_CONFIG_START);
			column = 0;
		}

		//itoa(mainConfig.version, &stringArray[3][5], 10);
		itoa(mainConfig.pidConfig[columnAxis].kp, &stringArray[4][3], 10);
		itoa(mainConfig.pidConfig[columnAxis].ki, &stringArray[5][3], 10);
		itoa(mainConfig.pidConfig[columnAxis].kd, &stringArray[6][3], 10);
		itoa(mainConfig.filterConfig[columnAxis].gyro.r, &stringArray[7][3], 10);

		//char *tempString[9] = { "SPEKTRUM", "MIGUELS FC", "RACEFLIGHT", "ONE", row5, "ROLL", row7, row8, row9 };
		if (currentTime - blinkTime > 100)
		{	
			blinkTime = currentTime;
		}
		else if (currentTime - blinkTime > 50)
		{
			if (column == 0)
				strcpy(stringArray[row], "");
			else if (column == 1)
				strcpy(&stringArray[row][3], "");
		}
		//blank++;
		//blank = blank % 20;
	}
	//char *tempString[9] = { "SPEKTRUM", "MIGUELS FC", "RACEFLIGHT", "ONE", "TWO", "THREE", "FOUR", "TEST", "TEST" };
	//xbus.textLine = row;
	sensorData.user_text.identifier = TELE_DEVICE_TEXTGEN;
	sensorData.user_text.sID = 0x00;
	sensorData.user_text.lineNumber = xbus.textLine;
	
	memcpy(sensorData.user_text.text, stringArray[xbus.textLine], 13);
	xbus.textLine++;
	if (xbus.textLine > 8)
	{
		xbus.textLine = 0;
	}
	dataInc = 0;
}
/*
void sendSpektrumSRXL(uint32_t baseAddress, uint8_t packetSize)
{

	while (DMA_GetCurrDataCounter(spektrumUart->txDMAStream) != 0)
	{
		dma_count = DMA_GetCurrDataCounter(spektrumUart->txDMAStream);
	}

	DMA_DeInit(spektrumUart->txDMAStream);
	DMA_StructInit(&DMA_InitStructure);
	DMA_InitStructure.DMA_Channel = spektrumUart->txDMAChannel;
	DMA_InitStructure.DMA_PeripheralBaseAddr = spektrumUart->txDMAPeripheralBaseAddr;
	DMA_InitStructure.DMA_Memory0BaseAddr = baseAddress;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = packetSize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	//DMA_InitStructure.DMA_FIFOMode =
	//DMA_InitStructure.DMA_FIFOThreshold =
	//DMA_InitStructure.DMA_MemoryBurst =
	//DMA_InitStructure.DMA_PeripheralBurst =
	DMA_Init(spektrumUart->txDMAStream, &DMA_InitStructure);

	//Disable rx DMA and IT before sending
	//USART_DMACmd(spektrumUart->USARTx, USART_DMAReq_Rx, DISABLE);
	//USART_ITConfig(spektrumUart->USARTx, USART_IT_IDLE, DISABLE);  

	//Interrupt will be used to determine when message is complete and we can re-enable uart rx
	//DMA_ITConfig(DMA1_Stream7, DMA_IT_TC, ENABLE);

	//Send data

	DMA_Cmd(spektrumUart->txDMAStream, ENABLE);		
	
}


#endif
ITStatus idle = 0; 
ITStatus rxne = 0; 
uint32_t uart5_dr = 0;
void DMA1_Stream7_IRQHandler() {
  
  //Transmit is complete, we can re enable RX interrupt and DMA
	if (DMA_GetITStatus(DMA1_Stream7, DMA_IT_TCIF7)) {
		DMA_ClearITPendingBit(DMA1_Stream7, DMA_IT_TCIF7);
    
		//Clear RXNE register. This will prevent idle interrupt bit from being set after this transmission ends
		idle = USART_GetFlagStatus(spektrumUart->USARTx, USART_FLAG_IDLE);
		rxne = USART_GetITStatus(spektrumUart->USARTx, USART_FLAG_RXNE);
		//USART_ReceiveData(UART5);
		uart5_dr = UART5->DR;
		///USART_ClearFlag(spektrumUart->USARTx, USART_FLAG_RXNE);
		idle = USART_GetFlagStatus(spektrumUart->USARTx, USART_IT_IDLE);
		rxne = USART_GetITStatus(spektrumUart->USARTx, USART_IT_RXNE);
		//Clear idle flag and re-enable UART tx
		//USART_ClearITPendingBit(spektrumUart->USARTx, USART_IT_IDLE);
		DMA_ITConfig(DMA1_Stream7, DMA_IT_TC, DISABLE);
		USART_DMACmd(spektrumUart->USARTx, USART_DMAReq_Rx, ENABLE);
		USART_ITConfig(spektrumUart->USARTx, USART_IT_IDLE, ENABLE);  
	}
}

*/
