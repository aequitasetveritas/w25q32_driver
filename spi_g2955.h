/*
 * spi_g2955.h
 *
 *  Created on: Oct 16, 2014
 *      Author: escualo
 */

#ifndef SPI_G2955_H_
#define SPI_G2955_H_

#include <stdint.h>


#define MEM_CS BIT0
#define MEM_HABILITAR_CS() P4OUT&=~MEM_CS
#define MEM_DESHABILITAR_CS() P4OUT|=MEM_CS

typedef enum
{
	P1,
	P2,
	P3,
	P4
} SPI_port_type;

typedef enum
{
	USCI_A0,
	USCI_B0,
	NUM_SPI_AVAILABLE
} SPI_channel_name_type;

typedef enum
{
	DATA_CHANGED_ON_FIRST_EDGE,
	DATA_CAPTURED_ON_FIRST_EDGE
} SPI_channel_phase_type;

typedef enum
{
	INACTIVE_LOW,
	INACTIVE_HIGH
} SPI_channel_polarity_type;

typedef enum
{
	LSB_FIRST,
	MSB_FIRST
} SPI_channel_direction_type;

typedef enum
{
	EIGHT_BIT,
	SEVEN_BIT
} SPI_channel_length_type;

typedef enum
{
	SPI_SLAVE,
	SPI_MASTER
} SPI_channel_mode_type;

typedef enum
{
	THREE_PIN,
	FOUR_PIN_STE_ACTIVE_HIGH,
	FOUR_PIN_STE_ACTIVE_LOW
} SPI_channel_USCI_mode_type;

typedef enum
{
	SPI_NA,
	SPI_ACLK,
	SPI_SMCLK
} SPI_channel_clock_type;

typedef enum
{
	SPI_ENABLED,
	SPI_DISABLED
} SPI_channel_enabled_type;

typedef enum
{
	DISABLED,
	SPI_TX_INTERRUPT_ENABLED,
	SPI_RX_INTERRUPT_ENABLED,
	SPI_TXRX_INTERRUPT_ENABLED
} SPI_channel_interrupt_enable_type;

typedef struct
{
	SPI_channel_name_type name;
	SPI_channel_phase_type phase;
	SPI_channel_polarity_type polarity;
	SPI_channel_direction_type direction;
	SPI_channel_length_type length;
	SPI_channel_mode_type mode;
	SPI_channel_USCI_mode_type USCI_mode;
	SPI_channel_clock_type clock;
	uint16_t prescaler;
	SPI_channel_enabled_type enabled;
	SPI_channel_interrupt_enable_type interrupt_enable;
} SPI_config_type;



typedef struct
{
	SPI_channel_name_type name;
	SPI_port_type cs_port;
	uint8_t cs_pin;
	//uint8_t *txrx_buffer;
	//uint16_t num_bytes;
} SPI_transfer_type;

void USCI_SPI_init(const SPI_config_type * config);
void USCI_SPI_CS_pin_init(const SPI_transfer_type * config);
void USCI_SPI_transfer(const SPI_transfer_type * config, uint8_t * tx_buffer, uint16_t tx_num_bytes,uint8_t * rx_buffer, uint16_t rx_num_bytes);

void spi_inicializar();
uint8_t spi_transferir(uint8_t);  // SPI xfer 1 byte
uint16_t spi_transferir16(uint16_t);  // SPI xfer 2 bytes

uint8_t spi_transferir_multi(uint8_t * datos, uint16_t n);
uint8_t spi_recibir_multi(uint8_t * datos, uint16_t n);

#endif /* SPI_G2955_H_ */
