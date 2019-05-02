/* ����������� ���������� */
#include <asf.h>

void processACSwitchTask(void);

/* ���������� ����������� - �������� ������, ��� ��������������� ��������� */
ISR(ANALOG_COMP_1_vect) {
	// ������ ��������� ������
	ioport_set_pin_level(ERROR_LED, true);
	
	// �������� � ������� ����������
	ACActive = false;
	PWMActive = false;
	
	// ���� �������� �� ���������� ���� (��������� ������ ������� �����������)
	OvercurrentActive = true;
}


/* ���������� ���������� ��� - ��������� ��������� ����� */
ISR(ADC_vect) {
	
	// ��������� ��������������� ��������� ����� AC �����������
	if (PartPlusActive)
		ioport_set_pin_level(ACM_DRV, false);
	else
		ioport_set_pin_level(ACP_DRV, false);
	
	// ������ ������� �������� ADC
	RecentADC = ADC;

	// �������� �� �������� (��������� �� ���������)
	if (is_overheat()) emerg_pwm_shutdown();

	// ������� ���������������� ����������
	if (ioport_get_pin_level(ACP_DRV))
		PropFactor = ENAmperage - RecentADC;
	else
		PropFactor = RecentADC - EPAmperage;

	// �������� �������� ����� AC ����������� (������� �������������� 
	// ��������� ����� ����������� � ����������)
	if (PartPlusActive) {
		ioport_set_pin_level(ACP_DRV, true);
	} else {
		ioport_set_pin_level(ACM_DRV, true);
	}

	// ���� �� ��������/��������� ���������� �� ������� ?
	fan_switch(is_fan_needed());
	
	// ������� PID � ��������� ����� (����� �������� � OCR1SA, OCR1SB)
	// ���������� PidControl ��� ���-��������� (������ ����� �������� � ���)
	PidControl = (PropFactor / 2 + IntgFactor) - DiffFactor / 4;
	
	// ������������ pid ��������� �������� �������� �������
	if (PidControl > PWM_MAX_UP_COUNTER) {
		PidControl = PWM_MAX_UP_COUNTER;
		PWMDownFinal = PWM_MIN_DOWN_COUNTER;
		IntgFactor = PWM_MAX_UP_COUNTER;
	} else {
		if (PidControl >= PWM_MIN_UP_COUNTER) {
			IntgFactor = PidControl;
			PWMDownFinal = PWM_HPERIOD_COUNTER - PidControl;
		} else {
			// ������� �� ����� ���� ������ �������� � ������� ��������
			PidControl = PWM_MIN_UP_COUNTER;
			PWMDownFinal = PWM_MAX_DOWN_COUNTER;
			IntgFactor = PWM_MIN_UP_COUNTER;
		}
	}

	DiffFactor = PropFactor;
	
	// ��������� ������������ ��������
	if (PWMActive) {
		if (PWMDownStart > PWM_MIN_DOWN_COUNTER) {
			PWMDownStart--;
			
			if (PWMDownFinal < PWMDownStart) {
				PWMDownFinal = PWMDownStart;
			}			
		} else {
			PWMActive = false;
		}
	}
	
	// ��������� �������� �����
	OCR1SA = PWMDownFinal;
	OCR1SB = PWMDownFinal;
	
	// ������������� ��������, ����� ��������� autolock
	OCR1RB = PWM_HPERIOD_COUNTER;
}


/* ���������� ������������� AC �����������, ���������� 10000 ��� � ������� */
void processACSwitchTask(void) {
	
	// �������� ���� �� ������ ���-�� ������...
	if (ActiveMode == MODE_NONE) return;
	
	// ���� ��� ��������� - �������������� ��������� ����������
	if (ActiveMode != MODE_ACTIG) {
		ENAmperageSet = TargetAmpEN;
		EPAmperageSet = TargetAmpENR;
		EPAmperage = EPAmperageSet;
		ENAmperage = ENAmperageSet;
		PartPlusActive = !DCRPolarity;
		return;
	}

	// ���� ����������, �� ��� �� ����������� ����� 
	// ����������� ���� - �������������� ����������
	if (!ACActive) {
		PartPlusActive = false;
		ENAmperageSet = TargetAmpEN;
		EPAmperageSet = TargetAmpEP;
		EPAmperage = EPAmperageSet;
		ENAmperage = ENAmperageSet;
		ACTimeCounter = 0;
		return;
	}
	
	// ������� �������� ������� � �������
	ACTimeCounter++;
	
	// ���������� ����� ���� ��� ������������ � ���������� ���� ������������
	if (ACTimeCounter <= ACEPCounter) {
		// �� 8 ����� �� ������������ �������� ������� ��� (1 ��� - 100���)
		if ((EPAmperage < SwitchEPAmperage) && 
			((ACTimeCounter + 8) >= ACEPCounter)) {
			EPAmperage = SwitchEPAmperage;
		} else {
			EPAmperage = EPAmperageSet;
		}
		
		// ���� ������� ����� ������������ - ���� ���� ��� �� 
		// �������� �� �������� �������� � �����������
		if (ACTimeCounter < ACEPCounter) {
			return;
		} else {
			if (RecentADC < SwitchEPAmperage) {
				ACTimeCounter--;
			} else {
				PartPlusActive = true;
			}
		}
	} else {
		// �� 8 ����� �� ������������ �������� ������� ��� (1 ��� - 100���)
		if ((ENAmperage > SwitchENAmperage) && 
			((ACTimeCounter) + 8 >= ACPeriodCounter)) {
			ENAmperage = SwitchENAmperage;
		} else {
			ENAmperage = ENAmperageSet;
		}
		
		// ���� ������� ����� ������������ - ���� ���� ��� ��
		// �������� �� �������� �������� � �����������
		if (ACTimeCounter < ACPeriodCounter) {
			return;
		} else {
			if (RecentADC > SwitchENAmperage) {
				ACTimeCounter--;
			} else {
				PartPlusActive = false;
			}
		}
	}
	
	// �������� �� ����� ������� ���������� ����
	if (ACTimeCounter >= ACPeriodCounter) {
		ACTimeCounter = 0;
	}
}


/* ���������� �� �������� ������� */
ISR(TIMER0_COMPB_vect) {
	callCounter++; // TODO: ������ � ���������� (���������, ����� ������� �� ������ ���-�� �������� � �������)
	processACSwitchTask();
}

