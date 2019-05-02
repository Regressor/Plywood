/* Работа с UART/SPI */
#include <asf.h>

/* Получает строку из сконфигурированного порта (UART или SPI) */
/* строка должна заканчиваться символом 0x0d (13) */
void serial_receive_line() {
	unsigned char idx = 0;
	unsigned char next = 0;

	while(1) {
		if (ioport_get_pin_level(SPI_SELECT)) {
			// Перемычкой выбран режим SPI
			while (!(SPSR & (1 << SPIF)));
			next = SPDR;
		} else {
			// Перемычкой выбран режим UART
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


/* Отправляет строку в сконфигурированный порт (UART или SPI) */
/* строка заканчивается символом 0x0d (13) */
void serial_send_line(char *line) {

	for (unsigned char i = 0; i < MAX_COMMAND_LEN; i++) {
		
		if (ioport_get_pin_level(SPI_SELECT)) {
			// Перемычкой выбран режим SPI
			// Slave не может слать данные. Только обмениваться с 
			// мастером. Поэтому мастер после отправки команды должен 
			// отправить N байт любого содержимого на slave и взамен
			// получить ответ. Лучше размер ответа зафиксировать.
			SPDR = line[i];
			while (!(SPSR & (1 << SPIF)));
		} else {
			// Перемычкой выбран режим UART
			while (!( UCSRA & (1 << UDRE)));
			UDR = line[i];
		}
		
		// Заканчиваем на переносе строки (или нуле)
		if (line[i] == 0xd || line[i] == 0) return;
	}
}


/* Хотел дополнение здесь сделать, но используемые строковые 
константы не позволяют */
void serial_send_answer(char *line) {
	serial_send_line(line);
}

