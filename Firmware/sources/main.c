/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
  /* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "adc.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "shared_definitions.h"
#include "sysutils.h"
#include "gsm.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t GSMStatus = 0;  // Состояние GSM-модуля: 0 - выключен ; 1 - включен и инициализирован
uint8_t GSM_Answer[MAXANSWERLENGTH];
volatile uint16_t GSM_AnswerWritePoint;
volatile uint8_t GSM_TotalReadByteCounter = 0;
uint8_t CurrentRXByte;
uint8_t IMEI[16];
uint8_t SERVICE_PROVIDER[16];
uint16_t Balance;
uint8_t BalanceInText[8];
uint8_t SMS_Message[140];
uint8_t ATD_Call[sizeof("ATD123456789123456;\r\n")];
bool SMS_SendingStatus = false;
bool IncomingCallStatus = false;
char SMS_PASS[5] = { "1234" };
uint8_t SMS_Commands[MAX_SMS_COMMANDS_AT_ONCE];
uint8_t ReceivedSMS_Commands = 0;
uint8_t LoopsState = 0;
uint8_t IncomingCallNumber[16];

uint32_t VMonitorADC_Code, AccMonitorADC_Code, AudioInADC_Code;

char MainStruct[MAINSTRUCTLENGTH];
uint16_t MainStructIndex;
volatile bool TimeToSaveFlashPack = false;
__no_init __root const uint8_t FlashPack[MAINSTRUCTLENGTH] @ PAGE1;
tmStruct * ROMStruct = (tmStruct *)FlashPack;
tmStruct *RAMStruct;

const char *DeviceName = "GSM Alarm"; // Не больше DEVICENAMELENGTH символов
const char *DeviceVersion = "0.0.57"; // Не больше STRINGLENGTH-1 символов
const char *DeviceController = "STM32F102RBT6"; // Не больше STRINGLENGTH-1 символов
const char *DeviceFirmwareChange = "16 nov 2016"; // Не больше STRINGLENGTH-1 символов
uint8_t DeviceIMEI[STRINGLENGTH];
uint8_t SIM1Operator[STRINGLENGTH];
uint8_t SIM1Balance[STRINGLENGTH];

uint8_t DataTransferState = DATAREQUESTNOPE;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

	/* USER CODE BEGIN 1 */


	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART3_UART_Init();
	MX_USB_DEVICE_Init();
	MX_ADC1_Init();

	/* USER CODE BEGIN 2 */
	HAL_Delay(400); // Ждем, пока устаканится генератор

	RAMStruct = (tmStruct *)MainStruct;

	HAL_GPIO_WritePin(YELLOW_LED_GPIO_Port, YELLOW_LED_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);

	BuzzerBuzzAndPause(3, 0);

	HAL_GPIO_WritePin(LOOPS_ON_GPIO_Port, LOOPS_ON_Pin, GPIO_PIN_SET);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	while (GSMStatus == 0) {
		SelectSIM1();
		GSM_On();
		GSM_Init();

		if ((strstr(GSM_Answer, "OK") != NULL) && (strstr(GSM_Answer, "ERROR") == NULL))
			if (WaitModuleAndSIMReady(30))
				if (ReadIMEI())
					if (ReadProviderName())
						if (ReadSIMBalance("#100#"))
							GSMStatus = 1;
	}

	strcpy(SMS_Message, "Ostatok sredstv ");
	strcat(SMS_Message, itoa(Balance, BalanceInText));
	strcat(SMS_Message, " rub.");

	//SMS_SendingStatus = SendSMS("89178098981", SMS_Message);
	if (strlen(ROMStruct->TelephoneNumber1) > 0)
		SMS_SendingStatus = SendSMS(ROMStruct->TelephoneNumber1, SMS_Message);

	if (SMS_SendingStatus)
	{
		BuzzerBuzzAndPause(3, 200);
		BuzzerBuzzAndPause(3, 200);
	}
	else
	{
		BuzzerBuzzAndPause(3, 200);
		BuzzerBuzzAndPause(3, 200);
		BuzzerBuzzAndPause(3, 200);
		BuzzerBuzzAndPause(3, 200);
	}

	HAL_Delay(8000); // Задержка после отправки SMS. Если пауза будет меньше, то следующая SMS может не отправиться (похоже на особенности GSM-модуля)

	while (1)
	{
		if (TimeToSaveFlashPack) // Заливаем данные из RAM в ROM
		{
			SaveFlashPack();
			TimeToSaveFlashPack = false;
		}

		if (HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin) == GPIO_PIN_RESET)
    {
			BuzzerBuzzAndPause(3, 1000);
      
      if ((strlen(ROMStruct->TelephoneNumber1) > 0) && (ROMStruct->InputDoSMS[0][0]))
      {
		    SMS_SendingStatus = SendSMS(ROMStruct->TelephoneNumber1, ROMStruct->InputReactionSMS1);
        HAL_Delay(8000);
        BuzzerBuzzAndPause(3, 1000);
      }
      
      if ((strlen(ROMStruct->TelephoneNumber2) > 0) && (ROMStruct->InputDoSMS[1][0]))
      {
		    SMS_SendingStatus = SendSMS(ROMStruct->TelephoneNumber2, ROMStruct->InputReactionSMS1);
        HAL_Delay(8000);
        BuzzerBuzzAndPause(3, 1000);
      }
      
      if ((strlen(ROMStruct->TelephoneNumber3) > 0) && (ROMStruct->InputDoSMS[2][0]))
      {
		    SMS_SendingStatus = SendSMS(ROMStruct->TelephoneNumber3, ROMStruct->InputReactionSMS1);
        HAL_Delay(8000);
        BuzzerBuzzAndPause(3, 1000);
      }
      
      if ((strlen(ROMStruct->TelephoneNumber4) > 0) && (ROMStruct->InputDoSMS[3][0]))
      {
		    SMS_SendingStatus = SendSMS(ROMStruct->TelephoneNumber4, ROMStruct->InputReactionSMS1);
        HAL_Delay(8000);
        BuzzerBuzzAndPause(3, 1000);
      }
      
      if ((strlen(ROMStruct->TelephoneNumber5) > 0) && (ROMStruct->InputDoSMS[4][0]))
      {
		    SMS_SendingStatus = SendSMS(ROMStruct->TelephoneNumber5, ROMStruct->InputReactionSMS1);
        HAL_Delay(8000);
        BuzzerBuzzAndPause(3, 1000);
      }
            
      if ((strlen(ROMStruct->TelephoneNumber6) > 0) && (ROMStruct->InputDoSMS[5][0]))
      {
		    SMS_SendingStatus = SendSMS(ROMStruct->TelephoneNumber6, ROMStruct->InputReactionSMS1);
        HAL_Delay(8000);
        BuzzerBuzzAndPause(3, 1000);
      }
      
      
      
      if ((strlen(ROMStruct->TelephoneNumber1) > 0) && (ROMStruct->InputDoCall[0][0]))
      {
		    Call(ROMStruct->TelephoneNumber1);
        HAL_Delay(4000);
        BuzzerBuzzAndPause(3, 1000);
      }
      
      if ((strlen(ROMStruct->TelephoneNumber2) > 0) && (ROMStruct->InputDoCall[1][0]))
      {
		    Call(ROMStruct->TelephoneNumber2);
        HAL_Delay(4000);
        BuzzerBuzzAndPause(3, 1000);
      }
      
      if ((strlen(ROMStruct->TelephoneNumber3) > 0) && (ROMStruct->InputDoCall[2][0]))
      {
		    Call(ROMStruct->TelephoneNumber3);
        HAL_Delay(4000);
        BuzzerBuzzAndPause(3, 1000);
      }

      if ((strlen(ROMStruct->TelephoneNumber4) > 0) && (ROMStruct->InputDoCall[3][0]))
      {
		    Call(ROMStruct->TelephoneNumber4);
        HAL_Delay(4000);
        BuzzerBuzzAndPause(3, 1000);
      }

      if ((strlen(ROMStruct->TelephoneNumber5) > 0) && (ROMStruct->InputDoCall[4][0]))
      {
		    Call(ROMStruct->TelephoneNumber5);
        HAL_Delay(4000);
        BuzzerBuzzAndPause(3, 1000);
      }
      
      if ((strlen(ROMStruct->TelephoneNumber6) > 0) && (ROMStruct->InputDoCall[5][0]))
      {
		    Call(ROMStruct->TelephoneNumber6);
        HAL_Delay(4000);
        BuzzerBuzzAndPause(3, 1000);
      }      
    }

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
		| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC | RCC_PERIPHCLK_USB;
	PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
	PeriphClkInit.UsbClockSelection = RCC_USBPLLCLK_DIV1;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

void BuzzerBuzzAndPause(uint16_t msBuzzTime, uint16_t msPauseTime)
{
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
	HAL_Delay(msBuzzTime);
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
	HAL_Delay(msPauseTime);
}

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
			/* User can add his own implementation to report the file name and line number,
			  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
			  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */

  /**
	* @}
  */

  /************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
