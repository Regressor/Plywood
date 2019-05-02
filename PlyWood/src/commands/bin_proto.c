/* �������� ��������. ���� ����� ������� ��������� ������    */
/* �� ��������� � ��������� ����������. ��� ���� ����������� */
/* ����� ������������ ������. �� ��������� � ���������       */
/* ���������� ������������� ������������ ����������� �����   */

#include <asf.h>
#ifdef BINARY_PROTOCOL

/* ������ 8������ ����������� ����� ���������� ������ */
NOINLINE unsigned char chksum(unsigned char *data) {
	unsigned int sum = 0;
	for (unsigned char i = 0; i < 8; i++) {
		sum = sum + (data[i] * 211);
		sum = sum ^ (sum >> 8);
	}
	
	return (sum & 0xFF);
}

/* ����� � ��������� ������ ������������. ������� ������. */
void main_parser(void) {
	/* TODO: ���� �� ����������� */
}

#endif /* BINARY_PROTOCOL */