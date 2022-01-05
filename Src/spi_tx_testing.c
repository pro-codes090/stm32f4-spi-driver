/*
 * spi_tx_testing.c
 *
 *  Created on: 03-Nov-2021
 *      Author: pro
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stm32f407xx.h"
#include "stm32f407xx_gpio_driver.h"
#include "stm32f407xx_spi_driver.h"

void SPI2_GPIOInits(void)
{
	GPIO_Handle_t SPIPins;

	SPIPins.pGPIOx = GPIOB;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
	SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPUPDControl = GPIO_NO_PUPD;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;

	//SCLK
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPIPins);

	//MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPIPins);

	//MISO
	//SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	//GPIO_Init(&SPIPins);


	//NSS
	//SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	//GPIO_Init(&SPIPins);


}

void SPI2_Inits(void)
{

	SPI_Handle_t SPI2handle;

	SPI2handle.pSPIx = SPI2;
	SPI2handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI2handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV2;//generates sclk of 8MHz
	SPI2handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2handle.SPIConfig.SPI_CPOL = SPI_CPOL_HIGH;
	SPI2handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2handle.SPIConfig.SPI_SSM = SPI_SSM_EN; //software slave management enabled for NSS pin

	SPI_Init(&SPI2handle);
}


int main (void ){


	char user_data[] = "Hello world";

	//this function is used to initialize the GPIO pins to behave as SPI2 pins
	SPI2_GPIOInits();

	//This function is used to initialize the SPI2 peripheral parameters
	SPI2_Inits();

	//spi ssi config
	SPI_SSIConfig(SPI2, ENABLE) ;

	// enable the spi peripheral
	SPI_PeripheralControl(SPI2, ENABLE) ;

	// send data over spi
	SPI_Send(SPI2, (uint8_t*)user_data, strlen(user_data)) ;

	//close the communication by disabling the peripherals
//	SPI_PeripheralControl(SPI2, DISABLE) ;

	while(1);



	return 0;
}


