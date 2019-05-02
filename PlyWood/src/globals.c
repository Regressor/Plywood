/* ���������� ���������� ������� */ 
#include <asf.h>

bool fanState = false;					// ��������� �����������
bool OvercurrentActive = false;			// ���� �������� ������ �� ����

unsigned int UARTBaudRate = 0;			// �������� UART ����������
unsigned int SPISettings = 0;			// ��������� SPI ����������
int HallZero = 0;						// ������� ����� hall-�������

int RecentADC = 0;						// ������� ��������� ��������� ����
int SwitchEPAmperage = 0;				// ��� ������� ����� �������������
int SwitchENAmperage = 0;				// ��� ������ ����� �������������

unsigned int ACTimeCounter = 0;			// ������� ������� AC �����������
unsigned int ACPeriodCounter = 0;		// ������ ����������� ���� � 1/10000� ������
unsigned int ACEPCounter = 0;			// ������� ������� EN ���������
unsigned int ACBalance = 0;				// ������ ��

unsigned int PWMDownStart = 0;			// ��������� ������� ������������ ����� PSC10
unsigned int PWMDownFinal = 0;			// ��������� ������� ������������ ����� PSC11

int ENAmperage = 0;						// ��� �������
int EPAmperage = 0;						// ��� ������

int TargetAmpEN = 0;					// ������� ���� ��� �������������� ���������
int TargetAmpENR = 0;					// ������� ���� ��� �������������� ��������� (������ �� DC)
int TargetAmpEP = 0;					// ������� ���� ��� �������������� ���������

int EPAmperageSet = 0;					// ������� ���� �������������� ����� (���. � ����������)
int ENAmperageSet = 0;					// ������� ���� �������������� ����� (���. � ����������)

int PropFactor = 0;					// ���������������� ����������� PID ����������
int DiffFactor = 0;					// ���������������� ����������� PID ����������
int Pid = 0;						// �������� PID ����������

unsigned char ActiveMode = MODE_DCTIG;		// ������� ����� ������ (AC/DC)

bool PWMActive = false;					// true - ��� �������, false - ��� ��������
bool ACActive = false;					// true - ���������� ���, false - ���������� ���
bool DCRPolarity = false;				// ���������� ����������� ���� (true - EN, false - EP)
bool PartPlusActive = true;				// true - ������� ���� �� ������
bool FanManualSwitch = false;			// ���� ������� ��������� ����������� (true - ��������)

void (*CommandParser)(void);			// ����� ��������� ������� ������ (����� ����� icall)

unsigned int callCounter = 0;			// TODO: ��� �������, ������ � ����������

char CmdBuffer[MAX_COMMAND_LEN + 4];	// ����� ��� ������ (+4 ��� ������������)
char OutBuffer[MAX_COMMAND_LEN + 4];	// ����� ��� ������� (+4 ��� ������������)
