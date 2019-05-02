#include <asf.h>

/* Плановый запуск ШИМ */
NOINLINE void pwm_start(void) {
	/* Проверим на всякий случай термодатчик */
	if (ioport_get_pin_level(OVRTMP_SENS)) {
		return;
	}

	/* Проверим включен ли ШИМ */
	unsigned char temp = PCTL1;
	temp &= (1 << PRUN1);
	PCTL1 = temp;
	if (temp == 0) {
		PWMSoftStart = true;
		
		/* Зададим максимальную длительность паузы */
		PWMDownStart = PWM_MAX_DOWN_COUNTER;
		OCR1SA = PWMDownStart;
		OCR1SB = PWMDownStart;
		
		/* Зададим длительность периода */
		OCR1RA = PWM_HPERIOD_COUNTER;
		OCR1RB = PWM_HPERIOD_COUNTER;
		
		/* Запустим ШИМ */
		PCTL1 |= (1 << PRUN1);
	}
}


/* Плановая остановка ШИМ */
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


/* Аварийная остановка ШИМ */
inline void emerg_pwm_shutdown(void) {
	pwm_shutdown();
	
	// Светодиод ошибки зажечь, вентилятор включить
	ioport_set_pin_level(ERROR_LED, true);
	fan_switch(true);
}

