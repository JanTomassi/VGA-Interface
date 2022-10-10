/**
 * @file    video.c
 * @author  Jan Tomassi
 * @version V0.0.1
 * @date    02/10/2022
 * @brief   This file implement the timer, spi and dma configuration
 */

#include "stm32f4_discovery.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_tim.h"
#include "misc.h"

#include "video.h"
/**
 * @addtogroup VGA-Interface
 * @{
 * @addtogroup Video
 * @{
 */

/**
 * @brief Define the properties of the DMA stream we will use for
 * the VGA transfers.
 */
///@{
#define VIDEO_DMA DMA2
#define DMA_STREAM DMA2_Stream3
#define DMA_CHANNEL DMA_Channel_3
#define DMA_STREAM_IRQ DMA2_Stream3_IRQn
#define DMA_IT_TCIF DMA_IT_TCIF0
#define DMA_STREAM_IRQHANDLER DMA2_Stream3_IRQHandler
///@}

/**
 * @brief The value for VTOTAL is the number of horizontal bytes to send.
 * @note Plus a small addition to act as a back porch.  Sending these extra few bytes via DMA simplifies the code.
 */
#define VTOTAL (VID_HSIZE + 2)

/**
 * @brief Frame buffer every bit is 1 pixel
 */
u8 fb[VID_VSIZE][VTOTAL] __attribute__((aligned(32))); /* Frame buffer */

static volatile u16 vline = 0; /* The current line being drawn */
volatile u32 vsync = 0;		   /* When 1, the SPI DMA request can draw on the screen */

/**
 * @brief Configure the timer for VGA horizontal and vertical sync
 */
void TIMER_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef nvic;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {
		0,
	};
	TIM_OCInitTypeDef TIM_OCInitStructure = {
		0,
	};
	u32 TimerPeriod = 0;
	u16 Channel1Pulse = 0;
	u16 Channel2Pulse = 0;
	u16 Channel3Pulse = 0;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);

	/**
	 * VGA Selected
	 * ------------
	 * @b SVGA 					800x600 @ 56 Hz
	 *
	 * @b Vertical @b refresh	35.15625 kHz
	 *
	 * @b Pixel @b freq.		36.0 MHz
	 *
	 * @b 1 @b timer @b tick 	@ 144Mhz = 6.944 ns
	 */

	/**
	 * Horizontal timing
	 * -----------------
	 *
	 * Timer 1 period = 35156 Hz
	 *
	 * Timer 1 channel 1 generates a pulse for HSYNC each 28.4 us.
	 * 28.4 us	= Visible area + Front porch + Sync pulse + Back porch.
	 * 2us / 6.944ns = 288 timer ticks.
	 *
	 * Timer 1 channel 2 generates a pulse equal to HSYNC + back porch.
	 * This interrupt will fire the DMA request to draw on the screen if vsync == 1.
	 * Since firing the DMA takes more or less 800ns, we'll add some extra time.
	 * The math for HSYNC + back porch is:
	 * (2us + 3,55us) / 6.944ns = 800-dma timer ticks
	 *
	 * Horizontal timing info
	 * ----------------------
	 *
	 * Type			|	Dots	|	us
	 * -----------: | :-------: | :------
	 * Visible area	|	800		|	22.222222222222
	 * Front porch	|	24		|	0.66666666666667
	 * Sync pulse	|	72		|	2				(500 kHz)
	 * Back porch	|	128		|	3.5555555555556	(sync+back 180 kHz)
	 * Whole line	|	1024	|	28.444444444444
	 */

	TimerPeriod = SystemCoreClock / 35156; // Horizontal line interval
	Channel1Pulse = SystemCoreClock / 500000;
	Channel2Pulse = SystemCoreClock / 180000 - 45;

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_OCStructInit(&TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Inactive;
	TIM_OCInitStructure.TIM_Pulse = Channel2Pulse;
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM1, ENABLE);

	// TIM1 counter enable and output enable
	TIM_CtrlPWMOutputs(TIM1, ENABLE);

	// Select TIM1 as Master
	TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);
	TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);

	/**
	 * Vertical timing
	 * ---------------
	 *
	 * Polarity of vertical sync pulse is positive.
	 *
	 * Type				|	Lines
	 * ----------------	| ------------
	 * Visible area		|	600
	 * Front porch		|	1
	 * Sync pulse		|	2
	 * Back porch		|	22
	 * Whole frame		|	625
	 */

	// VSYNC (TIM2_CH2) and VSYNC_BACKPORCH (TIM2_CH3)
	// Channel 2 and 3 Configuration in PWM mode
	TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Gated);
	TIM_SelectInputTrigger(TIM2, TIM_TS_ITR0);

	TimerPeriod = 625;		// Vertical lines
	Channel2Pulse = 2;		// Sync pulse
	Channel3Pulse = 24 + 2; // Sync pulse + Back porch

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_OCStructInit(&TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_Pulse = Channel2Pulse;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Inactive;
	TIM_OCInitStructure.TIM_Pulse = Channel3Pulse;
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);

	// TIM2 counter enable and output enable
	TIM_CtrlPWMOutputs(TIM2, ENABLE);

	// Interrupt TIM2
	nvic.NVIC_IRQChannel = TIM2_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&nvic);
	TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);

	// Interrupt TIM1
	nvic.NVIC_IRQChannel = TIM1_CC_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&nvic);
	TIM_ITConfig(TIM1, TIM_IT_CC2, ENABLE);

	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
}

