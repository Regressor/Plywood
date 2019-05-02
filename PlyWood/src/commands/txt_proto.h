/* Created: 02.12.2017 23:06:56 */ 

#ifndef BINARY_PROTOCOL

#ifndef TXT_PROTO_H_
#define TXT_PROTO_H_

/* Текстовые константы протокола Сэма */
#define INVALID_CMD		"?InvalidCommand\r"		// Ответ "некорректная команда"
#define OVERTEMP_CMD	"T              \r"		// Ответ "перегрев"
#define OVERCURR_CMD	"F              \r"		// Ответ "перегрузка по току"
#define OK_CMD			"O              \r"		// Ответ "ОК"

extern void main_parser(void);
extern void cmd_mma_section(void);
extern void cmd_dctig_section(void);
extern void cmd_actig_section(void);
extern void cmd_cal_section(void);

extern void serial_send_log(void);
extern void serial_send_dump(void);
extern bool is_psu_in_error_state(void);
extern bool is_num_str(char *str);
	
#endif /* TXT_PROTO_H_ */

#endif