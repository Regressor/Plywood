/* ������: ������� */
/* ������: v0.2    */

#include <asf.h>

void default_vars_settings(void);

/* �������������� �������� �� ��������� */
void default_vars_settings(void) {
	FanManualSwitch = false;
	ActiveMode = MODE_NONE;
	PWMActive = false;
	ACActive = false;
	
	ENAmperageSet = HallZero - DEFAULT_AMPERAGE * 2;
	EPAmperageSet = HallZero + DEFAULT_AMPERAGE * 2;

	ACPeriodCounter = 10000 / DEFAULT_AC_FREQUENCY;
	ACEPCounter = ACPeriodCounter * DEFAULT_AC_BALANCE / 100;
	
	// ������ ����� (�� ������ ������)
	for (int i = 0; i < MAX_COMMAND_LEN; i++) CmdBuffer[i] = 0;
}


/* ����� ����� � ��������� */
int main (void)
{	
	board_init();
	default_vars_settings();
	ioport_set_pin_level(ERROR_LED, true);
	
	/* ���� 4 ������� � �������� �������� ���� */
	/* ����� ���� �������� �������� �� ������  */
	delay_ms(4000);
	ioport_set_pin_level(ERROR_LED, false);
	ioport_set_pin_level(CHRG_SWITCH, true);
	
	/* �������� ������ � ������� ������� ������ */
	CommandParser = main_parser;
 
	/* �������� ������� ���� ��������� */
	while (1) {
		CommandParser();
	}
}
