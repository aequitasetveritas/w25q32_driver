/*
 * spi_g2955.c
 *
 *  Created on: Oct 16, 2014
 *      Author: escualo
 */

#include "spi_g2955.h"
#include <msp430.h>

static void USCI_SPI_pins_init(SPI_channel_name_type name)
{
	switch(name){
		case USCI_A0:
			break;
		case USCI_B0:
			//Config Pins P3.1 (SIMO) P3.2 (SOMI) P3.3 (SCLK)
			P3SEL |= BIT1 | BIT2 | BIT3;
			P3SEL2 &= ~(BIT1 | BIT2 | BIT3);
			break;
		default:
			break;
	}
}

void USCI_SPI_CS_pin_init(const SPI_transfer_type * config)
{
	switch(config->cs_port){
		case P1:
			P1DIR |= config->cs_pin;
			P1OUT |= config->cs_pin;
			break;
		case P2:
			P2DIR |= config->cs_pin;
			P2OUT |= config->cs_pin;
			break;
		case P3:
			P3DIR |= config->cs_pin;
			P3OUT |= config->cs_pin;
			break;
		case P4:
			P4DIR |= config->cs_pin;
			P4OUT |= config->cs_pin;
			break;
		default:
			break;
	}
}

static SPI_CS_set(const SPI_transfer_type * config){
	switch(config->cs_port){
			case P1:
				P1OUT &= ~config->cs_pin;
				break;
			case P2:
				P2OUT &= ~config->cs_pin;
				break;
			case P3:
				P3OUT &= ~config->cs_pin;
				break;
			case P4:
				P4OUT &= ~config->cs_pin;
				break;
			default:
				break;
		}
}

static SPI_CS_reset(const SPI_transfer_type * config){
	switch(config->cs_port){
			case P1:
				P1OUT |= config->cs_pin;
				break;
			case P2:
				P2OUT |= config->cs_pin;
				break;
			case P3:
				P3OUT |= config->cs_pin;
				break;
			case P4:
				P4OUT |= config->cs_pin;
				break;
			default:
				break;
		}
}


void USCI_SPI_init(const SPI_config_type * config){
	volatile uint8_t * CTL0;
	volatile uint8_t * CTL1;
	volatile uint8_t * BR0;
	volatile uint8_t * BR1;

	switch(config->name){
		case USCI_B0:
			CTL0 = &UCB0CTL0;
			CTL1 = &UCB0CTL1;
			BR0 = &UCB0BR0;
			BR1 = &UCB0BR1;

			//Config Pins P3.1 (SIMO) P3.2 (SP4OMI) P3.3 (SCLK)
			USCI_SPI_pins_init(USCI_B0);

			break;
		case USCI_A0:
			CTL0 = &UCA0CTL0;
			CTL1 = &UCA0CTL1;
			BR0 = &UCA0BR0;
			BR1 = &UCA0BR1;
			break;
		default:
			return;
	}

	*CTL1 |= UCSWRST;
	*CTL0 = 0x00;
	*CTL0 |= ((config->phase)==DATA_CAPTURED_ON_FIRST_EDGE) ? UCCKPH : 0x00;
	*CTL0 |= ((config->polarity)==INACTIVE_HIGH) ? UCCKPL : 0x00;
	*CTL0 |= ((config->direction)==MSB_FIRST) ? UCMSB : 0x00;
	*CTL0 |= ((config->length)==SEVEN_BIT) ? UC7BIT : 0x00;
	*CTL0 |= ((config->mode)==SPI_MASTER) ? UCMST : 0x00;
	*CTL0 |= ((config->USCI_mode) << 1);
	*CTL0 |= UCSYNC;

	*CTL1 |= ((config->clock) << 6);

	*BR0 = (uint8_t) ((config->prescaler) & 0x00ff);
	*BR1 = (uint8_t) (((config->prescaler)>>8) & 0x00ff);

	uint8_t interrupt_bits_offset = 0;
	interrupt_bits_offset = ((config->name) == USCI_B0) ? 2 : 0;
	switch((config->interrupt_enable)){
	case SPI_TX_INTERRUPT_ENABLED:
		IE2 |=  (0x02) << interrupt_bits_offset;
		break;
	case SPI_RX_INTERRUPT_ENABLED:
		IE2 |=  (0x01) << interrupt_bits_offset;
		break;
	case SPI_TXRX_INTERRUPT_ENABLED:
		IE2 |= (0x03) << interrupt_bits_offset;
		break;
	default:
		IE2 &= ~(0x03 << interrupt_bits_offset);
		break;
	}

	*CTL1 &= ((config->enabled) == SPI_ENABLED) ? ~UCSWRST : 0xff;
}

