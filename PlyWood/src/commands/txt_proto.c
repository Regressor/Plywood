/* Текстовый протокол авторства sam_soft. Добавлен для       */
/* возможности управления источником текстовыми командами из */
/* обычной терминальной программы, а также для совместимости */
/* с уже имеющимися программами и управляющими панелями      */
/* к функционалу sam_soft добавлено включение реверса в mma  */

#include <asf.h>
#ifndef BINARY_PROTOCOL

/* Проверка на ошибки перед обработкой команды */
bool is_psu_in_error_state(void) {
	if (OvercurrentActive) {
		ioport_set_pin_level(ERROR_LED, true);
		serial_send_answer(OVERCURR_CMD);
		return true;
	}
	
	if (ioport_get_pin_level(OVRTMP_SENS)) {
		ioport_set_pin_level(ERROR_LED, true);
		serial_send_answer(OVERTEMP_CMD);
		return true;
	}
	
	ioport_set_pin_level(ERROR_LED, false);
	return false;
}


/* Прием и обработка команд пользователя. Главная секция. */
void main_parser(void) {
	
	/* В главной секции все отключено, шима нет, нагрузки нет */
	ActiveMode = MODE_NONE;
	ioport_set_pin_level(ACP_DRV, false);
	ioport_set_pin_level(ACM_DRV, false);

	/* Считываем команду пользователя */
	serial_receive_line();
	if (is_psu_in_error_state()) return;

	switch(CmdBuffer[0]) {
		case 'm': // Переходим в секцию mma
			ActiveMode = MODE_MMA;
			CommandParser = cmd_mma_section;
			serial_send_answer(OK_CMD);
			break;
		case 'd': // Переходим в секцию dc_tig
			ActiveMode = MODE_DCTIG;
			CommandParser = cmd_dctig_section;
			serial_send_answer(OK_CMD);
			break;
		case 'a': // Переходим в секцию ac_tig
			ActiveMode = MODE_ACTIG;
			ACActive = false;
			CommandParser = cmd_actig_section;
			serial_send_answer(OK_CMD);
			break;
		case 'c': // Переходим в секцию калибровки
			ActiveMode = MODE_MMA;
			CommandParser = cmd_cal_section;
			serial_send_answer(OK_CMD);
			break;
		case 'v': // Отправляем версию прошивки
			serial_send_answer(FIRMWARE_VERSION);
			break;
		default:
			serial_send_answer(INVALID_CMD);
			break;
	}
}


/* Прием и обработка команд пользователя. Секция режима MMA */
void cmd_mma_section(void) {
	serial_receive_line();
	if (is_psu_in_error_state()) return;
	
	switch(CmdBuffer[0]) {
		case 's': // Запуск шим
			pwm_start();
			serial_send_log();
			break;
			
		case 'e': // Остановка шим
			pwm_shutdown();
			serial_send_answer(OK_CMD);
			break;
			
		case 'q': // Выход в главную секцию
			pwm_shutdown();
			CommandParser = main_parser;
			serial_send_answer(OK_CMD);
			break;
			
		case 'l': // Вывод данных состояния
			serial_send_log();
			break;

		case 'd': // Вывод дампа переменных
			serial_send_dump();
			break;
			
		case 'F': // Включение/выключение охлаждения
			switch (CmdBuffer[1]) {
				case '1': FanManualSwitch = true; break;
				case '0': FanManualSwitch = false; break;
				default: 
					serial_send_answer(INVALID_CMD);
					return;
			}
			fan_switch(true);
			serial_send_answer(OK_CMD);
			break;
			
		case 'a': // Задание тока
			if (!is_num_str(CmdBuffer + 1))
				serial_send_answer(INVALID_CMD);
				
			int enteredAmp = atoi(CmdBuffer + 1);
			if (enteredAmp < MMA_MIN_AMPERAGE || enteredAmp > MMA_MAX_AMPERAGE) {
				serial_send_answer(INVALID_CMD);
				return;
			}
			
			TargetAmpEN = HallZero + (enteredAmp * 2);
			TargetAmpENR = HallZero - (enteredAmp * 2);
			serial_send_answer(OK_CMD);
			break;
			
		case 'p': // Изменение полярности (реверс)
			switch (CmdBuffer[1]) {
				case '1': DCRPolarity = true; break;
				case '0': DCRPolarity = false; break;
				default:
					serial_send_answer(INVALID_CMD);
					return;
			}
			serial_send_answer(OK_CMD);
			break;
			
		default: // Ошибка - неизвестная команда
			serial_send_answer(INVALID_CMD);
			break;
	}
}


