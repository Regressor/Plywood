#ifndef CONF_BOARD_H
#define CONF_BOARD_H

//#define BINARY_PROTOCOL 1						// ���� ����������������� �� ��������� ������ � �������� ����������

#define FIRMWARE_VERSION	"PWv1102        \r"	// ������ ��������

/* ������������ ���� */
#define MIG_OUT     IOPORT_CREATE_PIN(PORTC, 7)	// ����� ��������� �������� MIG/MAG
#define GAS_SWITCH  IOPORT_CREATE_PIN(PORTB, 5)	// ����� ���������� �������� ����
#define CHRG_SWITCH IOPORT_CREATE_PIN(PORTB, 3)	// ����� �� ���� ������ ���� ������� ������� �������������
#define FAN_SWITCH  IOPORT_CREATE_PIN(PORTB, 4)	// ����� �� ���������� ������������ ����������
#define MAIN_BUTTON IOPORT_CREATE_PIN(PORTD, 1)	// ���� ������� ������ �� �������
#define OVRTMP_SENS IOPORT_CREATE_PIN(PORTC, 2)	// ���� ���� ���������
#define FAN_SENS    IOPORT_CREATE_PIN(PORTC, 3)	// ���� ��������� ����������� �� �������
#define SPI_SELECT  IOPORT_CREATE_PIN(PORTD, 5)	// ���� ������ ���� ���������� (0 - UART, 1 - SPI)
#define ERROR_LED   IOPORT_CREATE_PIN(PORTB, 7)	// ����� - ����� ������ (��������, ����������)
#define DC_INPUT    IOPORT_CREATE_PIN(PORTC, 5)	// ���� - �������� ����� �� ����������

#define ACP_DRV     IOPORT_CREATE_PIN(PORTD, 7)	// ����� ��������� �������������� ����� ��������� AC
#define ACM_DRV     IOPORT_CREATE_PIN(PORTB, 2)	// ����� ��������� �������������� ����� ��������� AC

#define SS_PIN      IOPORT_CREATE_PIN(PORTD, 0)	// ��� SS (slave select)
#define MOSI_PIN    IOPORT_CREATE_PIN(PORTD, 3)	// ��� MOSI (master out slave in)
#define MISO_PIN    IOPORT_CREATE_PIN(PORTD, 2)	// ��� MISO (master in slave out)
#define SCK_PIN     IOPORT_CREATE_PIN(PORTD, 4)	// ��� SCK (�������� ����� SPI)

/* ��������� eeprom */
#define EEPROM_DATA_SIZE 64						// ������ ������������� (� �����������) ����� ������ eeprom

#define HALL_ZERO_EEPROM_ADDR		0x0004		// ����� ������� ����� Hall-������� � EEPROM
#define UART_BAUDRATE_EEPROM_ADDR	0x0006		// ����� �������� �������� UART � EEPROM
#define SPI_SETTINGS_EEPROM_ADDR	0x0008		// ����� �������� SPI � EEROM

#define DEFAULT_SPI_SETTINGS		0x0003		// EEPROM ��������� SPI �� ��������� (Fclkio/128, leading sample, u2x=0)
#define DEFAULT_UART_SETTINGS		0x0011		// EEPROM �������� UART �� ��������� 115200 8n1
#define DEFAULT_HALL_ZERO			0x0200		// EEPROM ����� ���� Hall �� ��������� (512)

/* ������ �������� ������������� �������� */
#define MAX_COMMAND_LEN			32				// ������������ ����� ������� � ������ (����� ���������, ���� ������ ����)
#define DEFAULT_AMPERAGE		5				// ��� �� ��������� � �������
#define DEFAULT_AC_FREQUENCY	200				// ������� AC �� ��������� � ������
#define DEFAULT_AC_BALANCE		20				// ������ AC �� ��������� � ���������

#define MMA_MIN_AMPERAGE		5				// ����������� ��� ������ MMA
#define MMA_MAX_AMPERAGE		200				// ������������ ��� ������ MMA

#define DCTIG_MIN_AMPERAGE		5				// ����������� ��� ������ DC TIG
#define DCTIG_MAX_AMPERAGE		200				// ������������ ��� ������ DC TIG

#define CAL_MIN_AMPERAGE		5				// ����������� ��� ������ ����������
#define CAL_MAX_AMPERAGE		200				// ������������ ��� ������ ����������

#define HALLZERO_MIN_VALUE		391				// ����������� �������� ���� ������� �����
#define HALLZERO_MAX_VALUE		633				// ������������ �������� ���� ������� �����

#define FREQ_MIN_VALUE			50				// ����������� �������� ������� ����������� ����
#define FREQ_MAX_VALUE			200				// ������������ �������� ������� ����������� ����

#define CAMP_MIN_VALUE			10				// ����������� �������� ���� �������
#define CAMP_MAX_VALUE			100				// ������������ �������� ���� �������

#define BALANCE_MIN_VALUE		20				// ����������� �������� ������� ����������� ����
#define BALANCE_MAX_VALUE		80				// ������������ �������� ������� ����������� ����

/* ��������� ���������� DC-DC ��������������� */
#define PWM_FREQUENCY			40000			// ������� �������������� � ������ (�� ������ �����)
#define PWM_DEADTIME			1700			// ������� ����� � ������������
#define PWM_MINPULSE			225				// ����������� ����� �������� � ������������

/* ��������� ���������� DC-AC ��������������� */
#define AC_SWITCH_AMPERAGE		30				// ������������ ��� ��� ������������ ��������� DC-AC


#endif // CONF_BOARD_H

