/* ��������� �������� ��������� sam_soft. �������� ���       */
/* ����������� ���������� ���������� ���������� ��������� �� */
/* ������� ������������ ���������, � ����� ��� ������������� */
/* � ��� ���������� ����������� � ������������ ��������      */
/* � ����������� sam_soft ��������� ��������� ������� � mma  */

#include <asf.h>
#ifndef BINARY_PROTOCOL

/* �������� �� ������ ����� ���������� ������� */
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


/* ����� � ��������� ������ ������������. ������� ������. */
void main_parser(void) {
	
	/* � ������� ������ ��� ���������, ���� ���, �������� ��� */
	ActiveMode = MODE_NONE;
	ioport_set_pin_level(ACP_DRV, false);
	ioport_set_pin_level(ACM_DRV, false);

	/* ��������� ������� ������������ */
	serial_receive_line();
	if (is_psu_in_error_state()) return;

	switch(CmdBuffer[0]) {
		case 'm': // ��������� � ������ mma
			ActiveMode = MODE_MMA;
			CommandParser = cmd_mma_section;
			serial_send_answer(OK_CMD);
			break;
		case 'd': // ��������� � ������ dc_tig
			ActiveMode = MODE_DCTIG;
			CommandParser = cmd_dctig_section;
			serial_send_answer(OK_CMD);
			break;
		case 'a': // ��������� � ������ ac_tig
			ActiveMode = MODE_ACTIG;
			ACActive = false;
			CommandParser = cmd_actig_section;
			serial_send_answer(OK_CMD);
			break;
		case 'c': // ��������� � ������ ����������
			ActiveMode = MODE_MMA;
			CommandParser = cmd_cal_section;
			serial_send_answer(OK_CMD);
			break;
		case 'v': // ���������� ������ ��������
			serial_send_answer(FIRMWARE_VERSION);
			break;
		default:
			serial_send_answer(INVALID_CMD);
			break;
	}
}


/* ����� � ��������� ������ ������������. ������ ������ MMA */
void cmd_mma_section(void) {
	serial_receive_line();
	if (is_psu_in_error_state()) return;
	
	switch(CmdBuffer[0]) {
		case 's': // ������ ���
			pwm_start();
			serial_send_log();
			break;
			
		case 'e': // ��������� ���
			pwm_shutdown();
			serial_send_answer(OK_CMD);
			break;
			
		case 'q': // ����� � ������� ������
			pwm_shutdown();
			CommandParser = main_parser;
			serial_send_answer(OK_CMD);
			break;
			
		case 'l': // ����� ������ ���������
			serial_send_log();
			break;

		case 'd': // ����� ����� ����������
			serial_send_dump();
			break;
			
		case 'F': // ���������/���������� ����������
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
			
		case 'a': // ������� ����
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
			
		case 'p': // ��������� ���������� (������)
			switch (CmdBuffer[1]) {
				case '1': DCRPolarity = true; break;
				case '0': DCRPolarity = false; break;
				default:
					serial_send_answer(INVALID_CMD);
					return;
			}
			serial_send_answer(OK_CMD);
			break;
			
		default: // ������ - ����������� �������
			serial_send_answer(INVALID_CMD);
			break;
	}
}


/* ����� � ��������� ������ ������������. ������ ������ DC TIG. */
void cmd_dctig_section(void) {
	serial_receive_line();
	if (is_psu_in_error_state()) return;

	switch(CmdBuffer[0]) {
		case 'F': // ���������/���������� ����������
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

		case 'g': // ��������� �������� �������
			ioport_set_pin_level(GAS_SWITCH, true);
			serial_send_answer(OK_CMD);
			break;
			
		case 'r': // ���������� �������� �������
			ioport_set_pin_level(GAS_SWITCH, false);
			serial_send_answer(OK_CMD);
			break;
			
		case 'a': // ������� ����
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
			
		case 's': // ������ ���
			pwm_start();
			serial_send_log();
			break;
			
		case 'e': // ��������� ���
			pwm_shutdown();
			serial_send_answer(OK_CMD);
			break;
					
		case 'l': // ����� ������ ���������
			serial_send_log();
			break;

		case 'd': // ����� ����� ����������
			serial_send_dump();
			break;
					
		case 'q': // ����� � ������� ������
			pwm_shutdown();
			CommandParser = main_parser;
			serial_send_answer(OK_CMD);
			break;		
			
		default: // ������ - ����������� �������
			serial_send_answer(INVALID_CMD);
			break;
	}
}


