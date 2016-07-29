## Resumen

Driver en C de la flash Windbond W25Q32 para el MSP430G2955 y driver para el USCI_SPI del mismo micro. 

## Code Example

	\#include "spi_g2955.h"
	\#include "memFlash.h"

	/* Configuracion general para el SPI */
	const SPI_config_type SPI_config_table[]={
		/* Memoria Flash */
		{USCI_B0,DATA_CAPTURED_ON_FIRST_EDGE,INACTIVE_LOW,MSB_FIRST,EIGHT_BIT,SPI_MASTER,THREE_PIN,SPI_SMCLK,0x01,SPI_ENABLED,DISABLED}
	};

	void main()
	{
		/* Inicializar SPI */
		USCI_SPI_init(SPI_config_table);
	
		/* Configuración de Chip Select para la memoria */
		const SPI_transfer_type SPI_devices_table[]={
		      {USCI_B0,P4,BIT0} /* Memoria Flash */
		};
	
		/* Configuración de los otros pines de control de la memoria */
		const W25Q32B_config_type flash_config_table[]={
		      /* HOLDport,HOLDpin,WPport,WPpin,SPI_config */
		      {&P4OUT,BIT1,&P4OUT,BIT2,SPI_devices_table}
		};


		W25Q32B_init(flash_config_table);
		W25Q32B_write_enable(flash_config_table);

		uint32_t address = 0x00000000;
		/* Borrado de un sector */
		W25Q32B_sector_erase(flash_config_table,address);
		...
		...
	}