/* Прием и обработка команд пользователя. Секция режима DC TIG. */
void cmd_dctig_section(void) {
	serial_receive_line();
	if (is_psu_in_error_state()) return;

	switch(CmdBuffer[0]) {
		case 'F': // Включение/выключение охлаждения
			switch (CmdBuffer[1]) {
				case '1': FanManualSwitch = true; break;
				case '0': FanManualSwitch = false; break;
				default: 
					serial_send_answer(INVALID_CMD);
					return;
			}

			fan_switch(is_fan_needed());
			serial_send_answer(OK_CMD);
			break;

		case 'g': // Включение газового клапана
			ioport_set_pin_level(GAS_SWITCH, true);
			serial_send_answer(OK_CMD);
			break;
			
		case 'r': // Выключение газового клапана
			ioport_set_pin_level(GAS_SWITCH, false);
			serial_send_answer(OK_CMD);
			break;
			
		case 'a': // Задание тока
			if (!is_num_str(CmdBuffer + 1))
			serial_send_answer(INVALID_CMD);
			
			int enteredAmp = atoi(CmdBuffer + 1);
			if (enteredAmp < DCTIG_MIN_AMPERAGE || enteredAmp > DCTIG_MAX_AMPERAGE) {
				serial_send_answer(INVALID_CMD);
				return;
			}
			
			TargetAmpEN = HallZero + (enteredAmp * 2);
			TargetAmpENR = HallZero;
			serial_send_answer(OK_CMD);
			break;
			
		case 's': // Запуск шим
			pwm_start();
			serial_send_log();
			break;
			
		case 'e': // Остановка шим
			pwm_shutdown();
			serial_send_answer(OK_CMD);
			break;
					
		case 'l': // Вывод данных состояния
			serial_send_log();
			break;

		case 'd': // Вывод дампа переменных
			serial_send_dump();
			break;
					
		case 'q': // Выход в главную секцию
			pwm_shutdown();
			CommandParser = main_parser;
			serial_send_answer(OK_CMD);
			break;		
			
		default: // Ошибка - неизвестная команда
			serial_send_answer(INVALID_CMD);
			break;
	}
}


/* Прием и обработка команд пользователя. Секция режима калибровки */
void cmd_cal_section(void) {
		char *outNext = OutBuffer;
	serial_receive_line();
	if (is_psu_in_error_state()) return;
	
	switch(CmdBuffer[0]) {
		case 'F': // Включение/выключение охлаждения
			switch (CmdBuffer[1]) {
				case '1': FanManualSwitch = true; break;
				case '0': FanManualSwitch = false; break;
				default: 
					serial_send_answer(INVALID_CMD);
					return;
			}
			fan_switch(true);
			serial_send_answer(OK_CMD);
			break;
			
		case 'a': // Задание тока
			if (!is_num_str(CmdBuffer + 1))
				serial_send_answer(INVALID_CMD);
		
			int enteredAmp = atoi(CmdBuffer + 1);
			if (enteredAmp < CAL_MIN_AMPERAGE || enteredAmp > CAL_MAX_AMPERAGE) {
				serial_send_answer(INVALID_CMD);
				return;
			}
		
			TargetAmpEN = HallZero + (enteredAmp * 2);
			TargetAmpENR = HallZero - (enteredAmp * 2);;
			serial_send_answer(OK_CMD);
			break;
		
		case 'p': // Изменение полярности (реверс)
			switch (CmdBuffer[1]) {
				case '1': DCRPolarity = true; break;
				case '0': DCRPolarity = false; break;
				default: 
					serial_send_answer(INVALID_CMD);
					return;
			}
			serial_send_answer(OK_CMD);
			break;
		
		case 's': // Запуск шим
			pwm_start();
			serial_send_log();
			break;
		
		case 'e': // Остановка шим
			pwm_shutdown();
			serial_send_answer(OK_CMD);
			break;
		
		case 'l': // Вывод данных состояния
			serial_send_log();
			break;

		case 'd': // Вывод дампа переменных
			serial_send_dump();
			break;		
		
		case 'z': // Вывод текущей нулевой точки датчика Холла
			itoa(HallZero, outNext, 10);
			while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);

			// Дополняем до 15 байт (16тый - перенос строки)
			int len = outNext - OutBuffer;
			if (len < 15) {
				memset(outNext, ' ', 15 - len);
			}

			OutBuffer[15] = 0xd; // Перенос в конце строки
			OutBuffer[16] = 0x0; // Замыкающий ноль в конце строки	
			
			serial_send_answer(OutBuffer);
			break;
		
		case 'Z': // Установка нулевой точки датчика Холла
			if (!is_num_str(CmdBuffer + 1))
				serial_send_answer(INVALID_CMD);
				
			int enteredZero = atoi(CmdBuffer + 1);
			if (enteredZero > HALLZERO_MIN_VALUE && enteredZero < HALLZERO_MAX_VALUE) {
				HallZero = enteredZero;
				eeprom_write_word(HALL_ZERO_EEPROM_ADDR, HallZero);
				eeprom_update();
				serial_send_answer(OK_CMD);
			} else
				serial_send_answer(INVALID_CMD);
			
			break;
			
		case 'q': // Выход в главную секцию
			pwm_shutdown();
			CommandParser = main_parser;
			serial_send_answer(OK_CMD);
			break;
		
		default: // Ошибка - неизвестная команда
			serial_send_answer(INVALID_CMD);
			break;
	}
}


