#ifndef memFlash_H
#define memFlash_H
#include <stdint.h>
#include "spi_g2955.h"

/*Todos las funciones remiten a una instrucción de la memoria*/
typedef struct
{
	volatile uint8_t * W25Q32B_HOLD_PORT_OUT;
	uint8_t W25Q32B_HOLD_PIN;
	volatile uint8_t * W25Q32B_WP_PORT_OUT;
	uint8_t W25Q32B_WP_PIN;
	const SPI_transfer_type * spi_config;
} W25Q32B_config_type;

void W25Q32B_init(const W25Q32B_config_type * config);
void W25Q32B_jedecID(const W25Q32B_config_type * config,uint8_t * response);

void W25Q32B_write_enable(const W25Q32B_config_type * config);
void W25Q32B_write_disable(const W25Q32B_config_type * config);
void W25Q32B_chip_erase(const W25Q32B_config_type * config);
void W25Q32B_sector_erase(const W25Q32B_config_type * config, uint32_t address);
void W25Q32B_page_program(const W25Q32B_config_type * config, uint32_t address, uint8_t * buffer, uint16_t num_bytes);
void W25Q32B_read_data(const W25Q32B_config_type * config, uint32_t address, uint8_t * buffer, uint16_t num_bytes);
uint8_t W25Q32B_busy(const W25Q32B_config_type * config);

uint8_t W25Q32B_read_status_register_1(const W25Q32B_config_type * config);
uint8_t W25Q32B_read_status_register_2(const W25Q32B_config_type * config);


void write_enable();
void write_disable();
void page_program(uint32_t direccion, uint8_t * buff, uint16_t n);
void sector_erase(uint32_t direccion);
void block_erase32(uint32_t direccion);
void block_erase64(uint32_t direccion);
void chip_erase();
uint8_t read_status_register_1();
uint8_t read_status_register_2();
void read_data(uint32_t direccion, uint8_t * buff, uint16_t n);
uint8_t JedecID(uint8_t * buff);


#endif
