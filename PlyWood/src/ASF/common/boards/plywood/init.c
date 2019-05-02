#include <asf.h>
#include <conf_board.h>

/* ��������� �������� �������� */
void timers_setup(void) {
	
	/* WGM = 0b111  - Fast PWM, TOP=OCRA, TOV=TOP, update OCRx at TOP */
	/* COM0B = 0b10 - Clear OC0B on Compare Match                     */
	/* COM0A = 0b00 - Normal port operation, OC0A disconnected        */
	TCCR0A |= (1 << WGM00) | (1 << WGM01) | (1 << COM0B1);
	TCCR0B |= (1 << WGM02);
	
	/* OCR0A - TOP �������� �������� */
	OCR0A = 200;
	OCR0B = 100;
	
	/* ��������� ���������� Timer/Counter Compare Match B */
	TIMSK0 |= (1 << OCIE0B);
	
	/* ����� ������������: clkio/8 = 16000000/8 = 2000000 */
	/* ���������� ������ 200 ������ - 10000 ��� � ������� */
	TCCR0B |= (1 << CS01);
}


/* ������������� ��������� ����� ���������� */
void board_init(void)
{	
	/* �������� ������� PLL = 64��� */
	PLLCSR |= (1 << PLLF) | (1 << PLLE);
	while ((PLLCSR & (1 << PLOCK)) == 0) { }
		
	/* ������������� ����� GPIO (���������� � conf_board.h) */
	ioport_init();

    /* ���������� ����� */
	ioport_set_pin_dir(MAIN_BUTTON, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(MAIN_BUTTON, IOPORT_MODE_PULLUP);
	ioport_set_pin_dir(FAN_SENS, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(FAN_SENS, IOPORT_MODE_PULLUP);
	ioport_set_pin_dir(OVRTMP_SENS, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(OVRTMP_SENS, IOPORT_MODE_PULLUP);
	ioport_set_pin_dir(SPI_SELECT, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(SPI_SELECT, IOPORT_MODE_PULLUP);
	ioport_set_pin_dir(DC_INPUT, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(DC_INPUT, IOPORT_MODE_PULLDOWN);

	/* �������� �������� MIG/MAG */
	ioport_set_pin_dir(MIG_OUT, IOPORT_DIR_OUTPUT);
	ioport_set_pin_mode(MIG_OUT, IOPORT_MODE_PULLUP);

    /* ����������� ���������� ������ */
	ioport_set_pin_dir(GAS_SWITCH, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(GAS_SWITCH, false);

	ioport_set_pin_dir(CHRG_SWITCH, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(CHRG_SWITCH, false);

	ioport_set_pin_dir(FAN_SWITCH, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(FAN_SWITCH, false);

	ioport_set_pin_dir(ERROR_LED, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(ERROR_LED, false);
	
	/* ������ ���������� AC ��������� */
	ioport_set_pin_dir(ACP_DRV, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(ACP_DRV, false);
	
	ioport_set_pin_dir(ACM_DRV, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(ACM_DRV, false);
	
	/* ������������� ��� */
	ADCSRA  = (1 << ADPS2)		// �������� 16 (������� ��� 1Mhz)
			| (1 << ADIE)		// �������� ���������� �� ���
			| (1 << ADATE)		// �������������� ����� ���������
			| (1 << ADEN);		// �������� ���;
			
	ADCSRB  = (1 << ADTS0)		// �������� �������� ��� 
			| (1 << ADTS3)		// - ������� �� PSC1ASY
			| (1 << ADHSM);		// ���������������� �����
			
	ADMUX   = (1 << REFS0);		// AV�� � ������� ������������� �� ���� AREF
	ADMUX  |= (1 << MUX3);		// �������� ADC8
	
	/* ������������� ��� - �������� �������� MIG/MAG */
	DACON   = (1 << DAEN)		// �������� ���
			| (1 << DAOE);		// ��������� ��������� �� D2A
			
	/* ��������� ����������� ������ �� ���������� �� ���� */
	AC1CON  = (1 << AC1M2)		// ����� ����������� �� ACMPM (PD6)
			| (1 << AC1IS0)		// ���������� �� 
			| (1 << AC1IS1)		// ������� ������
			| (1 << AC1IE)		// ��������� ���������� �����������
			| (1 << AC1EN);		// �������� ����������

	/* ��������� �������� ����� */
	PSOC1   = (1 << POEN1A)		// �������� ����� PSC1 �� ��� A (PC0)
			| (1 << POEN1B)		// �������� ����� PSC1 �� ��� B (PB6)
			| (1 << PSYNC1_0)	// �������� ������� �� ����� PSCOUT11
			| (1 << PSYNC1_1);	// (��������� � OCR1RB)
		
	PCNF1   = (1 << PCLKSEL1)	// ����� ������������ PSC1 (fast clock)
			| (1 << POP1)		// ����� ���������� PSC1 (active high)
			| (1 << PMODE10)	// ����� ������ PSC1 (two ramp)
			| (1 << PALOCK1);	// ����� �������������� (��� ���������������)
	
	/* ��������� ���������� PSC1 ��� �������� ����������� - ������ */
	PFRC1A  = (1 << PRFM0A0)	// ����� �������� 7:
			| (1 << PRFM0A1)	// ��������� ��� �
			| (1 << PRFM0A2)	// ������� ������� ������������ �����������
			| (1 << PELEV0A)	// �������� �� ������� �������
			| (1 << PISEL1A)	// ���������� ���������� 1 �� ���� ������
			| (1 << PCAE1A);	// �������� ���� ������

	PFRC1B  = (1 << PRFM0B0)	// ����� �������� 7:
			| (1 << PRFM0B1)	// ��������� ��� �
			| (1 << PRFM0B2)	// ������� ������� ������������ �����������
			| (1 << PELEV0B)	// �������� �� ������� �������
			| (1 << PISEL1B)	// ����� ����������� 1 � �������� ����� ������
			| (1 << PCAE1B);	// �������� ���� ������
			
	/* ��������� �������� */
	timers_setup();

	sei();	
	delay_ms(125);
	
	/* ������ �������� � ���������� �� eeprom */
	eeprom_read_settings();
	
	/* ������������� ��������� ��� �������������� � ������������� */
	if (is_spi_needed()) 
	{
		MCUCR |= (1 << SPIPS);				// ����� ��������������� ������ ����� SPI
		SPCR  = (1 << SPE);					// ��������� SPI
		SPSR  = ((SPISettings >> 8) & 1);	// SPI2X ��� (9��� � �������)
		SPCR |= (SPISettings & 3);			// ������� 1 ��� ��������
		
		/* ��� ����� ������ ���� input ����� MISO */
		ioport_set_pin_dir(SS_PIN, IOPORT_DIR_INPUT);
		ioport_set_pin_mode(SS_PIN, IOPORT_MODE_PULLUP);
		
		ioport_set_pin_dir(MOSI_PIN, IOPORT_DIR_INPUT);
		ioport_set_pin_mode(MOSI_PIN, IOPORT_MODE_PULLUP);

		ioport_set_pin_dir(SCK_PIN, IOPORT_DIR_INPUT);
		ioport_set_pin_mode(SCK_PIN, IOPORT_MODE_PULLUP);

		ioport_set_pin_dir(MISO_PIN, IOPORT_DIR_OUTPUT);
		ioport_set_pin_mode(MISO_PIN, IOPORT_MODE_PULLDOWN);
		
	} else {
		ioport_set_pin_dir(SS_PIN, IOPORT_DIR_INPUT);
		ioport_set_pin_mode(SS_PIN, IOPORT_MODE_PULLUP);
		
		/* ���� U2X �������� � 0, ����� �������� ������� */
		UCSRA = (1 << U2X);
		
		/* ������ �������� UART ����� */
		UBRRH = (UARTBaudRate >> 8) & 0xFF;
		UBRRL = (UARTBaudRate) & 0xFF;
		
		/* ��������� 8n1 */
		UCSRC = (1 << UCPOL);
		UCSRC = (1 << UCSZ0) | (1 << UCSZ1);
		
		/* �������� ����� � �������� */
		UCSRB = (1 << TXEN) | (1 << RXEN);
	}
}

