#ifndef PWM_H_
#define PWM_H_

/* ��������� ��� ��� ���������� */
#define PWM_HPERIOD_COUNTER (BOARD_PLL_HZ/PWM_FREQUENCY/2+1)					// ������� ����������� ��� ��� DC-DC ����������
#define PWM_MIN_DOWN_COUNTER ((PWM_DEADTIME*(BOARD_PLL_HZ/1000000))/1000+1)		// ������� ����������� ����� ��� DC-DC ����������
#define PWM_MIN_UP_COUNTER (PWM_MINPULSE*(BOARD_PLL_HZ/1000000)/1000+1)			// ������� ������������ �������� ��� DC-DC ����������
#define PWM_MAX_UP_COUNTER (PWM_HPERIOD_COUNTER - PWM_MIN_DOWN_COUNTER)			// ������� ������������� �������� ���
#define PWM_MAX_DOWN_COUNTER (PWM_HPERIOD_COUNTER - PWM_MIN_UP_COUNTER)			// ������� ������������ ����� ���

/* ��������� */
extern void pwm_start(void);
extern void pwm_shutdown(void);
extern void emerg_pwm_shutdown(void);

#endif /* PWM_H_ */
