#ifndef INIT_H_
#define INIT_H_

#include <asf.h>
#include <conf_board.h>

extern void board_init(void);
extern void timers_setup(void);

/* Возвращает true если сработал датчик перегрева */
#define is_overheat() (ioport_get_pin_level(OVRTMP_SENS))

/* Возвращает true если необходимо включить охлаждение */
#define is_fan_needed() (ioport_get_pin_level(FAN_SENS) || ioport_get_pin_level(OVRTMP_SENS) || FanManualSwitch)

/* Включает или выключает охлаждение */
#define fan_switch(x) ioport_set_pin_level(FAN_SWITCH, x); fanState = x;

/* Возвращает true аппаратно сконфигурирован интерфейс SPI */
#define is_spi_needed() (ioport_get_pin_level(SPI_SELECT))

		
#endif /* INIT_H_ */