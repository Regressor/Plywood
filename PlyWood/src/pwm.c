#include <asf.h>

/* �������� ������ ��� */
NOINLINE void pwm_start(void) {
	/* �������� �� ������ ������ ����������� */
	if (ioport_get_pin_level(OVRTMP_SENS)) {
		return;
	}

	/* �������� ������� �� ��� */
	unsigned char temp = PCTL1;
	temp &= (1 << PRUN1);
	PCTL1 = temp;
	if (temp == 0) {
		PWMSoftStart = true;
		
		/* ������� ������������ ������������ ����� */
		PWMDownStart = PWM_MAX_DOWN_COUNTER;
		OCR1SA = PWMDownStart;
		OCR1SB = PWMDownStart;
		
		/* ������� ������������ ������� */
		OCR1RA = PWM_HPERIOD_COUNTER;
		OCR1RB = PWM_HPERIOD_COUNTER;
		
		/* �������� ��� */
		PCTL1 |= (1 << PRUN1);
	}
}


/* �������� ��������� ��� */
NOINLINE void pwm_shutdown(void) {
	ACActive = false;
	
	unsigned char temp = PCTL1;
	temp &= (1 << PRUN1);
	PCTL1 = temp;

	if (temp != 0) {
		PWMSoftStart = false;
		PCTL1 &= (~(1 << PRUN1));
	}
}


/* ��������� ��������� ��� */
inline void emerg_pwm_shutdown(void) {
	pwm_shutdown();
	
	// ��������� ������ ������, ���������� ��������
	ioport_set_pin_level(ERROR_LED, true);
	fan_switch(true);
}

