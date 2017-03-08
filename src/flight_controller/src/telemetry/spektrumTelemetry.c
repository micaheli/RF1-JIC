#include "includes.h"


//extern uartPort_t *spektrumUart;
//#define SRXLTELEM_PACKET_LEN 21
STR_SRXL_TELEM telemetry;
STR_SRXL_BIND bind;

uint8_t spektrumTxBuffer[20];

extern STRU_TELE_LAPTIMER lap_timer;

DMA_InitTypeDef DMA_InitStructure;

uint8_t dma_count;
TELEMETRY_STATE telemetryState = TELEM_START;
UN_TELEMETRY sensorData;
extern uint32_t progMode;
pidSpektrumTelem_t pidSpektrumTelem;

#define UINT16_ENDIAN(a)  (((a) >> 8) | ((a) << 8) )




void InitSpektrumTelemetry(void) {
	pidSpektrumTelem.row = 2;
	pidSpektrumTelem.status = IDLE;


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
			telemetry.packet.data.internalSensors.packVoltage = UINT16_ENDIAN((uint16_t)(averageVoltage * 100));

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
	if (!telemEnabled)
			return;

	memcpy(spektrumTxBuffer, (uint8_t *)baseAddress, packetSize);

	for (uint32_t serialNumber = 0;serialNumber<MAX_USARTS;serialNumber++)
	{
		if ( (board.serials[serialNumber].enabled) && (mainConfig.telemConfig.telemSpek) )
		{
			if (board.serials[serialNumber].Protocol == USING_SPEK_T)
				HAL_UART_Transmit_DMA(&uartHandles[board.serials[serialNumber].usartHandle], (uint8_t *)spektrumTxBuffer, packetSize);
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



float dataInc = 0;

#define ROW_MAX 8
#define COLUMN_MAX 1
#define MAX_MENUS 6

char stringArray[9][15];
char axisTable[7][15] = { "Yaw PIDs", "Roll PIDs", "Pitch PIDs", "Yaw Rate", "Roll Rate", "Pitch Rate", "General" };

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
		pidSpektrumTelem.currentTime = InlineMillis();

/*------------------------------------------checking sticks and moving column and row accordingly--------------------------------------------*/
		if (progMode != 0)
		{
			//vertical stick
			if (rxData[2] > 1224 && pidSpektrumTelem.vStickStatus != 1)
			{
				if (pidSpektrumTelem.column == 0)
				{
					pidSpektrumTelem.row--;
				    //stringArray[row][0] = '>';
				}
				else
					dataInc = 1;
				//toggleTime = currentTime;

				pidSpektrumTelem.vStickStatus = 1;
			}
			else if (rxData[2] < 824 && pidSpektrumTelem.vStickStatus != -1)
			{
				if (pidSpektrumTelem.column == 0)
				{
					pidSpektrumTelem.row++;
					//stringArray[row][0] = '>';
				}
				else 
					dataInc = -1;
				//toggleTime = currentTime;
				pidSpektrumTelem.vStickStatus = -1;
			}
			else if (rxData[2] > 924 && rxData[2] < 1124)
			{
				pidSpektrumTelem.vStickStatus = 0;
			}

			//horizontal stick
			if (rxData[1] > 1224 && pidSpektrumTelem.hStickStatus != 1)
			{
				pidSpektrumTelem.column++;
				//toggleTime = currentTime;

				pidSpektrumTelem.hStickStatus = 1;
				pidSpektrumTelem.status=CHANGING_SETTING;
			}
			else if (rxData[1] < 824 && pidSpektrumTelem.hStickStatus != -1)
			{
				pidSpektrumTelem.column--;
				//toggleTime = currentTime;

				pidSpektrumTelem.hStickStatus = -1;
				pidSpektrumTelem.status=IDLE;

			}	
			else if (rxData[1] > 924 && rxData[1] < 1124)
			{
				pidSpektrumTelem.hStickStatus = 0;
			}
		}
		else
		{
			//have these initialized so nothing happens when entering prog mode for the first time
			pidSpektrumTelem.vStickStatus = -1;
			pidSpektrumTelem.hStickStatus = -1;
		}
						

		if (pidSpektrumTelem.row >= ROW_MAX)
			pidSpektrumTelem.row = ROW_MAX;
		else if (pidSpektrumTelem.row < 0)
			pidSpektrumTelem.row = 2;

		if (pidSpektrumTelem.column >= COLUMN_MAX)
			pidSpektrumTelem.column = COLUMN_MAX;
		else if (pidSpektrumTelem.column < 0 )
		{
			pidSpektrumTelem.row = 2;
			pidSpektrumTelem.column = 0;
			pidSpektrumTelem.columnAxis = 0;
			progMode = 0;
			InitPid(); //Set PID's with new config PID's
		}
			

/*--------------------------------Main Logic-----------------------------------------------------------------------------------*/

        if (pidSpektrumTelem.row == 2)
        	{
        	pidSpektrumTelem.columnAxis += dataInc;
        	if (pidSpektrumTelem.columnAxis > MAX_MENUS)
        		pidSpektrumTelem.columnAxis = MAX_MENUS;
        	if (pidSpektrumTelem.columnAxis < 0)
        		pidSpektrumTelem.columnAxis = 0;
        	} //checks so that the user cant go out of bounds, happens every cycle



        if (pidSpektrumTelem.columnAxis >= 0 && pidSpektrumTelem.columnAxis <= 2 )
        {

        	//set menu, Always have a space before
			strcpy(stringArray[3], " P: ");
			strcpy(stringArray[4], " I: ");
			strcpy(stringArray[5], " D: ");
			strcpy(stringArray[6], " Filter: ");
			strcpy(stringArray[7], " GA: ");
			strcpy(stringArray[8], " Save");

			//checking each row, TODO:maybe make a switch case
			switch(pidSpektrumTelem.row)
			{
				case (3):
					mainConfig.pidConfig[pidSpektrumTelem.columnAxis].kp += dataInc * 10;
				    break;
				case (4):
		            mainConfig.pidConfig[pidSpektrumTelem.columnAxis].ki += dataInc * 10;
				    break;
				case (5):
		            mainConfig.pidConfig[pidSpektrumTelem.columnAxis].kd += dataInc * 50;
				    break;
				case (6):
					mainConfig.filterConfig[pidSpektrumTelem.columnAxis].gyro.q += dataInc * 5;
				    break;
				case (7):
					mainConfig.pidConfig[pidSpektrumTelem.columnAxis].ga += dataInc * 1;
				    break;
				case (8):
				    if (pidSpektrumTelem.column ==1)
				    {
				    	SaveConfig(ADDRESS_CONFIG_START);
						pidSpektrumTelem.column = 0;
						pidSpektrumTelem.status=SAVING;
						pidSpektrumTelem.waitTime=pidSpektrumTelem.currentTime;
				    }
				    break;

			}

			//fills in the rows with the data
			itoa(mainConfig.pidConfig[pidSpektrumTelem.columnAxis].kp, &stringArray[3][3], 10);
			itoa(mainConfig.pidConfig[pidSpektrumTelem.columnAxis].ki, &stringArray[4][3], 10);
			itoa(mainConfig.pidConfig[pidSpektrumTelem.columnAxis].kd, &stringArray[5][3], 10);
			itoa(mainConfig.filterConfig[pidSpektrumTelem.columnAxis].gyro.q, &stringArray[6][8], 10);
			itoa(mainConfig.pidConfig[pidSpektrumTelem.columnAxis].ga, &stringArray[7][4], 10);
        }

        if (pidSpektrumTelem.columnAxis > 2 && pidSpektrumTelem.columnAxis < 6)
		{
        	//set menu, Always have a space before
			strcpy(stringArray[3], " Rate:");
			strcpy(stringArray[4], " Expo:");
			strcpy(stringArray[5], " Acro:");
			strcpy(stringArray[6], "  ");
			strcpy(stringArray[7], "  ");
			strcpy(stringArray[8], " Save");

			//checking each row/changing config
			switch(pidSpektrumTelem.row)
			{
				case (3):
					mainConfig.rcControlsConfig.rates[pidSpektrumTelem.columnAxis-3] += dataInc * 10; // subtract three to make axis line up with enumeration
				    break;
				case (4):
					mainConfig.rcControlsConfig.acroPlus[pidSpektrumTelem.columnAxis-3] += dataInc * 10;
				    break;
				case (5):
			        mainConfig.rcControlsConfig.curveExpo[pidSpektrumTelem.columnAxis-3] += dataInc * 50;
				    break;
				case (8):
		            if (pidSpektrumTelem.column ==1)
					{
					 	SaveConfig(ADDRESS_CONFIG_START);
					 	pidSpektrumTelem.column = 0;
						pidSpektrumTelem.status=SAVING;
						pidSpektrumTelem.waitTime=pidSpektrumTelem.currentTime;
					}

				    break;

			}

        	//fills in the rows with the data
			itoa(mainConfig.rcControlsConfig.rates[pidSpektrumTelem.columnAxis-3], &stringArray[3][6], 10);
			itoa(mainConfig.rcControlsConfig.curveExpo[pidSpektrumTelem.columnAxis-3], &stringArray[4][6], 10);
			itoa(mainConfig.rcControlsConfig.acroPlus[pidSpektrumTelem.columnAxis-3], &stringArray[5][6], 10);


		}

        if (pidSpektrumTelem.columnAxis == 6)
		{
        	//led modes
        	//led count
        	//
        	//
        	//
        	//
        	//set menu, Always have a space before
			strcpy(stringArray[3], " LedCount:");
			strcpy(stringArray[4], " ");
			strcpy(stringArray[5], " ");
			strcpy(stringArray[6], "  ");
			strcpy(stringArray[7], "  ");
			strcpy(stringArray[8], " Save");

			//checking each row/changing config
        	switch(pidSpektrumTelem.row)
					{
						case (3):
							mainConfig.ledConfig.ledCount += dataInc;
							break;
						case (4):
							break;
						case (5):
							break;
						case (8):
							if (pidSpektrumTelem.column ==1)
							{
								SaveConfig(ADDRESS_CONFIG_START);
								pidSpektrumTelem.column = 0;
								pidSpektrumTelem.status=SAVING;
								pidSpektrumTelem.waitTime=pidSpektrumTelem.currentTime;
							}

							break;

					}

					//fills in the rows with the data
					itoa(mainConfig.ledConfig.ledCount, &stringArray[3][10], 10);

		}
/*------------------------------Switch for different status-----------------------------------------------------------------------*/
		switch(pidSpektrumTelem.status)
		{

			case (SAVING):
				strcpy(stringArray[0], " Saved");
				if (pidSpektrumTelem.currentTime-pidSpektrumTelem.waitTime > 500)
					pidSpektrumTelem.status=IDLE;
				break;

			case (IDLE):
				if (pidSpektrumTelem.row < 2)
					pidSpektrumTelem.row = 2;
				strcpy(stringArray[0], " RF1 Tuning");
				strcpy(stringArray[1], "------------------------ ");
				strcpy(stringArray[2], " ");
				strcpy(&stringArray[2][1], axisTable[pidSpektrumTelem.columnAxis]);


				if (progMode)
				{
					stringArray[pidSpektrumTelem.row][0] = '>';
				}

				break;
			case (CHANGING_SETTING):
				strcpy(&stringArray[2][1], axisTable[pidSpektrumTelem.columnAxis]);
				stringArray[pidSpektrumTelem.row][0] = '*';
				break;
		}


	}

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
}//end textMenuUpdate()









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
