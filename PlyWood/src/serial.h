#ifndef SERIAL_H_
#define SERIAL_H_

void serial_receive_line(void);
void serial_send_line(char *line);
void serial_send_answer(char *line);

#endif /* SERIAL_H_ */