#include "baseSoftware.h"
#include "video.h"
#include "scheduler.h"
#include "programmes.h"
#include "stm32f4xx_gpio.h"

#define PROGRAM_TO_LINE(x) ((x+1) * 2)

typedef enum keymap{
    KEY_D = 1,
    KEY_H = 2,
    KEY_0 = 3,
    KEY_T = 4,
    KEY_C = 5,
    KEY_9 = 6,
    KEY_8 = 7,
    KEY_7 = 8,
    KEY_B = 9,
    KEY_6 = 10,
    KEY_5 = 11,
    KEY_4 = 12,
    KEY_A = 13,
    KEY_3 = 14,
    KEY_2 = 15,
    KEY_1 = 16
};

u8 isFrameChanged = 1;

__always_inline inline void initPinIO(void);
void selectorScreen(void);
u8 readGPIOKeyboard(void);
u8 *keyboardInputToString(uc8 input);
uc8 getInput(void);

__always_inline inline void initPinIO(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_StructInit(&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin &= GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_StructInit(&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin &= GPIO_Pin_4 | GPIO_Pin_8 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_8 | GPIO_Pin_6 | GPIO_Pin_7);
}

void selectorScreen(void)
{
    static u8 programSelector;

    if (isFrameChanged)
    {
        isFrameChanged = 0;
        vidClearScreen();

        gdiDrawTextEx(CHAR_ON_SCREEN_X(5), CHAR_ON_SCREEN_Y(2), "0", GDI_ROP_OR, GDI_LEFT_ALIGN);
        gdiDrawTextEx(CHAR_ON_SCREEN_X(5), CHAR_ON_SCREEN_Y(4), "0", GDI_ROP_OR, GDI_LEFT_ALIGN);
        gdiDrawTextEx(CHAR_ON_SCREEN_X(5), CHAR_ON_SCREEN_Y(6), "0", GDI_ROP_OR, GDI_LEFT_ALIGN);

        gdiInvertTextLine(CHAR_ON_SCREEN_Y(PROGRAM_TO_LINE(programSelector)));
    }
    uc8 keyPressed = getInput();
    if (keyPressed)
    {
        if (keyPressed == KEY_4 && programSelector > 0)
        {
            programSelector--;
        }
        else if (keyPressed == KEY_6 && programSelector < PRO_NUM_PROGRAM-1)
        {
            programSelector++;
        }
        isFrameChanged = 1;
    }
}

void programSwapper()
{
    
}

u8 readGPIOKeyboard(void)
{
    for (volatile u16 pin = GPIO_Pin_0; pin <= GPIO_Pin_3; pin <<= 1)
    {
        u8 output = 0;
        output |= pin;
        GPIO_SetBits(GPIOD, pin);

        for (volatile u16 input = GPIO_Pin_4; input <= GPIO_Pin_7; input <<= 1)
        {
            input == GPIO_Pin_5 ? input = GPIO_Pin_8 : 0;
            if (GPIO_ReadInputDataBit(GPIOD, input))
            {
                output |= (input == GPIO_Pin_8 ? GPIO_Pin_5 : input) | output;
                GPIO_ResetBits(GPIOD, pin);
                return output;
            }
            input == GPIO_Pin_8 ? input = GPIO_Pin_5 : 0;
        }

        GPIO_ResetBits(GPIOD, pin);
    }
    return 0;
}

uc8 getInput(void)
{
    static u8 lastKey;
    u8 res;
    switch (readGPIOKeyboard())
    {
    case 0x00:
        res = 0;
        break;

    case 0x11:
        res = 1;
        break;
    case 0x12:
        res = 2;
        break;
    case 0x14:
        res = 3;
        break;
    case 0x18:
        res = 4;
        break;

    case 0x21:
        res = 5;
        break;
    case 0x22:
        res = 6;
        break;
    case 0x24:
        res = 7;
        break;
    case 0x28:
        res = 8;
        break;

    case 0x41:
        res = 9;
        break;
    case 0x42:
        res = 10;
        break;
    case 0x44:
        res = 11;
        break;
    case 0x48:
        res = 12;
        break;

    case 0x81:
        res = 13;
        break;
    case 0x82:
        res = 14;
        break;
    case 0x84:
        res = 15;
        break;
    case 0x88:
        res = 16;
        break;

    default:
        return 0;
    }

    if (lastKey != res)
    {
        lastKey = res;
        return res;
    }
    return 0;
}

u8 *keyboardInputToString(uc8 input)
{
    switch (input)
    {
    case 0x00:
        return (u8 *)"no input";

    case 0x11:
        return (u8 *)"0";
    case 0x12:
        return (u8 *)"1";
    case 0x14:
        return (u8 *)"2";
    case 0x18:
        return (u8 *)"3";

    case 0x21:
        return (u8 *)"4";
    case 0x22:
        return (u8 *)"5";
    case 0x24:
        return (u8 *)"6";
    case 0x28:
        return (u8 *)"7";

    case 0x41:
        return (u8 *)"8";
    case 0x42:
        return (u8 *)"9";
    case 0x44:
        return (u8 *)"10";
    case 0x48:
        return (u8 *)"11";

    case 0x81:
        return (u8 *)"12";
    case 0x82:
        return (u8 *)"13";
    case 0x84:
        return (u8 *)"14";
    case 0x88:
        return (u8 *)"15";

    default:
        return (u8 *)"Error in conversion!";
    }
}

void programCallback(void)
{
    vidClearScreen();

    initPinIO();

    schAddIndexTask(100, 0, selectorScreen);
    schAddTask(1000, programSwapper);
}