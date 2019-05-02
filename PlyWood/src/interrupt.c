/* Обработчики прерываний */
#include <asf.h>

void processACSwitchTask(void);

/* Прерывание компаратора - сработка защиты, ШИМ останавливается аппаратно */
ISR(ANALOG_COMP_1_vect) {
	// Зажжем светодиод ошибки
	ioport_set_pin_level(ERROR_LED, true);
	
	// Приведем в порядок переменные
	ACActive = false;
	PWMActive = false;
	
	// Флаг сработки по превышению тока (снимается только сбросом контроллера)
	OvercurrentActive = true;
}


int last_error = 0;
int D = 0;
int I = 0;

/* Обработчик прерывания АЦП - результат измерения готов */
ISR(ADC_vect) {
	// Выключаем противоположное заданному плечо AC коммутатора
	if (PartPlusActive)
		ioport_set_pin_level(ACM_DRV, false);
	else
		ioport_set_pin_level(ACP_DRV, false);
	
	// Читаем текущее значение ADC
	RecentADC = ADC;

	// Проверка на перегрев (остановка по перегреву)
	if (is_overheat()) emerg_pwm_shutdown();

	// Считаем пропорциональную компоненту
	if (RecentADC < HallZero) {
		// Отрицательный ток (очистка)
		PropFactor = RecentADC - ENAmperage;
	} else {
		// Положительный ток (плавление)
		PropFactor = EPAmperage - RecentADC;
	}
	
	// Включаем заданное плечо AC коммутатора (дедтайм обеспечивается 
	// командами между выключением и включением)
	if (PartPlusActive) {
		ioport_set_pin_level(ACP_DRV, true);
	} else {
		ioport_set_pin_level(ACM_DRV, true);
	}

	// Надо ли включить/выключить вентилятор по нагреву ?
	fan_switch(is_fan_needed());

	// Считаем PID и вычисляем паузу (пауза задается в OCR1SA, OCR1SB)
	// Переменная PidControl это ПИД-регулятор (задает время импульса в ШИМ)
	DiffFactor = PropFactor - last_error;
	last_error = PropFactor;
	Pid += PropFactor / 2 + DiffFactor / 8;

	// Лимиты значений пид регулятора
	if (Pid > PWM_MAX_DOWN_COUNTER)
		Pid = PWM_MAX_DOWN_COUNTER;

	if (Pid < PWM_MIN_DOWN_COUNTER)
		Pid = PWM_MIN_DOWN_COUNTER;

	// Поднимаем длительность импульса
	if (PWMActive) {
		if (PWMDownStart > PWM_MIN_DOWN_COUNTER) {
			PWMDownStart--;
			
			if (Pid < PWMDownStart) {
				Pid = PWMDownStart;
			}			
		} else {
			PWMActive = false;
		}
	}
	
	// Загружаем счетчики паузы ШИМа
	OCR1SA = Pid;
	OCR1SB = Pid;
	
	// Загружаем фиксированное значение, чтобы запустить autolock
	OCR1RB = PWM_HPERIOD_COUNTER;
}


/* Управление переключением AC коммутатора, вызывается 10000 раз в секунду */
void processACSwitchTask(void) {
	
	// Проверим надо ли вообще что-то делать...
	if (ActiveMode == MODE_NONE) return;
	
	// Если это постоянка - инициализируем выбранную полярность
	if (ActiveMode != MODE_ACTIG) {
		ENAmperageSet = TargetAmpEN;
		EPAmperageSet = TargetAmpENR;
		EPAmperage = EPAmperageSet;
		ENAmperage = ENAmperageSet;
		PartPlusActive = !DCRPolarity;
		return;
	}

	// Если установлен, но еще не активирован режим 
	// переменного тока - инициализируем переменные
	if (!ACActive) {
		PartPlusActive = false;
		ENAmperageSet = TargetAmpEN;
		EPAmperageSet = TargetAmpEP;
		EPAmperage = EPAmperageSet;
		ENAmperage = ENAmperageSet;
		ACTimeCounter = 0;
		return;
	}
	
	// Счетчик текущего времени в периоде
	ACTimeCounter++;
	
	// Определяем форму тока при переключении и выставляем флаг переключения
	if (ACTimeCounter <= ACEPCounter) {
		// За 8 тиков до переключения начинаем снижать ток (1 тик - 100мкс)
		if ((EPAmperage < SwitchEPAmperage) && 
			((ACTimeCounter + 8) >= ACEPCounter)) {
			EPAmperage = SwitchEPAmperage;
		} else {
			EPAmperage = EPAmperageSet;
		}
		
		// Если подошло время переключения - ждем пока ток не 
		// снизится до заданной величины и переключаем
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
		// За 8 тиков до переключения начинаем снижать ток (1 тик - 100мкс)
		if ((ENAmperage > SwitchENAmperage) && 
			((ACTimeCounter) + 8 >= ACPeriodCounter)) {
			ENAmperage = SwitchENAmperage;
		} else {
			ENAmperage = ENAmperageSet;
		}
		
		// Если подошло время переключения - ждем пока ток не
		// снизится до заданной величины и переключаем
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
	
	// Проверим на конец периода переменого тока
	if (ACTimeCounter >= ACPeriodCounter) {
		ACTimeCounter = 0;
	}
}


/* Прерывание по сработке таймера */
ISR(TIMER0_COMPB_vect) {
	callCounter++;
	processACSwitchTask();
}

