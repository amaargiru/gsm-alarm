#ifndef __sysutils_H
#define __sysutils_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "shared_definitions.h"

uint8_t LoopsControl(void);
bool Normal (uint32_t InputCode);
   
void SMS_CommandExecutor(void);
   
void Out1ON(void);
void Out1OFF(void);
void Out2ON(void);
void Out2OFF(void);
void Out3ON(void);
void Out3OFF(void);
void Out4ON(void);
void Out4OFF(void);

int StrToInt(const char *str);
char* itoa(int i, char b[]);

void SaveFlashPack(void);
void LoadFlashPack(void);

#ifdef __cplusplus
}
#endif
#endif