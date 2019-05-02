/* Глобальные переменные проекта */ 
#include <asf.h>

bool fanState = false;					// Состояние вентилятора
bool OvercurrentActive = false;			// Флаг сработки защиты по току

unsigned int UARTBaudRate = 0;			// Скорость UART интерфейса
unsigned int SPISettings = 0;			// Настройки SPI интерфейса
int HallZero = 0;						// Нулевая точка hall-сенсора

int RecentADC = 0;						// Текущий результат измерения тока
int SwitchEPAmperage = 0;				// Ток очистки перед переключением
int SwitchENAmperage = 0;				// Ток сварки перед переключением

unsigned int ACTimeCounter = 0;			// Счетчик времени AC коммутатора
unsigned int ACPeriodCounter = 0;		// Период переменного тока в 1/10000с циклах
unsigned int ACEPCounter = 0;			// Счетчик времени EN включения
unsigned int ACBalance = 0;				// Баланс АС

unsigned int PWMDownStart = 0;			// Расчетный счетчик длительности паузы PSC10
unsigned int PWMDownFinal = 0;			// Расчетный счетчик длительности паузы PSC11

int ENAmperage = 0;						// Ток очистки
int EPAmperage = 0;						// Ток сварки

int TargetAmpEN = 0;					// Уставка тока для отрицательного включения
int TargetAmpENR = 0;					// Уставка тока для отрицательного включения (реверс на DC)
int TargetAmpEP = 0;					// Уставка тока для положительного включения

int EPAmperageSet = 0;					// Задание тока положительного плеча (исп. в прерывании)
int ENAmperageSet = 0;					// Задание тока отрицательного плеча (исп. в прерывании)

int PropFactor = 0;					// Пропорциональный коэффициент PID регулятора
int DiffFactor = 0;					// Дифференциальный коэффициент PID регулятора
int Pid = 0;						// Значение PID регулятора

unsigned char ActiveMode = MODE_DCTIG;		// Текущий режим работы (AC/DC)

bool PWMActive = false;					// true - ШИМ включен, false - ШИМ выключен
bool ACActive = false;					// true - переменный ток, false - постоянный ток
bool DCRPolarity = false;				// Полярность постоянного тока (true - EN, false - EP)
bool PartPlusActive = true;				// true - включен плюс на детали
bool FanManualSwitch = false;			// Флаг ручного включения вентилятора (true - включить)

void (*CommandParser)(void);			// Адрес активного парсера команд (вызов через icall)

unsigned int callCounter = 0;			// TODO: для отладки, убрать в продакшене

char CmdBuffer[MAX_COMMAND_LEN + 4];	// Буфер для команд (+4 для безопасности)
char OutBuffer[MAX_COMMAND_LEN + 4];	// Буфер для ответов (+4 для безопасности)