/* USCI_SPI_transfer transmit tx_num_bytes bytes from tx_buffer, then sends rx_num_bytes from rx_buffer and overwrites its contents with the
 * response from the device.
*/
void USCI_SPI_transfer(const SPI_transfer_type * config,uint8_t * tx_buffer, uint16_t tx_num_bytes,uint8_t * rx_buffer, uint16_t rx_num_bytes)
{
	//volatile uint8_t * STAT;
	switch(config->name){
			case USCI_B0:
				//STAT = &UCB0STAT;
				SPI_CS_set(config);

				uint16_t i;
				uint8_t dummy;
				for(i=0;i<(tx_num_bytes);i++){
					while ((IFG2 & UCB0TXIFG) == 0); // Esperar a que se libere el buffer de transmisión
					UCB0TXBUF = (tx_buffer)[i];      // Escribir
					while ((IFG2 & UCB0RXIFG) == 0); // Esperar a que se llene el buffer RX
					dummy = UCB0RXBUF;
				}
				for(i=0;i<(rx_num_bytes);i++){
					while ((IFG2 & UCB0TXIFG) == 0); // Esperar a que se libere el buffer de transmisión
					UCB0TXBUF = (rx_buffer)[i];      // Escribir
					while ((IFG2 & UCB0RXIFG) == 0); // Esperar a que se llene el buffer RX
					(rx_buffer)[i] = UCB0RXBUF;
				}

				SPI_CS_reset(config);

				break;
			case USCI_A0:
				//STAT = &UCA0STAT;

				break;
			default:
				return;
	}
}


void spi_inicializar(){
	P3SEL |= BIT1 | BIT2 | BIT3;
	P3SEL2 &= ~(BIT1 | BIT2 | BIT3);
	UCB0CTL1 |= UCSWRST; /* USCI-B SPI reset */
	UCB0CTL0 = UCCKPH | UCMSB | UCMST | UCMODE_0 | UCSYNC; // SPI mode 0, master
	UCB0BR0 = 0x28; // 400kHz
	UCB0BR1 = 0x00;
	UCB0CTL1 = UCSSEL_2; // Clock = SMCLK, limpiar UCSWRST y habilitar modulo USCI_B.
	P4DIR |= BIT0 | BIT1; // P3.0 output direction
}

uint8_t spi_transferir(uint8_t dat){
	uint8_t dummy;
	    while ((IFG2 & UCB0TXIFG) == 0); // Esperar a que se libere el buffer de transmisión
	    UCB0TXBUF = dat; // Escribir
	    while ((IFG2 & UCB0RXIFG) == 0); // Esperar a que se llene el buffer RX
	    dummy = UCB0RXBUF;
	    return (dummy);
}

uint16_t spi_transferir16(uint16_t dato16){
	//QUIZAS SEA NECESARIO CAMBIAR EL ORDEN
	uint8_t msb_res = spi_transferir((uint8_t) (dato16 >> 8));
	uint8_t lsb_res = spi_transferir((uint8_t) (dato16));

	uint16_t res = msb_res << 8 | lsb_res;
	return res;
}


uint8_t spi_transferir_multi(uint8_t * datos, uint16_t n){
	uint16_t i;
	uint8_t recibido = 0;

	for(i=0;i<n;i++){
		while(!(IFG2 & UCB0TXIFG));
		UCB0TXBUF = datos[i];
		while(!(IFG2 & UCB0RXIFG));
		recibido = UCB0RXBUF;
	}

	return recibido;
}

uint8_t spi_recibir_multi(uint8_t * datos, uint16_t n){
	uint16_t i;

	for(i=0;i<n;i++){
		while(!(IFG2 & UCB0TXIFG));
		UCB0TXBUF = 0xFF;
		while(!(IFG2 & UCB0RXIFG));
		datos[i] = UCB0RXBUF;
	}

	return datos[i];
}
