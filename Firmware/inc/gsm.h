#ifndef __gsm_H
#define __gsm_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "shared_definitions.h"

void GSM_Answer_Clear();
void GSM_On(void);
void GSM_Off(void);
void GSM_Init(void);
void SelectSIM1(void);
void SelectSIM1(void);
bool WaitModuleAndSIMReady(uint16_t DelayInSec);
bool ReadIMEI(void);
bool ReadProviderName(void);
bool ReadSIMBalance(char *Request);
bool SendSMS(char *Number, char *Text);
bool Call(char *Number);
uint8_t ReadCommandsFromSMS(void);
bool IncomingCallDetect(void);

#ifdef __cplusplus
}
#endif
#endif