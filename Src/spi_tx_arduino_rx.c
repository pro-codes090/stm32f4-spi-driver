/*
 * spi_tx_arduino_rx.c
 *
 *  Created on: 04-Nov-2021
 *      Author: pro
 */


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stm32f407xx.h"
#include "stm32f407xx_gpio_driver.h"
#include "stm32f407xx_spi_driver.h"


// pb15as mosi
	// pb 14 as miso
	// pb13 as sclk
	// pb12 as nss
	// Alt function mode : 5



void SPI2_GPIOInits(void){

	GPIO_Handle_t SPIPins ;
	memset(&SPIPins , 0 , sizeof(SPIPins)) ; // its for safety
	SPIPins.pGPIOx = GPIOB ;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN ;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5 ;
	SPIPins.GPIO_PinConfig.GPIO_PinOPType= GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPUPDControl = GPIO_NO_PUPD ;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH ;


	// using chip select pin (NSS) in software mannaged mode

	// pin for sclk pin
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13 ;
	GPIO_Init(&SPIPins) ;

	// pin for MISO  pin
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14 ;
	GPIO_Init(&SPIPins) ;

	// pin for MOSI pin
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15 ;
	GPIO_Init(&SPIPins) ;

	// pin for NSS pin
	SPIPins.pGPIOx = GPIOB ;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUTPUT;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5 ;
	SPIPins.GPIO_PinConfig.GPIO_PinOPType= GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPUPDControl = GPIO_NO_PUPD ;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH ;
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12 ;
	GPIO_Init(&SPIPins) ;
}


void SPI2_Inits(void) {

	SPI_Handle_t SPI2handle ;
	memset(&SPI2handle , 0 , sizeof(SPI2handle)) ; // its for safety
	SPI2handle.pSPIx = SPI2 ;
	SPI2handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD ; // dual side communication
	SPI2handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER ; // i am the master
	SPI2handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV32 ; // sclk of 8  megahertz
	SPI2handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS ; // data frame buffer of 8bits
	SPI2handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW ; // ideal clock state is 0
	SPI2handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW ; // sampling data at the rising edge
	SPI2handle.SPIConfig.SPI_SSM = SPI_SSM_DI ;  // hardware slave management is enabled ;

	SPI_Init(&SPI2handle) ; // initilize spi2 communication with the above specfication

}


void gpio_button_init(void) {
	GPIO_Handle_t Gpiobutton ;

	Gpiobutton.pGPIOx = GPIOA ;
	Gpiobutton.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0 ;
	Gpiobutton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_INPUT ;
	Gpiobutton.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST ;
	Gpiobutton.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	Gpiobutton.GPIO_PinConfig.GPIO_PinPUPDControl = GPIO_NO_PUPD;

    GPIO_Init(&Gpiobutton);
}





int main(void) {

	uint8_t userData[] = "hello WEOef;lkewyrlwrewhruiwu " ;

	// this function is used to initialize gpio pins for spi communication
	SPI2_GPIOInits() ;

	// nss pin pulled to high by default
	GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_12, SET) ;

	// this function initializes spi2 peripheral based on specification
	SPI2_Inits() ;

	// all the configurations must be done before enabling the spi peripheral
	// the spe but of cr1 register must be set in order to enable spi peripheral
	// spi peripheral has to be enabled explicitly by th programmer
	// all the configurations must be done before setting spe bit
	/** once spi peripheral is busy in communication it will
	 * not accept any changes to control register
	 *
	 * in order to disable the spi peripheral make sure that the tx buffer is empty
	 * and busy flag is not set which means its is 0 and then
	 * change spe but to 0 in order to disable the spi peripheral
	 *
	 * when spe = 1 NSS is automatically pulled low
	 * in order for the above thing to work ssoe bit has to be enabled
	 * so that it can work in accordance with the spe bit
	*/

	/**
	 * ssi bit must be configured when using software slave mannagment
	 * if ssm is used and ssi is not configured it causes multiple
	 * errors affecting bits like SPE bit of cr1 and totally shuts down the
	 * communication
	 * ssi bit can influence the nss state of the master
	 * */

//	SPI_SSIConfig(SPI2 , ENABLE) ; // this makes nss signal internally high and avoids modf error


	// enable the ssoe for the hardware slave mannagment
	SPI_SSOEConfig(SPI2, ENABLE) ;
    gpio_button_init() ;


    while(1){


          while (! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0)) ;

       for (int i = 0; i  < 50000; i++) {

	    }

        SPI_PeripheralControl(SPI2, ENABLE ) ; // enable the spi2 peripheral

        uint8_t dataLen = strlen((char*)userData) ;

        // nss pin pulled to low by communication initiated
    	GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_12, RESET) ;

        SPI_Send(SPI2 , &dataLen, 1) ; // send the length information

        // send the main data
        SPI_Send(SPI2 , userData, strlen((char*)userData)) ;


    		while(SPI_GetFlagStatus(SPI2, SPI_BSY_FLAG)){

    		}

    	SPI_PeripheralControl(SPI2, DISABLE); // disabling the peripheral

    	// nss pin pulled to high to mark the end of the communication
    	GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_12, SET) ;


    }


	return 0 ;


}

