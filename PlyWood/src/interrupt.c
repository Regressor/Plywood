/* Обработчики прерываний */
#include <asf.h>

void processACSwitchTask(void);

/* Прерывание компаратора - сработка защиты, ШИМ останавливается аппаратно */
ISR(ANALOG_COMP_1_vect) {
	/* Зажжем светодиод ошибки */
	ioport_set_pin_level(ERROR_LED, true);
	
	/* Приведем в порядок переменные */
	ACActive = false;
	PWMSoftStart = false;
	
	/* Флаг сработки по превышению тока (снимается только сбросом контроллера) */
	OvercurrentActive = true;
}


/* Обработчик прерывания АЦП - результат измерения готов */
ISR(ADC_vect) {
	/* Выключаем противоположное заданному плечо AC коммутатора */
	if (PartPlusActive)
		ioport_set_pin_level(ACM_DRV, false);
	else
		ioport_set_pin_level(ACP_DRV, false);
	
	/* Читаем текущее значение ADC */
	RecentADC = ADC;

	/* Проверка на перегрев (остановка по перегреву) */
	if (is_overheat()) emerg_pwm_shutdown();

	/* Считаем пропорциональную компоненту */
	if (RecentADC < HallZero) {
		/* Отрицательный ток (очистка) */
		PropFactor = EPAmperage - RecentADC;
	} else {
		/* Положительный ток (плавление) */
		PropFactor = RecentADC - ENAmperage;
	}

	/* Включаем заданное плечо AC коммутатора (дедтайм полумоста 
	   обеспечивается командами между его выключением и включением) */
	if (PartPlusActive) {
		ioport_set_pin_level(ACP_DRV, true);
	} else {
		ioport_set_pin_level(ACM_DRV, true);
	}

	/* Надо ли включить/выключить вентилятор по нагреву ? */
	fan_switch(is_fan_needed());

	/* Период ШИМ у нас статичен. Мы меняем только время паузы. 
	   Чем оно больше, тем меньше ширина импульса.
	   Считаем PID и вычисляем паузу (пауза задается в OCR1SA, OCR1SB)
	   Переменная Pid это ПИД-регулятор (задает время паузы в ШИМ) */
	DiffFactor = PropFactor - LastError;
	LastError = PropFactor;
	Pid += PropFactor / 2 + DiffFactor / 8;

	/* Нельзя давать ширину импульса меньше заданной, так как 
	   драйвер транзистора моста может частично импульс поглотить 
	   и выдать амплитуду, недостаточную для полного открытия 
	   транзистора, что приведет к линейному режиму его работы и 
	   возможно выходу из строя */
	if (Pid > PWM_MAX_DOWN_COUNTER)
		Pid = PWM_MAX_DOWN_COUNTER; 

	/* Максимальная ширина импульса зависит от длины периода и 
	   заданного времени deadtime. Превышать ее также нельзя 
	   так как это приведет к маленькому или нулевому deatime,
	   сквозному току и выходу из строя транзисторов моста */
	if (Pid < PWM_MIN_DOWN_COUNTER)
		Pid = PWM_MIN_DOWN_COUNTER;

	/* На старте плавно поднимаем длительность импульса */
	if (PWMSoftStart) {
		if (PWMDownStart > PWM_MIN_DOWN_COUNTER) {
			PWMDownStart--;

			if (Pid < PWMDownStart) {
				Pid = PWMDownStart;
			}			
		} else {
			PWMSoftStart = false;
		}
	}
	
	/* Загружаем регистры-счетчики паузы ШИМа */
	OCR1SA = Pid;
	OCR1SB = Pid;
	
	/* Загружаем фиксированное значение, чтобы запустить autolock. 
	   Конфигурация AUTOLOCK выбирается при первоначальной настройке ШИМ и 
	   служит для симметрирования моста - изменения применяются только 
	   после отработки полного цикла ШИМ */
	OCR1RB = PWM_HPERIOD_COUNTER;
}


/* Управление переключением AC коммутатора, вызывается 10000 раз в секунду */
void processACSwitchTask(void) {
	
	/* Проверим надо ли вообще что-то делать... */
	if (ActiveMode == MODE_NONE) return;
	
	/* Если это постоянка - инициализируем выбранную полярность */
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
	
	/* Счетчик текущего времени в периоде */
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

