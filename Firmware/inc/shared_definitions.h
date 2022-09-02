#ifndef __shared_h
#define __shared_h

#include "stdbool.h"
#include "string.h"
#include "sysutils.h"

#define MAXANSWERLENGTH 500

#define CtrlZ					0x1A

#define MAX_SMS_COMMANDS_AT_ONCE 15

#define VMONITOR ADC_CHANNEL_2
#define ACCMONITOR ADC_CHANNEL_4
#define LOOP2 ADC_CHANNEL_6
#define LOOP1 ADC_CHANNEL_5
#define LOOP3 ADC_CHANNEL_7
#define LOOP6 ADC_CHANNEL_8
#define AUDIO_IN ADC_CHANNEL_9
#define LOOP5 ADC_CHANNEL_15
#define LOOP4 ADC_CHANNEL_14

#define LOWNORM 1040
#define HINORM 2200

#define LOOPDELAY 300

#define DATAREQUESTNOPE   0 

#define DATAWRITEREQUEST  1
#define DATAREADREQUEST   2

#define DATAWRITEPROCESS  3

#define DATAREADPROCESS   4

#define MAINSTRUCTLENGTH  (4250)

#define PAGE1             0x0801E400

#define DEVICENAMELENGTH  62
#define REQUESTLENGTH     31
#define STRINGLENGTH      16
#define TELEPHONELENGTH   15
#define SMSLENGTH         159
#define DELAYLENGTH       4
#define REACTTIMELENGTH   4
#define SMSPASSLENGTH     4
#define RADIOSENSORLENGTH 19
#define IBUTNUMLENGTH     12

