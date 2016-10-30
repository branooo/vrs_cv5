/*
 * vrs_cv5.c
 *
 *  Created on: 21. 10. 2016
 *      Author: Branis
 */
#include "vrs_cv5.h"

extern uint16_t AnalogovaHodnota;
extern uint8_t i,format,sprava[7];

void blikanieLED(void){
	int i,j;
	if (AnalogovaHodnota<10){
	GPIO_ToggleBits(GPIOA, GPIO_Pin_5);
			for (i = 1; i <= 20; i++){
				for (j = 1; j <= 20; j++){
				}
			}
	}
	if ((AnalogovaHodnota>10)&&(AnalogovaHodnota<800)){
		GPIO_ToggleBits(GPIOA, GPIO_Pin_5);
				for (i = 1; i <= 60; i++){
					for (j = 1; j <= 60; j++){
					}
				}
	}
	if ((AnalogovaHodnota>801)&&(AnalogovaHodnota<1600)){
			GPIO_ToggleBits(GPIOA, GPIO_Pin_5);
					for (i = 1; i <= 100; i++){
						for (j = 1; j <= 100; j++){
						}
					}
	}
	if ((AnalogovaHodnota>1601)&&(AnalogovaHodnota<2400)){
				GPIO_ToggleBits(GPIOA, GPIO_Pin_5);
						for (i = 1; i <= 140; i++){
							for (j = 1; j <= 140; j++){
							}
						}
	}
	if ((AnalogovaHodnota>2401)&&(AnalogovaHodnota<3200)){
				GPIO_ToggleBits(GPIOA, GPIO_Pin_5);
						for (i = 1; i <= 200; i++){
							for (j = 1; j <= 200; j++){
							}
						}
	}
	if (AnalogovaHodnota>3201){
		GPIO_ToggleBits(GPIOA, GPIO_Pin_5);
				for (i = 1; i <= 240; i++){
					for (j = 1; j <= 240; j++){
					}
				}
	}
}

void ADC1_IRQHandler (void){
	if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
	{
		AnalogovaHodnota = ADC_GetConversionValue(ADC1);
	}
}

void USART2_IRQHandler(void){
	uint16_t pomocnaHodnota;
	uint16_t prijataHodnota;
			if (USART_GetFlagStatus(USART2, USART_FLAG_TC)){
				if (format){
					if (i == 0){
						pomocnaHodnota = AnalogovaHodnota*1000/4095*33;
						sprava[0] = (pomocnaHodnota/10000) + 48;
						sprava[1] = 0x2E;
						sprava[2] = (pomocnaHodnota/1000)%10 + 48;
						sprava[3] = (pomocnaHodnota/100)%10 + 48;
						sprava[4] = 0x56;
						sprava[5] = '\n';
						sprava[6] = '\r';
					}
					USART_SendData(USART2, sprava[i]);
					i++;
					if (i >= 7)	i = 0;
				}
				else{
					if (i == 0){
						pomocnaHodnota = AnalogovaHodnota;
						sprava[0] = (pomocnaHodnota/1000) + 48;
						sprava[1] = (pomocnaHodnota/100)%10 + 48;
						sprava[2] = (pomocnaHodnota/10)%10 + 48;
						sprava[3] = pomocnaHodnota%10 + 48;
						sprava[4] = '\n';
						sprava[5] = '\r';
					}
					USART_SendData(USART2, sprava[i]);
					i++;
					if (i >= 6)	i=0;
				}
			}


			if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE)){
				prijataHodnota = USART_ReceiveData(USART2);
			 	if (prijataHodnota == 'm'){
			 		format ^= 1;
			 		i = 0;
			 	}
			}
}

void inicializaciaNVIC(void){
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 16;	//adc ma vyssiu prioritu ako uasrt
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	ADC_ITConfig(ADC1,ADC_IT_EOC, ENABLE);
	ADC_ITConfig(ADC1,ADC_IT_OVR, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//rx prerusenie
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);	//tx prerusenie
}

void inicializaciaUSART(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600; //realne bude fungovat na 4800
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2, ENABLE);	//zapnutie usartu
}

void inicializaciaGPIO(void){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitTypeDef	GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	RCC_HSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
	GPIO_InitTypeDef gpioInitStruct;
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF;
	gpioInitStruct.GPIO_OType = GPIO_OType_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_40MHz;
	gpioInitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &gpioInitStruct);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
}

void inicializaciaADC(){
	ADC_InitTypeDef	ADC_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_StructInit(&ADC_InitStruct);
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStruct);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_384Cycles);
	ADC_Cmd(ADC1, ENABLE);
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET){}
	ADC_SoftwareStartConv(ADC1);
}
