#include "sysutils.h"
#include "adc.h"

#define OUT1OFF 10
#define OUT1ON  11
#define OUT2OFF 12
#define OUT2ON  13
#define OUT3OFF 14
#define OUT3ON  15
#define OUT4OFF 16
#define OUT4ON  17
#define SYSTEM_RESET  55

extern uint8_t SMS_Commands[MAX_SMS_COMMANDS_AT_ONCE];
extern uint8_t ReceivedSMS_Commands;

extern char MainStruct[MAINSTRUCTLENGTH];
extern __no_init __root const uint8_t FlashPack[MAINSTRUCTLENGTH] @ PAGE1; //__no_init

uint8_t LoopsControl(void)
{
  uint8_t LoopState = 0;
  
  MX_ADC1_SelectChannel(LOOP1);
  if (!Normal(GetADC1Value())){
    HAL_Delay(LOOPDELAY);
    if (!Normal(GetADC1Value()))
      LoopState |= 1;
  }
    
 	MX_ADC1_SelectChannel(LOOP2);
  if (!Normal(GetADC1Value())){
    HAL_Delay(LOOPDELAY);
    if (!Normal(GetADC1Value()))
      LoopState |= 2;
  }

	MX_ADC1_SelectChannel(LOOP3);
  if (!Normal(GetADC1Value())){
    HAL_Delay(LOOPDELAY);
    if (!Normal(GetADC1Value()))
      LoopState |= 4;
  }
    
 	MX_ADC1_SelectChannel(LOOP4);
  if (!Normal(GetADC1Value())){
    HAL_Delay(LOOPDELAY);
    if (!Normal(GetADC1Value()))
      LoopState |= 8;
  }
    
 	MX_ADC1_SelectChannel(LOOP5);
  if (!Normal(GetADC1Value())){
    HAL_Delay(LOOPDELAY);
    if (!Normal(GetADC1Value()))
      LoopState |= 16;
  }

	MX_ADC1_SelectChannel(LOOP6);
  if (!Normal(GetADC1Value())){
    HAL_Delay(LOOPDELAY);
    if (!Normal(GetADC1Value()))
      LoopState |= 32;
  }
  
  return LoopState;
}

bool Normal (uint32_t InputCode)
{
 if ((InputCode > LOWNORM) && (InputCode < HINORM))
   return true;
 else
   return false;
}

void SMS_CommandExecutor(void)
{
  uint8_t counter = 0;
  
  if (ReceivedSMS_Commands == 0)
    return;
  
  while (counter < ReceivedSMS_Commands)
  {
    switch (SMS_Commands[counter])
    {
      case OUT1OFF:
        Out1OFF();
        break;
        
      case OUT1ON:
        Out1ON();
        break;
        
      case OUT2OFF:
        Out2OFF();
        break;
        
      case OUT2ON:
        Out2ON();
        break;
        
      case OUT3OFF:
        Out3OFF();
        break;
        
      case OUT3ON:
        Out3ON();
        break;
        
      case OUT4OFF:
        Out4OFF();
        break;
        
      case OUT4ON:
        Out4ON();
        break;
        
      case SYSTEM_RESET:
        BuzzerBuzzAndPause(200, 0);
        NVIC_SystemReset();
        break;
    }
    
    counter++;
  }
}

void Out1ON(void) // Out 1 = Реле K2
{
  HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
}

void Out1OFF(void) // Out 1 = Реле K2
{
  HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
}

void Out2ON(void) // Out 2 = Реле K1
{
  HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
}

void Out2OFF(void) // Out 2 = Реле K1
{
  HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
}

void Out3ON(void)
{
  HAL_GPIO_WritePin(OUTA_GPIO_Port, OUTA_Pin, GPIO_PIN_SET);
}

void Out3OFF(void)
{
  HAL_GPIO_WritePin(OUTA_GPIO_Port, OUTA_Pin, GPIO_PIN_RESET);
}

void Out4ON(void)
{
  HAL_GPIO_WritePin(OUTD_GPIO_Port, OUTD_Pin, GPIO_PIN_SET);
}

void Out4OFF(void)
{
  HAL_GPIO_WritePin(OUTD_GPIO_Port, OUTD_Pin, GPIO_PIN_RESET);
}

int StrToInt(const char *str) {
	while (*str) {
		int number;
		if (sscanf(str, "%d", &number) == 1) {
			return number;
		}
		str++;
	}
	return -1;
}

char* itoa(int i, char b[]) {
	char const digit[] = "0123456789";
	char* p = b;
	if (i < 0) {
		*p++ = '-';
		i *= -1;
	}
	int shifter = i;
	do { //Move to where representation ends
		++p;
		shifter = shifter / 10;
	} while (shifter);
	*p = '\0';
	do { //Move back, inserting digits as u go
		*--p = digit[i % 10];
		i = i / 10;
	} while (i);
	return b;
}

void SaveFlashPack(void)
{
	HAL_FLASH_Unlock();
	FLASH_PageErase(PAGE1);
	FLASH_PageErase(PAGE1 + 0x400);
	FLASH_PageErase(PAGE1 + 0x800);
	CLEAR_BIT(FLASH->CR, FLASH_CR_PER);

	for (uint32_t index = 0; index < *(uint16_t *)MainStruct; index += 2)
		HAL_FLASH_Program(TYPEPROGRAM_HALFWORD, PAGE1 + index, *(uint16_t *)(MainStruct + index));

	HAL_FLASH_Lock();
}

void LoadFlashPack(void)
{
	uint16_t PackLength = *(uint16_t *)FlashPack; // В первых двух байтах лежит общая длина пакета

	if ((PackLength > 128) && (PackLength < 4096)) // Разумные размеры пакета
	{
		for (uint16_t i = 0; i < PackLength - 2; i++)  // Переливаем из ROM в RAM
			MainStruct[i] = FlashPack[i];
	}
}