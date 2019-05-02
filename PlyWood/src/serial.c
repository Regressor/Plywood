/* ������ � UART/SPI */
#include <asf.h>

/* �������� ������ �� ������������������� ����� (UART ��� SPI) */
/* ������ ������ ������������� �������� 0x0d (13) */
void serial_receive_line() {
	unsigned char idx = 0;
	unsigned char next = 0;

	while(1) {
		if (ioport_get_pin_level(SPI_SELECT)) {
			// ���������� ������ ����� SPI
			while (!(SPSR & (1 << SPIF)));
			next = SPDR;
		} else {
			// ���������� ������ ����� UART
			while (!(UCSRA & (1 << RXC)));
			next = UDR;
		}
		
		if ((idx + 1) >= MAX_COMMAND_LEN || next == 0xd) {
			CmdBuffer[idx] = 0;
			break;
		}
				
		CmdBuffer[idx++] = next;
	}
}


/* ���������� ������ � ������������������ ���� (UART ��� SPI) */
/* ������ ������������� �������� 0x0d (13) */
void serial_send_line(char *line) {

	for (unsigned char i = 0; i < MAX_COMMAND_LEN; i++) {
		
		if (ioport_get_pin_level(SPI_SELECT)) {
			// ���������� ������ ����� SPI
			// Slave �� ����� ����� ������. ������ ������������ � 
			// ��������. ������� ������ ����� �������� ������� ������ 
			// ��������� N ���� ������ ����������� �� slave � ������
			// �������� �����. ����� ������ ������ �������������.
			SPDR = line[i];
			while (!(SPSR & (1 << SPIF)));
		} else {
			// ���������� ������ ����� UART
			while (!( UCSRA & (1 << UDRE)));
			UDR = line[i];
		}
		
		// ����������� �� �������� ������ (��� ����)
		if (line[i] == 0xd || line[i] == 0) return;
	}
}


/* ����� ���������� ����� �������, �� ������������ ��������� 
��������� �� ��������� */
void serial_send_answer(char *line) {
	serial_send_line(line);
}

