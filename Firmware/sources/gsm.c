#include "gsm.h"

extern uint8_t GSM_Answer[MAXANSWERLENGTH];
extern uint16_t GSM_AnswerWritePoint;
extern uint8_t CurrentRXByte;
extern uint8_t GSM_TotalReadByteCounter;
extern uint8_t IMEI[16];
extern uint8_t SERVICE_PROVIDER[16];
extern uint16_t Balance;
extern uint8_t ATD_Call[sizeof("ATD123456789123456;\r\n")];
extern char SMS_PASS[5];
extern uint8_t SMS_Commands[MAX_SMS_COMMANDS_AT_ONCE];
extern uint8_t IncomingCallNumber[16];

extern UART_HandleTypeDef huart3;

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_IT(&huart3, (uint8_t *)&CurrentRXByte, 1);// Continue receiving
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if (CurrentRXByte >= 0x20) // Отсекаем служебные символы
	{
		GSM_Answer[GSM_AnswerWritePoint] = CurrentRXByte;

		if (GSM_AnswerWritePoint < MAXANSWERLENGTH - 1) GSM_AnswerWritePoint++;
		else GSM_AnswerWritePoint = 0;

		GSM_TotalReadByteCounter++;
	}

	HAL_UART_Receive_IT(&huart3, (uint8_t *)&CurrentRXByte, 1);// Continue receiving
}

int putchar(int data)
{
	HAL_UART_Transmit(&huart3, (uint8_t*)&data, 1, 20);
	return data;
}

void GSM_Answer_Clear()
{
	memset(GSM_Answer, 0, sizeof(GSM_Answer));
	GSM_AnswerWritePoint = 0;
}

void GSM_On(void)     // Включение GSM-модуля
{
	HAL_GPIO_WritePin(PWR_ON_GPIO_Port, PWR_ON_Pin, GPIO_PIN_SET);
	HAL_Delay(2000);

	HAL_GPIO_WritePin(PWR_KEY_GPIO_Port, PWR_KEY_Pin, GPIO_PIN_RESET);
	HAL_Delay(3000);
	HAL_GPIO_WritePin(PWR_KEY_GPIO_Port, PWR_KEY_Pin, GPIO_PIN_SET);
	HAL_Delay(8000);
	HAL_GPIO_WritePin(PWR_KEY_GPIO_Port, PWR_KEY_Pin, GPIO_PIN_RESET);
	HAL_Delay(12000);

	HAL_UART_Receive_IT(&huart3, &CurrentRXByte, 1); // Start receiving
	GSM_Answer_Clear();
}

void GSM_Off(void)     // Выключение GSM-модуля
{
	HAL_GPIO_WritePin(PWR_KEY_GPIO_Port, PWR_KEY_Pin, GPIO_PIN_SET);
	HAL_Delay(4000);
	HAL_GPIO_WritePin(PWR_KEY_GPIO_Port, PWR_KEY_Pin, GPIO_PIN_RESET);
	HAL_Delay(4000);

	HAL_GPIO_WritePin(PWR_ON_GPIO_Port, PWR_ON_Pin, GPIO_PIN_RESET);
	HAL_Delay(500);
}

void GSM_Init(void)
{
	GSM_Answer_Clear();
	printf("AT\r\n"); // Настраиваем скорость связи
	HAL_Delay(2000);
	printf("ATE0\r\n"); // Отключаем эхо в UART, чтобы не забивать наш приемный буфер лишней информацией
	HAL_Delay(2000);
	printf("AT+CHFA=0\r\n"); // Выбираем аудиоканал
	HAL_Delay(2000);
	printf("AT+CLVL=90\r\n"); // Уровень усиления динамика
	HAL_Delay(2000);
	printf("AT+CMIC=0,15\r\n"); // Чувствительность микрофона
	HAL_Delay(2000);
	printf("AT+CSCS=\"GSM\"\r\n");
	HAL_Delay(2000);
	printf("AT+CLIP=1\r\n"); // Включаем АОН
	HAL_Delay(2000);
	printf("AT+CMGF=1\r\n"); // Текстовый режим (не PDU)
	HAL_Delay(2000);
}

