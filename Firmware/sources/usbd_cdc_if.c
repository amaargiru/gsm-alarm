#include "usbd_cdc_if.h"
#include "shared_definitions.h"

extern tmStruct *RAMStruct;
extern const char *DeviceName;
extern const char *DeviceVersion;
extern const char *DeviceController;
extern const char *DeviceFirmwareChange;
extern uint8_t DeviceIMEI[];
extern uint8_t SIM1Operator[];
extern uint8_t SIM1Balance[];

#define APP_RX_DATA_SIZE 64
#define APP_TX_DATA_SIZE 64
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

USBD_HandleTypeDef *hUsbDevice_0;

extern USBD_HandleTypeDef hUsbDeviceFS;

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t *pbuf, uint32_t *Len);

const uint16_t fcstab[] = {
0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

extern char MainStruct[];
extern uint16_t MainStructIndex;
uint16_t PacketSize = 0;
extern bool TimeToSaveFlashPack;

extern __no_init __root const uint8_t FlashPack[MAINSTRUCTLENGTH] @ PAGE1;

char WRITEREQUEST[] = "HardwareWriteRequest";
char WRITERESPONSE[] = "HardwareWriteResponseOK";
uint16_t WRITERESPONSELENGTH = sizeof(WRITERESPONSE) - 1;
char READREQUEST[] = "HardwareReadRequest";
char READRESPONSE[] = "HardwareReadResponseOK";
uint16_t READRESPONSELENGTH = sizeof(READRESPONSE) - 1;

extern uint8_t DataTransferState;

static uint16_t ComputeCRC16(char data[], uint16_t start, uint16_t length)
{
	uint16_t fcs = 0xFFFF;
	uint16_t end = start + length;

	for (uint16_t i = start; i < end; i++)
		fcs = (uint16_t)(((uint16_t)(fcs >> 8)) ^ fcstab[(fcs ^ data[i]) & 0xFF]);

	return (uint16_t)(~fcs);
}

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS = { CDC_Init_FS, CDC_DeInit_FS,
CDC_Control_FS, CDC_Receive_FS };

static int8_t CDC_Receive_FS(uint8_t *Buf, uint32_t *Len) {

	for (int i = 0; i < *Len; i++) {
		UserRxBufferFS[i] = Buf[i];

		if (DataTransferState == DATAWRITEPROCESS) // Идет запись пакета
		{
		//	HAL_GPIO_WritePin(GPIOC, Red_LED_Pin, GPIO_PIN_SET);

			MainStruct[MainStructIndex] = Buf[i];

			if (MainStructIndex == 1) // В первых двух байтах должна была прийти общая длина пакета
				PacketSize = *(uint16_t *)MainStruct;

			if (MainStructIndex == PacketSize - 1) // Пришел последний байт пакета
			{
				uint16_t checksum = ComputeCRC16(MainStruct, 0, PacketSize - 2); // При расчете контрольной суммы не учитываем два последних байта, содержащих собственно контрольную сумму
				uint16_t receivedchecksum = *(uint16_t *)(MainStruct + MainStructIndex - 1);

				if (checksum == receivedchecksum) // Правильная контрольная сумма
					TimeToSaveFlashPack = true;

				DataTransferState = DATAREQUESTNOPE;
			//	HAL_GPIO_WritePin(GPIOC, Red_LED_Pin, GPIO_PIN_RESET);
			}

			MainStructIndex++;
		}
	}

	if (DataTransferState == DATAREADPROCESS) // Отдаем пакет
	{
		if (!TimeToSaveFlashPack) // Пришедший пакет уже был переписан в ROM или его вообще не было
		{
			uint16_t PackLength = *(uint16_t *)FlashPack; // В первых двух байтах лежит общая длина пакета

			if ((PackLength >= 128) && (PackLength <= MAINSTRUCTLENGTH)) // Разумные размеры пакета
			{
				for (uint16_t i = 0; i < PackLength - 2; i++)  // Переливаем из ROM в RAM
					MainStruct[i] = FlashPack[i];
        
        RAMStruct = (tmStruct *)MainStruct;
				strcpy(RAMStruct->DeviceName, DeviceName); // Дополняем пакет данными, полученными от GSM модуля
				strcpy(RAMStruct->DeviceVersion, DeviceVersion);
				strcpy(RAMStruct->DeviceController, DeviceController);
				strcpy(RAMStruct->DeviceFirmwareChange, DeviceFirmwareChange);
				strcpy(RAMStruct->DeviceIMEI, DeviceIMEI);
				strcpy(RAMStruct->SIM1Operator, SIM1Operator);
				strcpy(RAMStruct->SIM1Balance, SIM1Balance);

				uint16_t checksum = ComputeCRC16(MainStruct, 0, PackLength - 2); // При расчете контрольной суммы не учитываем два последних байта, содержащих собственно контрольную сумму
				MainStruct[PackLength - 2] = checksum & 0x00ff; // Записываем контрольную сумму в конец передаваемого пакета
				MainStruct[PackLength - 1] = (checksum & 0xff00) >> 8;

				CDC_Transmit_FS(MainStruct, PackLength);
			}
		}

		DataTransferState = DATAREQUESTNOPE;
	}

	if (*Len > 0)
		if (strstr(UserRxBufferFS, WRITEREQUEST) != NULL)
			DataTransferState = DATAWRITEREQUEST;   // Запрос на запись данных с хоста
		else if (strstr(UserRxBufferFS, READREQUEST) != NULL)
			DataTransferState = DATAREADREQUEST;

	if (*Len > 0)
		if (DataTransferState == DATAWRITEREQUEST) { // Если хост хочет записать пакет данных, отвечаем согласием
			CDC_Transmit_FS(WRITERESPONSE, WRITERESPONSELENGTH);
			DataTransferState = DATAWRITEPROCESS; // и переходим в режим записи
			MainStructIndex = 0;
		}
		else if (DataTransferState == DATAREADREQUEST) {
			CDC_Transmit_FS(READRESPONSE, READRESPONSELENGTH);
			DataTransferState = DATAREADPROCESS; // и переходим в режим чтения
			MainStructIndex = 0;
		}
		USBD_CDC_ReceivePacket(hUsbDevice_0);

		return (USBD_OK);
}

static int8_t CDC_Init_FS(void) {
	hUsbDevice_0 = &hUsbDeviceFS;
	USBD_CDC_SetTxBuffer(hUsbDevice_0, UserTxBufferFS, 0);
	USBD_CDC_SetRxBuffer(hUsbDevice_0, UserRxBufferFS);
	return (USBD_OK);
}

static int8_t CDC_DeInit_FS(void) {
	return (USBD_OK);
}

static int8_t CDC_Control_FS(uint8_t cmd, uint8_t *pbuf, uint16_t length) {

	switch (cmd) {
	case CDC_SEND_ENCAPSULATED_COMMAND:
		break;
	case CDC_GET_ENCAPSULATED_RESPONSE:
		break;
	case CDC_SET_COMM_FEATURE:
		break;
	case CDC_GET_COMM_FEATURE:
		break;
	case CDC_CLEAR_COMM_FEATURE:
		break;
	case CDC_SET_LINE_CODING:
		break;
	case CDC_GET_LINE_CODING:
		break;
	case CDC_SET_CONTROL_LINE_STATE:
		break;
	case CDC_SEND_BREAK:
		break;
	default:
		break;
	}
	return (USBD_OK);
}

uint8_t CDC_Transmit_FS(uint8_t *Buf, uint16_t Len) {
	uint8_t result = USBD_OK;
	USBD_CDC_SetTxBuffer(hUsbDevice_0, Buf, Len);
	result = USBD_CDC_TransmitPacket(hUsbDevice_0);
	return result;
}