/**
 * @brief Configure SPI and DMA for a faster transition
 */
void SPI_Configuration(void)
{
	NVIC_InitTypeDef nvic;
	SPI_InitTypeDef SPI_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);

	SPI_I2S_DeInit(SPI1);
	SPI_Cmd(SPI1, DISABLE);
	DMA_DeInit(DMA_STREAM);

	DMA_StructInit(&DMA_InitStructure);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&fb[0][0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = VTOTAL;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_Channel = DMA_CHANNEL;		   // added channel number
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; // added FIFO mode
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC16;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_MemoryBurst_INC16;
	DMA_Init(DMA_STREAM, &DMA_InitStructure);

	SPI_StructInit(&SPI_InitStructure);
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 0;
	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_CalculateCRC(SPI1, DISABLE);

	nvic.NVIC_IRQChannel = DMA_STREAM_IRQ;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	DMA_STREAM->CR &= ~DMA_SxCR_EN;	   // clear the EN bit to disable the stream
	DMA_STREAM->NDTR = VTOTAL;		   // set number of bytes to transfer
	DMA_STREAM->M0AR = (u32)&fb[0][0]; // set start of frame buffer

	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE); // allow Tx interrupt to generate DMA requests
	SPI_Cmd(SPI1, ENABLE);

	DMA_ITConfig(DMA_STREAM, DMA_IT_TC, ENABLE);
	DMA_STREAM->CR |= DMA_SxCR_EN; // set the EN bit to enable the stream
}

/**
 * @brief IRQ call at the end of every horizontal back porch
 * @warning If you change anything, you should adjust Tim1 Ouput Compare 2
 *
 * @details Check if it is a valid vertical line and then start the stream with DMA and SPI
 */
void TIM1_CC_IRQHandler(void)
{
	TIM1->SR &= ~TIM_IT_CC2;
	if (vsync)
	{
		DMA_STREAM->CR |= DMA_SxCR_EN; // set the EN bit to enable the stream
	}
}

/**
 * @brief IRQ call at the end of every vertical back porch
 * @warning  If you change anything, you should adjust Tim2 Ouput Compare 3
 *
 * @details Set if it is in a valid vertical frame
 */
void TIM2_IRQHandler(void)
{
	TIM2->SR &= ~TIM_IT_CC3; // 0xFFF7; //~TIM_IT_CC3;
	vsync = 1;
}

/**
 * @brief IRQ call at the end of every stream transfer.
 *
 * @details This code disable the stream, then updates values in the stream register
 * to prepare for the next stream.
 *
 * @return At the end of the function the stream is disabled but ready
 *
 */
void DMA_STREAM_IRQHANDLER(void)
{
	VIDEO_DMA->LIFCR = DMA_LIFCR_CTCIF3; // clear the transfer complete interrupt flag
	DMA_STREAM->CR &= ~DMA_SxCR_EN;		 // clear the EN bit to disable the stream

	vline++;

	if (vline == VID_VSIZE)
	{
		vline = vsync = 0;
		DMA_STREAM->M0AR = (u32)&fb[0][0];
	}
	else
	{
		DMA_STREAM->M0AR += VTOTAL;
	}
}

/**
 * @brief write all 0 on the frame buffer
 *
 */
void vidClearScreen(void)
{
	u16 x, y;

	for (y = 0; y < VID_VSIZE; y++)
	{
		for (x = 0; x < VTOTAL; x++)
		{
			while (!vsync)
				__WFI();

			if (fb[y][x] == 0)
				continue;

			fb[y][x] = 0;
		}
	}
}

void vidInit(void)
{
	SPI_Configuration();
	TIMER_Configuration();
	vidClearScreen();
}
///@}
///@}