void SelectSIM1(void)
{
	HAL_GPIO_WritePin(SIM2_GPIO_Port, SIM2_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(SIM1_GPIO_Port, SIM1_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
}

void SelectSIM2(void)
{
	HAL_GPIO_WritePin(SIM1_GPIO_Port, SIM1_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(SIM2_GPIO_Port, SIM2_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
}

bool WaitModuleAndSIMReady(uint16_t DelayInSec)
{
	uint16_t counter = 0;

	GSM_Answer_Clear();
	printf("AT+CPAS\r\n");

	while ((strstr(GSM_Answer, "0") == NULL) && (counter < DelayInSec))
	{
		HAL_Delay(1000);
		counter++;
	}

	if (counter == DelayInSec)
		return false;
	else {
		HAL_Delay(2000);
		return true;
	}
}

bool ReadIMEI(void)
{
#define MAX_TIME_TO_WAIT_IMEI 10
	uint8_t counter = 0;

	GSM_Answer_Clear();
	printf("AT+GSN\r\n");

	while ((strstr(GSM_Answer, "OK") == NULL) && (counter < MAX_TIME_TO_WAIT_IMEI))
	{
		HAL_Delay(1000);
		counter++;
	}

	if (counter == MAX_TIME_TO_WAIT_IMEI)
		return false;
	else
	{
		strncpy(IMEI, GSM_Answer, 15);
		IMEI[15] = '\0';

		return true;
	}
}

bool ReadProviderName(void)
{
#define MAX_TIME_TO_WAIT_SERVICE_PROVIDER_NAME 20
	uint8_t counter = 0;

	GSM_Answer_Clear();
	printf("AT+CSPN?\r\n");

	while ((strstr(GSM_Answer, "OK") == NULL) && (counter < MAX_TIME_TO_WAIT_SERVICE_PROVIDER_NAME))
	{
		HAL_Delay(1000);
		counter++;
	}

	char *bracket1 = strchr(GSM_Answer, '"');
	char *bracket2 = strchr(bracket1 + 1, '"');

	if ((bracket1 != NULL) && (bracket2 != NULL))
	{
		memmove(GSM_Answer, bracket1 + 1, strlen(GSM_Answer) - (bracket1 - GSM_Answer) + 1);

		bracket2 = strchr(GSM_Answer, '"');
		memset(bracket2, 0, sizeof(GSM_Answer) - (bracket2 - GSM_Answer));
		strncpy(SERVICE_PROVIDER, GSM_Answer, 15);
		SERVICE_PROVIDER[15] = '\0';
		return true;
	}
	else return false;
}

bool ReadSIMBalance(char *Request)
{
#define MAX_TIME_TO_WAIT_BALANCE 20
	uint8_t counter = 0;
	uint16_t bal = 0;

	GSM_Answer_Clear();
	printf("AT+CUSD=1,\"%s\"\r\n", Request);

	while ((strstr(GSM_Answer, "\"") == NULL) && (counter < MAX_TIME_TO_WAIT_BALANCE))
	{
		HAL_Delay(1000);
		counter++;
	}

	if (counter == MAX_TIME_TO_WAIT_BALANCE)
		return false;

	char *bracket1 = strchr(GSM_Answer, '"');
	char *bracket2 = strchr(bracket1 + 1, '"');

	if ((bracket1 != NULL) && (bracket2 != NULL))
	{
		memmove(GSM_Answer, bracket1 + 1, strlen(GSM_Answer) - (bracket1 - GSM_Answer) + 1);

		bracket2 = strchr(GSM_Answer, '"');
		memset(bracket2, 0, sizeof(GSM_Answer) - (bracket2 - GSM_Answer));

		bal = StrToInt(GSM_Answer);
		if (bal >= 0)
		{
			Balance = bal;
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

bool SendSMS(char *Number, char *Text)
{
#define MAX_TIME_TO_WAIT_QUERY 10
#define MAX_TIME_TO_WAIT_SMS_SENDING 30

	uint8_t counter = 0;

	GSM_Answer_Clear();
	HAL_Delay(2000);

	printf("AT+CMGS=\"%s\"\r\n", Number);
	HAL_Delay(1000);

	while ((strstr(GSM_Answer, ">") == NULL) && (counter < MAX_TIME_TO_WAIT_QUERY))
	{
		HAL_Delay(1000);
		counter++;
	}

	if (counter == MAX_TIME_TO_WAIT_QUERY)
		return false;

	HAL_Delay(2000);
	printf("%s%c", Text, CtrlZ);

	while ((strstr(GSM_Answer, "OK") == NULL) && (counter < MAX_TIME_TO_WAIT_SMS_SENDING))
	{
		HAL_Delay(1000);
		counter++;
	}

	if (counter == MAX_TIME_TO_WAIT_SMS_SENDING)
		return false;

	HAL_Delay(2000);
	return true;
}

bool Call(char *Number)
{
#define MAX_TIME_TO_WAIT_CALL_ANSWER 120

	uint8_t counter = 0;
	bool AnswerReceived = false;
	bool UserInformed = false;

	strcpy(ATD_Call, "ATD");
	strcat(ATD_Call, Number);
	strcat(ATD_Call, ";\r\n");

	GSM_Answer_Clear();
	printf("%s", ATD_Call);

	do {
		HAL_Delay(1000);
		counter++;

		if (strstr(GSM_Answer, "NO ANSWER") != NULL)
		{
			AnswerReceived = true; // Не дозвонились, никто не взял трубку, не сбросил звонок, либо телефон абонента отключен
			UserInformed = false;
		}

		if (strstr(GSM_Answer, "NO DIALTONE") != NULL)
		{
			AnswerReceived = true; // По какой-то причине не можем осуществлять исходящие звонки
			UserInformed = false;
		}

		if (strstr(GSM_Answer, "BUSY") != NULL)
		{
			AnswerReceived = true;// Звонок сбросили = пользователь проинформирован
			UserInformed = true;
		}

		if (strstr(GSM_Answer, "NO CARRIER") != NULL)
		{
			AnswerReceived = true;// Взяли трубку, потом сбросили звонок = пользователь проинформирован
			UserInformed = true;
		}
	} while ((!AnswerReceived) && (counter < MAX_TIME_TO_WAIT_CALL_ANSWER));

	return UserInformed;
}

uint8_t ReadCommandsFromSMS(void)
{
#define MAX_TIME_TO_READ_SMS 20

	uint8_t counter = 0;
	int Command = 0;
	int CommandPos = 0;

	memset(SMS_Commands, 0, sizeof(SMS_Commands));

	GSM_Answer_Clear();
	printf("AT+CMGL=\"REC UNREAD\"\r\n");
	HAL_Delay(1000);

	while ((strstr(GSM_Answer, "OK") == NULL) && (counter < MAX_TIME_TO_READ_SMS))
	{
		HAL_Delay(1000);
		counter++;
	}

	if (counter == MAX_TIME_TO_READ_SMS)
		return 0;

	char *start = strstr(GSM_Answer, SMS_PASS);

	while ((start != NULL) && (CommandPos < MAX_SMS_COMMANDS_AT_ONCE))
	{
		memmove(GSM_Answer, start + 5, strlen(GSM_Answer) - (start - GSM_Answer) + 1);
		Command = StrToInt(GSM_Answer);
		if ((Command > 0) && (Command < 255))
			SMS_Commands[CommandPos++] = Command;

		start = strstr(GSM_Answer, SMS_PASS);
	}

	GSM_Answer_Clear();
	printf("AT+CMGDA=\"DEL ALL\"\r\n");
	HAL_Delay(3000);

	return CommandPos;
}

bool IncomingCallDetect(void)
{
	if (strstr(GSM_Answer, "RING") != NULL)
	{
		char *bracket1 = strchr(GSM_Answer, '"');
		char *bracket2 = strchr(bracket1 + 1, '"');

		if ((bracket1 != NULL) && (bracket2 != NULL))
		{
			memmove(GSM_Answer, bracket1 + 1, strlen(GSM_Answer) - (bracket1 - GSM_Answer) + 1);
			bracket2 = strchr(GSM_Answer, '"');
			memset(bracket2, 0, sizeof(GSM_Answer) - (bracket2 - GSM_Answer));
			strcpy(IncomingCallNumber, GSM_Answer);

			if (strlen(IncomingCallNumber) > 0)
				return true;
			else
				return false;
		}
	}
	return false;
}