/* Прием и обработка команд пользователя. Секция режима AC TIG */
void cmd_actig_section(void) {
	serial_receive_line();
	if (is_psu_in_error_state()) return;
	
	switch(CmdBuffer[0]) {
		case 'F': // Включение/выключение охлаждения
			switch (CmdBuffer[1]) {
				case '1': FanManualSwitch = true; break;
				case '0': FanManualSwitch = false; break;
				default: 
					serial_send_answer(INVALID_CMD);
					return;
			}
			fan_switch(true);
			serial_send_answer(OK_CMD);
			break;
			
		case 'g': // Включение газового клапана
			ioport_set_pin_level(GAS_SWITCH, true);
			serial_send_answer(OK_CMD);
			break;
		
		case 'r': // Выключение газового клапана
			ioport_set_pin_level(GAS_SWITCH, false);
			serial_send_answer(OK_CMD);
			break;
		
		case 'a': // Задание тока
			if (!is_num_str(CmdBuffer + 1))
				serial_send_answer(INVALID_CMD);
		
			int enteredAmp = atoi(CmdBuffer + 1);
			if (enteredAmp < DCTIG_MIN_AMPERAGE || enteredAmp > DCTIG_MAX_AMPERAGE) {
				serial_send_answer(INVALID_CMD);
				return;
			}
		
			TargetAmpEN = HallZero + (enteredAmp * 2);
			TargetAmpENR = HallZero;
			serial_send_answer(OK_CMD);
			break;
		
		case 's': // Запуск шим
			pwm_start();
			serial_send_log();
			break;
		
		case 'e': // Остановка шим
			pwm_shutdown();
			serial_send_answer(OK_CMD);
			break;
		
		case 'l': // Вывод данных состояния
			serial_send_log();
			break;

		case 'd': // Вывод дампа переменных
			serial_send_dump();
			break;		
		
		case 'q': // Выход в главную секцию
			pwm_shutdown();
			CommandParser = main_parser;
			serial_send_answer(OK_CMD);
			break;
		
		case 'b': // Задание баланса
			if (!is_num_str(CmdBuffer + 1))
				serial_send_answer(INVALID_CMD);
			
			int enteredBalance = atoi(CmdBuffer + 1);
			if (enteredBalance < BALANCE_MIN_VALUE || enteredBalance > BALANCE_MAX_VALUE) {
				serial_send_answer(INVALID_CMD);
				return;
			}
			
			ACBalance = enteredBalance;
			ACEPCounter = ACPeriodCounter * ACBalance / 100;
			serial_send_answer(OK_CMD);
			break;
			
		case 'c': // Задание тока очистки
			if (!is_num_str(CmdBuffer + 1))
				serial_send_answer(INVALID_CMD);
			
			int enteredCAmp = atoi(CmdBuffer + 1);
			if (enteredCAmp < CAMP_MIN_VALUE || enteredCAmp > CAMP_MAX_VALUE) {
				serial_send_answer(INVALID_CMD);
				return;
			}
			
			/* Вычисление целевого тока очистки (уставка, абсолютное значение) */
			TargetAmpEP = HallZero - (enteredCAmp * 2);
			
			/* Подрежем ток переключения c EN на EP так как уставка EP может быть меньше */
			if (enteredCAmp < AC_SWITCH_AMPERAGE)
				SwitchENAmperage = HallZero + (enteredCAmp * 2);
			else
				SwitchENAmperage = HallZero + AC_SWITCH_AMPERAGE * 2;
			
			serial_send_answer(OK_CMD);
			break;
			
		case 'f': // Задание частоты
			if (!is_num_str(CmdBuffer + 1))
				serial_send_answer(INVALID_CMD);
			
			int enteredFreq = atoi(CmdBuffer + 1);
			if (enteredFreq < FREQ_MIN_VALUE || enteredFreq > FREQ_MAX_VALUE) {
				serial_send_answer(INVALID_CMD);
				return;
			}
			
			// Расчет периода и времени EN
			ACPeriodCounter = 10000 / enteredFreq;
			ACEPCounter = ACPeriodCounter * ACBalance / 100;
			serial_send_answer(OK_CMD);
			break;
			
		case '=': // Переключение в режим постоянного тока (DC)
			ACActive = false;
			PartPlusActive = false;
			serial_send_answer(OK_CMD);
			break;
	
		case '~': // Переключение в режим переменного тока (AC)
			ACActive = true;
			PartPlusActive = false; // Начинаем с очистки
			serial_send_answer(OK_CMD);
			break;
		
		default: // Ошибка - неизвестная команда
			serial_send_answer(INVALID_CMD);
			break;
	}
}