#pragma pack(1)
__packed typedef struct
{
	uint16_t PackLength;
	// Вкладка "Информация"
	uint8_t DeviceName[DEVICENAMELENGTH];
	uint8_t DeviceVersion[STRINGLENGTH];
	uint8_t DeviceIMEI[STRINGLENGTH];
	uint8_t DeviceController[STRINGLENGTH];
	uint8_t DeviceFirmwareChange[REQUESTLENGTH];
	uint8_t SIM1Operator[STRINGLENGTH];
	uint8_t SIM1Balance[STRINGLENGTH];
	uint8_t SIM2Operator[STRINGLENGTH];
	uint8_t SIM2Balance[STRINGLENGTH];
	uint8_t SIMActivityMode;
	uint8_t SIM1BalanceRequest[REQUESTLENGTH];
	uint8_t SIM2BalanceRequest[REQUESTLENGTH];
	uint8_t MinBalance[REQUESTLENGTH];

	// Вкладка "Общие"
	uint8_t ProtectionSetDelayInS[DELAYLENGTH];
	uint8_t ManagementModeIbutton;
	uint8_t ManagementModeWifi;
	uint8_t ManagementModeRadio;
	uint8_t ManagementModeSwitch;
	uint8_t PowerOffEvent;
	uint8_t PowerOffSMS[SMSLENGTH];
	uint8_t PowerOffReactionTime[REACTTIMELENGTH];
	uint8_t PowerOffGSMManagementMode;
	uint8_t SplitTo2SectionsMode;
	uint8_t AlarmONVerification;
	uint8_t AlarmONVerificationSMS[SMSLENGTH];
	uint8_t AlarmOFFVerification;
	uint8_t AlarmOFFVerificationSMS[SMSLENGTH];
	uint8_t BuzzerVerification;
	uint8_t SirenVerification;
	uint8_t SMSPass[SMSPASSLENGTH];

	// Вкладка "Телефоны"
	uint8_t TelephoneNumber1[TELEPHONELENGTH];
	uint8_t TelephoneNumber2[TELEPHONELENGTH];
	uint8_t TelephoneNumber3[TELEPHONELENGTH];
	uint8_t TelephoneNumber4[TELEPHONELENGTH];
	uint8_t TelephoneNumber5[TELEPHONELENGTH];
	uint8_t TelephoneNumber6[TELEPHONELENGTH];
	uint8_t SMSInfo1;
	uint8_t SMSInfo2;
	uint8_t SMSInfo3;
	uint8_t SMSInfo4;
	uint8_t SMSInfo5;
	uint8_t SMSInfo6;
	uint8_t SMSManagement1;
	uint8_t SMSManagement2;
	uint8_t SMSManagement3;
	uint8_t SMSManagement4;
	uint8_t SMSManagement5;
	uint8_t SMSManagement6;
	uint8_t CallAttempt;

	// Вкладка "Входы"
	uint8_t InputState[6];
	uint8_t InputReactionTime[6];
	uint8_t InputDoCall[6][6];
	uint8_t InputDoSMS[6][6];
	uint8_t InputReactionSMS1[SMSLENGTH];
	uint8_t InputReactionSMS2[SMSLENGTH];
	uint8_t InputReactionSMS3[SMSLENGTH];
	uint8_t InputReactionSMS4[SMSLENGTH];
	uint8_t InputReactionSMS5[SMSLENGTH];
	uint8_t InputReactionSMS6[SMSLENGTH];
	uint8_t InputNoctidialSecurity[6];
	uint8_t InputInternalSignaling[6];

	// Вкладка "Радиодатчики"
	uint8_t RadioSensorNum1[RADIOSENSORLENGTH];
	uint8_t RadioSensorNum2[RADIOSENSORLENGTH];
	uint8_t RadioSensorNum3[RADIOSENSORLENGTH];
	uint8_t RadioSensorNum4[RADIOSENSORLENGTH];
	uint8_t RadioSensorNum5[RADIOSENSORLENGTH];
	uint8_t RadioSensorNum6[RADIOSENSORLENGTH];
	uint8_t RadioSensorNum7[RADIOSENSORLENGTH];
	uint8_t RadioSensorNum8[RADIOSENSORLENGTH];
	uint8_t RadioSensorNum9[RADIOSENSORLENGTH];
	uint8_t RadioSensorNum10[RADIOSENSORLENGTH];
	uint8_t RadioSensorNum11[RADIOSENSORLENGTH];
	uint8_t RadioSensorNum12[RADIOSENSORLENGTH];

	uint8_t RadioSensorWrite1;
	uint8_t RadioSensorWrite2;
	uint8_t RadioSensorWrite3;
	uint8_t RadioSensorWrite4;
	uint8_t RadioSensorWrite5;
	uint8_t RadioSensorWrite6;
	uint8_t RadioSensorWrite7;
	uint8_t RadioSensorWrite8;
	uint8_t RadioSensorWrite9;
	uint8_t RadioSensorWrite10;
	uint8_t RadioSensorWrite11;
	uint8_t RadioSensorWrite12;

	// Вкладка "Выходы"
	uint8_t OutputState[6];
	uint8_t AlarmOn[6];
	uint8_t AlarmOnMin[6];
	uint8_t OffReaction[6];
	uint8_t OffReactionSec[6];
	uint8_t AlarmReaction[6];
	uint8_t AlarmReactionMin[6];
	uint8_t OnOffReaction[6];
	uint8_t TempSensorConnect[6];
	uint8_t SMSControl[6];
	uint8_t SMSOn[6];
	uint8_t SMSOnTime[6];
	uint8_t SMSOn4Sec[6];
	uint8_t ReturnSMSState[6];
	uint8_t ReturnSMS1[SMSLENGTH];
	uint8_t ReturnSMS2[SMSLENGTH];
	uint8_t ReturnSMS3[SMSLENGTH];
	uint8_t ReturnSMS4[SMSLENGTH];
	uint8_t ReturnSMS5[SMSLENGTH];
	uint8_t ReturnSMS6[SMSLENGTH];

	// Вкладка "Термоконтроль"
	uint8_t TempBorder1;
	uint8_t TempBorder2;
	uint8_t TempBorder3;
	uint8_t InternalAlarmState1;
	uint8_t InternalAlarmState2;
	uint8_t InternalAlarmState3;
	uint8_t Tmin1;
	uint8_t Tmin2;
	uint8_t Tmin3;
	uint8_t Tmax1;
	uint8_t Tmax2;
	uint8_t Tmax3;
	uint8_t ThermoSMS1[SMSLENGTH];
	uint8_t ThermoSMS2[SMSLENGTH];
	uint8_t ThermoSMS3[SMSLENGTH];

	// Вкладка "Ключи/радиопульты"
	uint8_t ibutSerialNumber1[IBUTNUMLENGTH];
	uint8_t ibutSerialNumber2[IBUTNUMLENGTH];
	uint8_t ibutSerialNumber3[IBUTNUMLENGTH];
	uint8_t ibutSerialNumber4[IBUTNUMLENGTH];
	uint8_t ibutSerialNumber5[IBUTNUMLENGTH];
	uint8_t ibutSerialNumber6[IBUTNUMLENGTH];
	uint8_t ibutSerialNumber7[IBUTNUMLENGTH];
	uint8_t ibutSerialNumber8[IBUTNUMLENGTH];
	uint8_t ibutSerialNumber9[IBUTNUMLENGTH];
	uint8_t ibutSerialNumber10[IBUTNUMLENGTH];
	uint8_t ibutName1[IBUTNUMLENGTH];
	uint8_t ibutName2[IBUTNUMLENGTH];
	uint8_t ibutName3[IBUTNUMLENGTH];
	uint8_t ibutName4[IBUTNUMLENGTH];
	uint8_t ibutName5[IBUTNUMLENGTH];
	uint8_t ibutName6[IBUTNUMLENGTH];
	uint8_t ibutName7[IBUTNUMLENGTH];
	uint8_t ibutName8[IBUTNUMLENGTH];
	uint8_t ibutName9[IBUTNUMLENGTH];
	uint8_t ibutName10[IBUTNUMLENGTH];
	uint8_t radioSerialNumber1[IBUTNUMLENGTH];
	uint8_t radioSerialNumber2[IBUTNUMLENGTH];
	uint8_t radioSerialNumber3[IBUTNUMLENGTH];
	uint8_t radioSerialNumber4[IBUTNUMLENGTH];
	uint8_t radioSerialNumber5[IBUTNUMLENGTH];
	uint8_t radioSerialNumber6[IBUTNUMLENGTH];
	uint8_t radioSerialNumber7[IBUTNUMLENGTH];
	uint8_t radioSerialNumber8[IBUTNUMLENGTH];
	uint8_t radioSerialNumber9[IBUTNUMLENGTH];
	uint8_t radioSerialNumber10[IBUTNUMLENGTH];
	uint8_t radioName1[IBUTNUMLENGTH];
	uint8_t radioName2[IBUTNUMLENGTH];
	uint8_t radioName3[IBUTNUMLENGTH];
	uint8_t radioName4[IBUTNUMLENGTH];
	uint8_t radioName5[IBUTNUMLENGTH];
	uint8_t radioName6[IBUTNUMLENGTH];
	uint8_t radioName7[IBUTNUMLENGTH];
	uint8_t radioName8[IBUTNUMLENGTH];
	uint8_t radioName9[IBUTNUMLENGTH];
	uint8_t radioName10[IBUTNUMLENGTH];

	// Вкладка "ПЦО"
	uint8_t ContactIDTransmit;
	uint8_t TransmitTestTime;
	uint8_t AlarmOperatorPhone1[TELEPHONELENGTH];
	uint8_t AlarmOperatorPhone2[TELEPHONELENGTH];
}tmStruct;

void BuzzerBuzzAndPause(uint16_t msBuzzTime, uint16_t msPauseTime);

#endif