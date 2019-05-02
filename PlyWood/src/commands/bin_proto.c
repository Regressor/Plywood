/* Бинарный протокол. Дает более быструю обработку команд    */
/* по сравнению с текстовым протоколом. При этом сокращается */
/* объем передаваемых данных. По сравнению с текстовым       */
/* протоколом дополнительно используется контрольная сумма   */

#include <asf.h>
#ifdef BINARY_PROTOCOL

/* Расчет 8битной контрольной суммы командного пакета */
NOINLINE unsigned char chksum(unsigned char *data) {
	unsigned int sum = 0;
	for (unsigned char i = 0; i < 8; i++) {
		sum = sum + (data[i] * 211);
		sum = sum ^ (sum >> 8);
	}
	
	return (sum & 0xFF);
}

/* Прием и обработка команд пользователя. Главная секция. */
void main_parser(void) {
	/* TODO: пока не реализовано */
}

#endif /* BINARY_PROTOCOL */