/* ����� � ��������� ������ ������������. ������ ������ ���������� */
void cmd_cal_section(void) {
		char *outNext = OutBuffer;
	serial_receive_line();
	if (is_psu_in_error_state()) return;
	
	switch(CmdBuffer[0]) {
		case 'F': // ���������/���������� ����������
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
			
		case 'a': // ������� ����
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
		
		case 'p': // ��������� ���������� (������)
			switch (CmdBuffer[1]) {
				case '1': DCRPolarity = true; break;
				case '0': DCRPolarity = false; break;
				default: 
					serial_send_answer(INVALID_CMD);
					return;
			}
			serial_send_answer(OK_CMD);
			break;
		
		case 's': // ������ ���
			pwm_start();
			serial_send_log();
			break;
		
		case 'e': // ��������� ���
			pwm_shutdown();
			serial_send_answer(OK_CMD);
			break;
		
		case 'l': // ����� ������ ���������
			serial_send_log();
			break;

		case 'd': // ����� ����� ����������
			serial_send_dump();
			break;		
		
		case 'z': // ����� ������� ������� ����� ������� �����
			itoa(HallZero, outNext, 10);
			while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);

			// ��������� �� 15 ���� (16��� - ������� ������)
			int len = outNext - OutBuffer;
			if (len < 15) {
				memset(outNext, ' ', 15 - len);
			}

			OutBuffer[15] = 0xd; // ������� � ����� ������
			OutBuffer[16] = 0x0; // ���������� ���� � ����� ������	
			
			serial_send_answer(OutBuffer);
			break;
		
		case 'Z': // ��������� ������� ����� ������� �����
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
			
		case 'q': // ����� � ������� ������
			pwm_shutdown();
			CommandParser = main_parser;
			serial_send_answer(OK_CMD);
			break;
		
		default: // ������ - ����������� �������
			serial_send_answer(INVALID_CMD);
			break;
	}
}


/* ����� � ��������� ������ ������������. ������ ������ AC TIG */
void cmd_actig_section(void) {
	serial_receive_line();
	if (is_psu_in_error_state()) return;
	
	switch(CmdBuffer[0]) {
		case 'F': // ���������/���������� ����������
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
			
		case 'g': // ��������� �������� �������
			ioport_set_pin_level(GAS_SWITCH, true);
			serial_send_answer(OK_CMD);
			break;
		
		case 'r': // ���������� �������� �������
			ioport_set_pin_level(GAS_SWITCH, false);
			serial_send_answer(OK_CMD);
			break;
		
		case 'a': // ������� ����
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
		
		case 's': // ������ ���
			pwm_start();
			serial_send_log();
			break;
		
		case 'e': // ��������� ���
			pwm_shutdown();
			serial_send_answer(OK_CMD);
			break;
		
		case 'l': // ����� ������ ���������
			serial_send_log();
			break;

		case 'd': // ����� ����� ����������
			serial_send_dump();
			break;		
		
		case 'q': // ����� � ������� ������
			pwm_shutdown();
			CommandParser = main_parser;
			serial_send_answer(OK_CMD);
			break;
		
		case 'b': // ������� �������
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
			
		case 'c': // ������� ���� �������
			if (!is_num_str(CmdBuffer + 1))
				serial_send_answer(INVALID_CMD);
			
			int enteredCAmp = atoi(CmdBuffer + 1);
			if (enteredCAmp < CAMP_MIN_VALUE || enteredCAmp > CAMP_MAX_VALUE) {
				serial_send_answer(INVALID_CMD);
				return;
			}
			
			/* ���������� �������� ���� ������� (�������, ���������� ��������) */
			TargetAmpEP = HallZero - (enteredCAmp * 2);
			
			/* �������� ��� ������������ c EN �� EP ��� ��� ������� EP ����� ���� ������ */
			if (enteredCAmp < AC_SWITCH_AMPERAGE)
				SwitchENAmperage = HallZero + (enteredCAmp * 2);
			else
				SwitchENAmperage = HallZero + AC_SWITCH_AMPERAGE * 2;
			
			serial_send_answer(OK_CMD);
			break;
			
		case 'f': // ������� �������
			if (!is_num_str(CmdBuffer + 1))
				serial_send_answer(INVALID_CMD);
			
			int enteredFreq = atoi(CmdBuffer + 1);
			if (enteredFreq < FREQ_MIN_VALUE || enteredFreq > FREQ_MAX_VALUE) {
				serial_send_answer(INVALID_CMD);
				return;
			}
			
			// ������ ������� � ������� EN
			ACPeriodCounter = 10000 / enteredFreq;
			ACEPCounter = ACPeriodCounter * ACBalance / 100;
			serial_send_answer(OK_CMD);
			break;
			
		case '=': // ������������ � ����� ����������� ���� (DC)
			ACActive = false;
			PartPlusActive = false;
			serial_send_answer(OK_CMD);
			break;
	
		case '~': // ������������ � ����� ����������� ���� (AC)
			ACActive = true;
			PartPlusActive = false; // �������� � �������
			serial_send_answer(OK_CMD);
			break;
		
		default: // ������ - ����������� �������
			serial_send_answer(INVALID_CMD);
			break;
	}
}


