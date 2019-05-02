#ifndef CONF_BOARD_H
#define CONF_BOARD_H

//#define BINARY_PROTOCOL 1						// Если раскомментировать то соберется версия с бинарным протоколом

#define FIRMWARE_VERSION	"PWv1102        \r"	// Версия прошивки

/* Используемые пины */
#define MIG_OUT     IOPORT_CREATE_PIN(PORTC, 7)	// Выход задатчика скорости MIG/MAG
#define GAS_SWITCH  IOPORT_CREATE_PIN(PORTB, 5)	// Выход управления клапаном газа
#define CHRG_SWITCH IOPORT_CREATE_PIN(PORTB, 3)	// Выход на реле обхода цепи плавной зарядки конденсаторов
#define FAN_SWITCH  IOPORT_CREATE_PIN(PORTB, 4)	// Выход на управление вентилятором охлаждения
#define MAIN_BUTTON IOPORT_CREATE_PIN(PORTD, 1)	// Вход главной кнопки на держаке
#define OVRTMP_SENS IOPORT_CREATE_PIN(PORTC, 2)	// Вход реле перегрева
#define FAN_SENS    IOPORT_CREATE_PIN(PORTC, 3)	// Вход включения вентилятора по нагреву
#define SPI_SELECT  IOPORT_CREATE_PIN(PORTD, 5)	// Вход выбора типа интерфейса (0 - UART, 1 - SPI)
#define ERROR_LED   IOPORT_CREATE_PIN(PORTB, 7)	// Выход - лампа ошибки (перегрев, перегрузка)
#define DC_INPUT    IOPORT_CREATE_PIN(PORTC, 5)	// Вход - обратная связь по напряжению

#define ACP_DRV     IOPORT_CREATE_PIN(PORTD, 7)	// Выход включения положительного плеча полумоста AC
#define ACM_DRV     IOPORT_CREATE_PIN(PORTB, 2)	// Выход включения отрицательного плеча полумоста AC

#define SS_PIN      IOPORT_CREATE_PIN(PORTD, 0)	// Пин SS (slave select)
#define MOSI_PIN    IOPORT_CREATE_PIN(PORTD, 3)	// Пин MOSI (master out slave in)
#define MISO_PIN    IOPORT_CREATE_PIN(PORTD, 2)	// Пин MISO (master in slave out)
#define SCK_PIN     IOPORT_CREATE_PIN(PORTD, 4)	// Пин SCK (тактовая линия SPI)

/* Параметры eeprom */
#define EEPROM_DATA_SIZE 64						// Размер используемого (и защищенного) блока памяти eeprom

#define HALL_ZERO_EEPROM_ADDR		0x0004		// Адрес нулевой точки Hall-сенсора в EEPROM
#define UART_BAUDRATE_EEPROM_ADDR	0x0006		// Адрес значения скорости UART в EEPROM
#define SPI_SETTINGS_EEPROM_ADDR	0x0008		// Адрес настроек SPI в EEROM

#define DEFAULT_SPI_SETTINGS		0x0003		// EEPROM настройки SPI по умолчанию (Fclkio/128, leading sample, u2x=0)
#define DEFAULT_UART_SETTINGS		0x0011		// EEPROM скорость UART по умолчанию 115200 8n1
#define DEFAULT_HALL_ZERO			0x0200		// EEPROM точка нуля Hall по умолчанию (512)

/* Лимиты вводимых пользователем значений */
#define MAX_COMMAND_LEN			32				// Максимальная длина команды в байтах (можно уменьшить, если убрать дамп)
#define DEFAULT_AMPERAGE		5				// Ток по умолчанию в амперах
#define DEFAULT_AC_FREQUENCY	200				// Частота AC по умолчанию в герцах
#define DEFAULT_AC_BALANCE		20				// Баланс AC по умолчанию в процентах

#define MMA_MIN_AMPERAGE		5				// Минимальный ток режима MMA
#define MMA_MAX_AMPERAGE		200				// Максимальный ток режима MMA

#define DCTIG_MIN_AMPERAGE		5				// Минимальный ток режима DC TIG
#define DCTIG_MAX_AMPERAGE		200				// Максимальный ток режима DC TIG

#define CAL_MIN_AMPERAGE		5				// Минимальный ток режима калибровки
#define CAL_MAX_AMPERAGE		200				// Максимальный ток режима калибровки

#define HALLZERO_MIN_VALUE		391				// Минимальное значение нуля датчика Холла
#define HALLZERO_MAX_VALUE		633				// Максимальное значение нуля датчика Холла

#define FREQ_MIN_VALUE			50				// Минимальное значение частоты переменного тока
#define FREQ_MAX_VALUE			200				// Максимальное значение частоты переменного тока

#define CAMP_MIN_VALUE			10				// Минимальное значение тока очистки
#define CAMP_MAX_VALUE			100				// Максимальное значение тока очистки

#define BALANCE_MIN_VALUE		20				// Минимальное значение баланса переменного тока
#define BALANCE_MAX_VALUE		80				// Максимальное значение баланса переменного тока

/* Настройки коммутации DC-DC преобразователя */
#define PWM_FREQUENCY			40000			// Частота преобразования в герцах (на каждое плечо)
#define PWM_DEADTIME			1700			// Мертвое время в наносекундах
#define PWM_MINPULSE			225				// Минимальное время импульса в наносекундах

/* Настройки коммутации DC-AC преобразователя */
#define AC_SWITCH_AMPERAGE		30				// Максимальный ток для переключения полумоста DC-AC


#endif // CONF_BOARD_H