/* Вывод состояния (ток, напряжение, вент, клапан, кнопка) */
NOINLINE void serial_send_log(void) {
	int amperage = 0;
	int voltage = 0;
	bool button = !ioport_get_pin_level(MAIN_BUTTON);
	
	// Проверим, что ШИМ работает и получим значения
	if ((PCTL1 & (1 << PRUN1)) != 0) {
		
		// Напряжение пока пороговое - 1 или 0
		voltage = ioport_get_pin_level(DC_INPUT);

		// Абсолютное значение тока
		if (RecentADC > HallZero) {
			amperage = (RecentADC - HallZero) / 2;
		} else {
			amperage = (HallZero - RecentADC) / 2;
		}		
	}
	
	// Сформируем и отправим ответ пользователю (III UUU B)
	char *outNext = OutBuffer;
	
	// Добавляем в ответ текущий ток
	itoa(amperage, outNext, 10);
	while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);
	*outNext = ' ';
	outNext++;
	
	// Добавляем в ответ напряжение на выходе
	itoa(voltage, outNext, 10);
	while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);
	*outNext = ' ';
	outNext++;

	// Добавляем состояние главной кнопки
	if (button) *outNext = '1'; else *outNext = '0';
	outNext++;
	*outNext = 0;

	// Дополняем до 15 байт (16тый - перенос строки)
	int len = outNext - OutBuffer;
	if (len < 15) {
		memset(outNext, ' ', 15 - len);
	}

	OutBuffer[15] = 0xd; // Перенос в конце строки
	OutBuffer[16] = 0x0; // Замыкающий ноль в конце строки

	serial_send_answer(OutBuffer);
}


/* Вывод дампа переменных */
NOINLINE void serial_send_dump(void) {
	
	// Сформируем и отправим ответ пользователю (III UUU B)
	char *outNext = OutBuffer;
	
	// Состояние плечей АС коммутатора
	itoa(PartPlusActive, outNext, 10);
	while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);
	*outNext = ' ';
	outNext++;

    // "Сырое" значение силы тока, полученное с АЦП
	ltoa(RecentADC, outNext, 10);
	while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);
	*outNext = ' ';
	outNext++;
	
	// Значение АЦП для переключения на EP
	itoa(SwitchEPAmperage, outNext, 10);
	while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);
	*outNext = ' ';
	outNext++;

	// Значение АЦП для переключения на EN
	itoa(SwitchENAmperage, outNext, 10);
	while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);
	*outNext = ' ';
	outNext++;

	// Счетчик времени алгоритма формирования перменного тока
	ltoa(ACTimeCounter, outNext, 10);
	while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);
	*outNext = ' ';
	outNext++;

	// Максимальное значение счетчика времени для плеча EP
	itoa(ACEPCounter, outNext, 10);
	while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);
	*outNext = ' ';
	outNext++;

	// Максимальное значение счетчика времени для периода переменного тока
	ltoa(ACPeriodCounter, outNext, 10);
	while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);
	*outNext = 0xd;
	outNext++;

	*outNext = 0;
	serial_send_answer(OutBuffer);
}


/* Проверяет является ли строка числовым значением */
inline bool is_num_str(char *str) {
	char *maxStr = str + MAX_COMMAND_LEN;
	while (*str != 0 && *str != 0xd && str < maxStr) {
		if (*str != '+' && *str != '-' && ((*str > '9') || (*str < '0')))
			return false;
		str++;
	}

	return true;
}

#endif
