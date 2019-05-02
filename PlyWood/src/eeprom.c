/* ��� ��� ��������� � ������ � ������ eeprom */
#include <asf.h>

/* ������ 1 ���� eeprom �� ��������� ������ */
unsigned char eeprom_read_byte(unsigned int addr)
{
	while (EECR & (1 << EEWE));
	EEAR = addr;
	EECR |= (1 << EERE);
	return EEDR;
}


/* ������ 2 ����� eeprom �� ���������� ������ */
unsigned int eeprom_read_word(unsigned int addr)
{
	unsigned int ret = 0;
	while (EECR & (1 << EEWE));
	
	/* ������ ������ ���� */
	EEAR = addr;
	EECR |= (1 << EERE);
	ret = (EEDR << 8);

	/* ������ ������ ���� */
	EEAR = addr + 1;
	EECR |= (1 << EERE);
	ret |= EEDR;
	
	return ret;
}


/* ���������� ���� � eeprom �� ��������� ������ */
void eeprom_write_byte(unsigned int addr, unsigned char data)
{
	while (EECR & (1 << EEWE));
	EEAR = addr;
	EEDR = data;
	EECR |= (1 << EEMWE);
	EECR |= (1 << EEWE);
}


/* ���������� ��� ����� � eeprom �� ���������� ������ */
void eeprom_write_word(unsigned int addr, unsigned int data)
{
	eeprom_write_byte(addr, (data >> 8) & 0xFF);
	eeprom_write_byte(addr + 1, data & 0xFF);
}


/* ������ ���������/���������� �� eeprom � ���������� ���������� */
void eeprom_read_settings(void)
{
	if (is_eeprom_valid()) {
		// ���� eeprom � ������� - ������ ������
		SPISettings = eeprom_read_word(SPI_SETTINGS_EEPROM_ADDR);
		UARTBaudRate = eeprom_read_word(UART_BAUDRATE_EEPROM_ADDR);
		HallZero = eeprom_read_word(HALL_ZERO_EEPROM_ADDR);
	} else {
		// ����� � eeprom ������ �� ��������� (�� ���� ���� ���������)
		SPISettings = DEFAULT_SPI_SETTINGS;
		UARTBaudRate = DEFAULT_UART_SETTINGS;
		HallZero = DEFAULT_HALL_ZERO;
		eeprom_write_word(SPI_SETTINGS_EEPROM_ADDR, SPISettings);
		eeprom_write_word(UART_BAUDRATE_EEPROM_ADDR, UARTBaudRate);
		eeprom_write_word(HALL_ZERO_EEPROM_ADDR, HallZero);
		eeprom_update();
	}
	
	// ����������, ��������� �� ����������� �� eeprom
	RecentADC = HallZero;
	SwitchENAmperage = HallZero + AC_SWITCH_AMPERAGE * 2;
	SwitchEPAmperage = HallZero - AC_SWITCH_AMPERAGE * 2;
}


/* �������� ���������� ������ � eeprom (��������� � ����������� �����) */
/* ���� ������������ false, �� eeprom �� ��������������� ��� ��������� */
bool is_eeprom_valid(void)
{
	unsigned int chksum = 0;
	unsigned int echksum = eeprom_read_word(EEPROM_DATA_SIZE);
	unsigned char temp = 0;
	for (int i=0; i<EEPROM_DATA_SIZE; i++) {
		temp = eeprom_read_byte(i);
		if (i == 0 && temp != 0xAA) return false;
		if (i == 1 && temp != 0x99) return false;
		chksum += temp * 211;
	}
	
	return chksum == echksum;
}


/* ������ ���������, ������ � ������ ����������� ����� eeprom */
void eeprom_update(void)
{
	unsigned int chksum = 0;
	unsigned char temp = 0;
	eeprom_write_byte(0x00, 0xAA);
	eeprom_write_byte(0x01, 0x99);
	
	for (int i=0; i<EEPROM_DATA_SIZE; i++) {
		temp = eeprom_read_byte(i);
		chksum += temp * 211;
	}

	eeprom_write_word(EEPROM_DATA_SIZE, chksum);
}