/* ����� ��������� (���, ����������, ����, ������, ������) */
NOINLINE void serial_send_log(void) {
	int amperage = 0;
	int voltage = 0;
	bool button = !ioport_get_pin_level(MAIN_BUTTON);
	
	// ��������, ��� ��� �������� � ������� ��������
	if ((PCTL1 & (1 << PRUN1)) != 0) {
		
		// ���������� ���� ��������� - 1 ��� 0
		voltage = ioport_get_pin_level(DC_INPUT);

		// ���������� �������� ����
		if (RecentADC > HallZero) {
			amperage = (RecentADC - HallZero) / 2;
		} else {
			amperage = (HallZero - RecentADC) / 2;
		}		
	}
	
	// ���������� � �������� ����� ������������ (III UUU B)
	char *outNext = OutBuffer;
	
	// ��������� � ����� ������� ���
	itoa(amperage, outNext, 10);
	while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);
	*outNext = ' ';
	outNext++;
	
	// ��������� � ����� ���������� �� ������
	itoa(voltage, outNext, 10);
	while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);
	*outNext = ' ';
	outNext++;

	// ��������� ��������� ������� ������
	if (button) *outNext = '1'; else *outNext = '0';
	outNext++;
	*outNext = 0;

	// ��������� �� 15 ���� (16��� - ������� ������)
	int len = outNext - OutBuffer;
	if (len < 15) {
		memset(outNext, ' ', 15 - len);
	}

	OutBuffer[15] = 0xd; // ������� � ����� ������
	OutBuffer[16] = 0x0; // ���������� ���� � ����� ������

	serial_send_answer(OutBuffer);
}


/* ����� ����� ���������� */
NOINLINE void serial_send_dump(void) {
	
	// ���������� � �������� ����� ������������ (III UUU B)
	char *outNext = OutBuffer;
	
	// ��������� ������ �� �����������
	itoa(PartPlusActive, outNext, 10);
	while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);
	*outNext = ' ';
	outNext++;

    // "�����" �������� ���� ����, ���������� � ���
	ltoa(RecentADC, outNext, 10);
	while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);
	*outNext = ' ';
	outNext++;
	
	// �������� ��� ��� ������������ �� EP
	itoa(SwitchEPAmperage, outNext, 10);
	while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);
	*outNext = ' ';
	outNext++;

	// �������� ��� ��� ������������ �� EN
	itoa(SwitchENAmperage, outNext, 10);
	while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);
	*outNext = ' ';
	outNext++;

	// ������� ������� ��������� ������������ ���������� ����
	ltoa(ACTimeCounter, outNext, 10);
	while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);
	*outNext = ' ';
	outNext++;

	// ������������ �������� �������� ������� ��� ����� EP
	itoa(ACEPCounter, outNext, 10);
	while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);
	*outNext = ' ';
	outNext++;

	// ������������ �������� �������� ������� ��� ������� ����������� ����
	ltoa(ACPeriodCounter, outNext, 10);
	while (*outNext != 0 && (outNext++ - OutBuffer) < MAX_COMMAND_LEN);
	*outNext = 0xd;
	outNext++;

	*outNext = 0;
	serial_send_answer(OutBuffer);
}


/* ��������� �������� �� ������ �������� ��������� */
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
