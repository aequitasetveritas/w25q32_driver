#include "memFlash.h"
#include "spi_g2955.h"
#include <msp430.h>


static void instruccion_simple(const SPI_transfer_type * config,uint8_t codigo)
{
	USCI_SPI_transfer(config,&codigo,1,&codigo,0);
}

static void instruccion_simple_dir(uint32_t direccion,uint8_t codigo){
//	uint8_t * dir_byte = (uint8_t *) &direccion;
	uint8_t dir_byte[3];
	dir_byte[0] = (uint8_t)(direccion & 0x000000ff);
	dir_byte[1] = (uint8_t)((direccion & 0x0000ff00) >> 8);
	dir_byte[2] = (uint8_t)((direccion & 0x00ff0000) >> 16);

	MEM_HABILITAR_CS();
	spi_transferir(codigo);
	spi_transferir(dir_byte[2]);
	spi_transferir(dir_byte[1]);
	spi_transferir(dir_byte[0]);
	MEM_DESHABILITAR_CS();
}

void W25Q32B_init(const W25Q32B_config_type * config)
{
	// PxDIR as Output
	*((config->W25Q32B_HOLD_PORT_OUT)+1) |= (config->W25Q32B_HOLD_PIN);
	*((config->W25Q32B_WP_PORT_OUT)+1) |= (config->W25Q32B_WP_PIN);
	// PxOUT High
	*(config->W25Q32B_HOLD_PORT_OUT) |= (config->W25Q32B_HOLD_PIN);
	*(config->W25Q32B_WP_PORT_OUT) |= (config->W25Q32B_WP_PIN);

	// Initialize the CS pin for the memory.
	USCI_SPI_CS_pin_init(config->spi_config);
}

void W25Q32B_jedecID(const W25Q32B_config_type * config,uint8_t * response)
{
	response[0] = 0x9F;
	USCI_SPI_transfer(config->spi_config,response,1,response,3);
}

uint8_t W25Q32B_read_status_register_1(const W25Q32B_config_type * config)
{
	uint8_t status[2];
	status[0] = 0x05;
	USCI_SPI_transfer(config->spi_config,status,1,(status + 1),1);
	return status[1];
}

uint8_t W25Q32B_read_status_register_2(const W25Q32B_config_type * config)
{
	/*uint8_t status;
	status = 0x35;
	USCI_SPI_transfer(config->spi_config,&status,1,&status,1);
	*/return 0x00;
}

void W25Q32B_write_enable(const W25Q32B_config_type * config)
{
	instruccion_simple(config->spi_config,0x06);
}

void W25Q32B_write_disable(const W25Q32B_config_type * config)
{
	instruccion_simple(config->spi_config,0x04);
}

void W25Q32B_chip_erase(const W25Q32B_config_type * config)
{
	instruccion_simple(config->spi_config,0xC7);
}


void W25Q32B_page_program(const W25Q32B_config_type * config, uint32_t address, uint8_t * buffer, uint16_t num_bytes)
{
	uint8_t command[4];
	command[0] = 0x02;
	command[1] = (uint8_t)((address & 0x00ff0000) >> 16); //Byte MAS Significativo
	command[2] = (uint8_t)((address & 0x0000ff00) >> 8);
	command[3] = (uint8_t)(address & 0x000000ff); 		//Byte Menos Significativo
	USCI_SPI_transfer(config->spi_config,command,4,buffer,num_bytes);
}
void W25Q32B_read_data(const W25Q32B_config_type * config, uint32_t address, uint8_t * buffer, uint16_t num_bytes)
{
	uint8_t command[4];
	command[0] = 0x03;
	command[1] = (uint8_t)((address & 0x00ff0000) >> 16); //Byte MAS Significativo
	command[2] = (uint8_t)((address & 0x0000ff00) >> 8);
	command[3] = (uint8_t)(address & 0x000000ff); 		//Byte Menos Significativo
	USCI_SPI_transfer(config->spi_config,command,4,buffer,num_bytes);
}
void W25Q32B_sector_erase(const W25Q32B_config_type * config, uint32_t address)
{
	uint8_t command[4];
	command[0] = 0x20;
	command[1] = (uint8_t)((address & 0x00ff0000) >> 16); //Byte MAS Significativo
	command[2] = (uint8_t)((address & 0x0000ff00) >> 8);
	command[3] = (uint8_t)(address & 0x000000ff); 		//Byte Menos Significativo
	USCI_SPI_transfer(config->spi_config,command,4,command,0);
}

uint8_t W25Q32B_busy(const W25Q32B_config_type * config)
{
	return (W25Q32B_read_status_register_1(config) & 0x01);
}
void page_program(uint32_t direccion, uint8_t * buff, uint16_t n){
//	uint8_t * dir_byte = (uint8_t *) &direccion;

	uint8_t dir_byte[3];
	dir_byte[0] = (uint8_t)(direccion & 0x000000ff); 		//Byte Menos Significativo
	dir_byte[1] = (uint8_t)((direccion & 0x0000ff00) >> 8);
	dir_byte[2] = (uint8_t)((direccion & 0x00ff0000) >> 16); //Byte MAS Significativo

	MEM_HABILITAR_CS();
	spi_transferir(0x02);
	spi_transferir(dir_byte[2]);		//MSB al principio
	spi_transferir(dir_byte[1]);
	spi_transferir(dir_byte[0]);
	spi_transferir_multi(buff,n);
	MEM_DESHABILITAR_CS();
}

void sector_erase(uint32_t direccion){
	instruccion_simple_dir(direccion,0x20);
}

void block_erase32(uint32_t direccion){
	instruccion_simple_dir(direccion,0x52);
}

void block_erase64(uint32_t direccion){
	instruccion_simple_dir(direccion,0xD8);
}



uint8_t read_status_register_1(){
	MEM_HABILITAR_CS();
	uint8_t status = spi_transferir(0x05);
	MEM_DESHABILITAR_CS();
	return status;
}

uint8_t read_status_register_2(){
	MEM_HABILITAR_CS();
	uint8_t status = spi_transferir(0x35);
	MEM_DESHABILITAR_CS();
	return status;
}

void read_data(uint32_t direccion, uint8_t * buff, uint16_t n){
//	uint8_t * dir_byte = (uint8_t *) &direccion;
	uint8_t dir_byte[3];
	dir_byte[0] = (uint8_t)(direccion & 0x000000ff);
	dir_byte[1] = (uint8_t)((direccion & 0x0000ff00) >> 8);
	dir_byte[2] = (uint8_t)((direccion & 0x00ff0000) >> 16);

	MEM_HABILITAR_CS();
	spi_transferir(0x03);
	spi_transferir(dir_byte[2]);
	spi_transferir(dir_byte[1]);
	spi_transferir(dir_byte[0]);
	spi_recibir_multi(buff,n);
	MEM_DESHABILITAR_CS();
}

/*JedecID es una instrucción para identificar a la memoria segun
el estandar JEDEC. Devuelve 3 bytes: Codigo de fabricante, tipo de
memoria y capacidad. El código de fabricante para Windbond es 0xEF.*/

uint8_t JedecID(uint8_t * buff){
	MEM_HABILITAR_CS();
	spi_transferir(0x9F);  					//Codigo de instrucción 9F
	spi_recibir_multi(buff,3);
	MEM_DESHABILITAR_CS();

	if(buff[0]==0xFF){return 1;}			//Falla en la lectura
	return 0;
}

