#include "stm32f4_discovery.h"

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_dma.h"
#include "misc.h"

#include "sys.h"
#include "gdi.h"
#include "video.h"
#include "baseSoftware.h"

void RCC_Configuration(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_SPI1, ENABLE);

#ifdef DEBUG
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
#endif
}

int main(void)
{
	SystemInit();
	RCC_Configuration();

	vidInit();
	sysInitSystemTimer();

	initProgram();

	while (1)
		__WFI();
}
