#include <asf.h>
#include <conf_board.h>

/* Установка значений таймеров */
void timers_setup(void) {
	
	/* WGM = 0b111  - Fast PWM, TOP=OCRA, TOV=TOP, update OCRx at TOP */
	/* COM0B = 0b10 - Clear OC0B on Compare Match                     */
	/* COM0A = 0b00 - Normal port operation, OC0A disconnected        */
	TCCR0A |= (1 << WGM00) | (1 << WGM01) | (1 << COM0B1);
	TCCR0B |= (1 << WGM02);
	
	/* OCR0A - TOP значения счетчика */
	OCR0A = 200;
	OCR0B = 100;
	
	/* Разрешить прерывание Timer/Counter Compare Match B */
	TIMSK0 |= (1 << OCIE0B);
	
	/* Выбор тактирования: clkio/8 = 16000000/8 = 2000000 */
	/* Прерывание каждые 200 тактов - 10000 раз в секунду */
	TCCR0B |= (1 << CS01);
}


/* Инициализация периферии платы управления */
void board_init(void)
{	
	/* Настроим частоту PLL = 64Мгц */
	PLLCSR |= (1 << PLLF) | (1 << PLLE);
	while ((PLLCSR & (1 << PLOCK)) == 0) { }
		
	/* Инициализация пинов GPIO (определены в conf_board.h) */
	ioport_init();

    /* Дискретные входы */
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

	/* Задатчик скорости MIG/MAG */
	ioport_set_pin_dir(MIG_OUT, IOPORT_DIR_OUTPUT);
	ioport_set_pin_mode(MIG_OUT, IOPORT_MODE_PULLUP);

    /* Управляющие дискретные выходы */
	ioport_set_pin_dir(GAS_SWITCH, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(GAS_SWITCH, false);

	ioport_set_pin_dir(CHRG_SWITCH, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(CHRG_SWITCH, false);

	ioport_set_pin_dir(FAN_SWITCH, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(FAN_SWITCH, false);

	ioport_set_pin_dir(ERROR_LED, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(ERROR_LED, false);
	
	/* Выходы управления AC полумоста */
	ioport_set_pin_dir(ACP_DRV, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(ACP_DRV, false);
	
	ioport_set_pin_dir(ACM_DRV, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(ACM_DRV, false);
	
	/* Инициализация АЦП */
	ADCSRA  = (1 << ADPS2)		// делитель 16 (частота АЦП 1Mhz)
			| (1 << ADIE)		// включаем прерывание на АЦП
			| (1 << ADATE)		// автоматический старт измерения
			| (1 << ADEN);		// включить АЦП;
			
	ADCSRB  = (1 << ADTS0)		// источник сработки АЦП 
			| (1 << ADTS3)		// - событие на PSC1ASY
			| (1 << ADHSM);		// высокоскоростной режим
			
	ADMUX   = (1 << REFS0);		// AVСС с внешним конденсатором на пине AREF
	ADMUX  |= (1 << MUX3);		// включить ADC8
	
	/* Инициализация ЦАП - задатчик скорости MIG/MAG */
	DACON   = (1 << DAEN)		// включить ЦАП
			| (1 << DAOE);		// результат конверсии на D2A
			
	/* Настройка компаратора защиты от перегрузки по току */
	AC1CON  = (1 << AC1M2)		// минус компаратора на ACMPM (PD6)
			| (1 << AC1IS0)		// прерывание на 
			| (1 << AC1IS1)		// подъеме выхода
			| (1 << AC1IE)		// разрешить прерывание компаратора
			| (1 << AC1EN);		// включить компаратор

	/* Настройка раскачки моста */
	PSOC1   = (1 << POEN1A)		// включить выход PSC1 на пин A (PC0)
			| (1 << POEN1B)		// включить выход PSC1 на пин B (PB6)
			| (1 << PSYNC1_0)	// отправка сигнала на спаде PSCOUT11
			| (1 << PSYNC1_1);	// (сравнение с OCR1RB)
		
	PCNF1   = (1 << PCLKSEL1)	// выбор тактирования PSC1 (fast clock)
			| (1 << POP1)		// выбор полярности PSC1 (active high)
			| (1 << PMODE10)	// выбор режима PSC1 (two ramp)
			| (1 << PALOCK1);	// режим автоблокировки (для симметрирования)
	
	/* Настройка отключения PSC1 при сработке компаратора - защита */
	PFRC1A  = (1 << PRFM0A0)	// Режим сработки 7:
			| (1 << PRFM0A1)	// заглушить ШИМ и
			| (1 << PRFM0A2)	// ожидать реакции программного обеспечения
			| (1 << PELEV0A)	// сработка на подъеме сигнала
			| (1 << PISEL1A)	// подключить компаратор 1 на вход защиты
			| (1 << PCAE1A);	// включить блок защиты

	PFRC1B  = (1 << PRFM0B0)	// Режим сработки 7:
			| (1 << PRFM0B1)	// заглушить ШИМ и
			| (1 << PRFM0B2)	// ожидать реакции программного обеспечения
			| (1 << PELEV0B)	// сработка на подъеме сигнала
			| (1 << PISEL1B)	// выбор компаратора 1 в качестве входа защиты
			| (1 << PCAE1B);	// включить блок защиты
			
	/* Настройка таймеров */
	timers_setup();

	sei();	
	delay_ms(125);
	
	/* Чтение настроек и калибровок из eeprom */
	eeprom_read_settings();
	
	/* Конфигурируем интерфейс для взаимодействия с пользователем */
	if (is_spi_needed()) 
	{
		MCUCR |= (1 << SPIPS);				// выбор альтернативного набора пинов SPI
		SPCR  = (1 << SPE);					// включение SPI
		SPSR  = ((SPISettings >> 8) & 1);	// SPI2X бит (9тый в конфиге)
		SPCR |= (SPISettings & 3);			// старший 1 бит делителя
		
		/* Все входы должны быть input кроме MISO */
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
		
		/* Пока U2X выставим в 0, потом возможно изменим */
		UCSRA = (1 << U2X);
		
		/* Задаем скорость UART порта */
		UBRRH = (UARTBaudRate >> 8) & 0xFF;
		UBRRL = (UARTBaudRate) & 0xFF;
		
		/* настройки 8n1 */
		UCSRC = (1 << UCPOL);
		UCSRC = (1 << UCSZ0) | (1 << UCSZ1);
		
		/* включить прием и передачу */
		UCSRB = (1 << TXEN) | (1 << RXEN);
	}
}

