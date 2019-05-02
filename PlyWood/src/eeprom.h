#ifndef EEPROM_H_
#define EEPROM_H_

unsigned char eeprom_read_byte(unsigned int addr);
unsigned int eeprom_read_word(unsigned int addr);
void eeprom_write_byte(unsigned int addr, unsigned char data);
void eeprom_write_word(unsigned int addr, unsigned int data);
void eeprom_read_settings(void);
void eeprom_update(void);
bool is_eeprom_valid(void);

#endif /* EEPROM_H_ */