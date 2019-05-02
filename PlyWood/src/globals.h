#ifndef GLOBALS_H_
#define GLOBALS_H_

#define MODE_NONE	0
#define MODE_MMA	1
#define MODE_DCTIG	2
#define MODE_ACTIG	3

extern bool fanState;
extern bool OvercurrentActive;

extern unsigned int UARTBaudRate;
extern unsigned int SPISettings;

extern int HallZero;
extern int RecentADC;
extern int SwitchEPAmperage;
extern int SwitchENAmperage;

extern unsigned int ACTimeCounter;
extern unsigned int ACPeriodCounter;
extern unsigned int ACEPCounter;
extern unsigned int ACBalance;

extern unsigned int PWMDownStart;
extern unsigned int PWMDownFinal;

extern int ENAmperage;
extern int EPAmperage;

extern int TargetAmpEN;
extern int TargetAmpEP;
extern int TargetAmpENR;

extern int EPAmperageSet;
extern int ENAmperageSet;

extern int PropFactor;
extern int DiffFactor;
extern int LastError;
extern int Pid;

extern unsigned char ActiveMode;

extern bool PWMSoftStart;
extern bool ACActive;
extern bool DCRPolarity;
extern bool PartPlusActive;
extern bool FanManualSwitch;

extern void (*CommandParser)(void);

extern unsigned int callCounter;

extern char CmdBuffer[MAX_COMMAND_LEN + 4];
extern char OutBuffer[MAX_COMMAND_LEN + 4]; 

#endif /* GLOBALS_H_ */