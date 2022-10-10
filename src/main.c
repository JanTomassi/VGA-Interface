#include "stm32f4_discovery.h"

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_dma.h"
#include "misc.h"

#include "sys.h"
#include "gdi.h"
#include "video.h"
#include "baseSoftware.h"
#include "scheduler.h"

__always_inline inline void RCC_Configuration(void);

void RCC_Configuration(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_SPI1, ENABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
}

int main(void)
{
	SystemInit();
	RCC_Configuration();

	vidInit();
	sysInitSystemTimer();

	initProgram();

	// Set the MCU to unprivileged, use the default stack pointer and disabled the FPU
	__set_CONTROL(0x1);

	while (1)
	{
		schRunTask();
		__WFI();
	}
}