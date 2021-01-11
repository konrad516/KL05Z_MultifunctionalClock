#include "MKL05Z4.h"
#include "keyboard.h"
#include "lcd1602.h"
#include "pit.h"
#include "calculate.h"
#include "rtc.h"
#include "buttons.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

bool irqPIT = 0;
bool irqRTC = 1;

uint16_t DMAvalue[8];

uint32_t rtc_seconds_counter = 0;
uint32_t rtc_hours = 0, rtc_minutes = 0, rtc_seconds = 0;

extern unsigned int Image$$value$$Base;

void PIT_IRQHandler()
{
  if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK)
  {
    if (irqPIT == 0)
      irqPIT = 1;
    value = 1;
    PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK; // clear the timer interrupt flag
  }
  NVIC_ClearPendingIRQ(PIT_IRQn);
}

void RTC_Seconds_IRQHandler(void)
{
  rtc_seconds_counter++;
  irqRTC = 1;
}

void PORTB_IRQHandler(void)
{
  if (PORTB->ISFR & (1 << BUT1))
  {
    LCD1602_PrintXY("BUT1\0", 0, 0);
    while ((FPTB->PDIR & (1 << BUT1)) == 0)
      ;                                    //wait for button release
    PORTB->PCR[BUT1] |= PORT_PCR_ISF_MASK; //clear interrupt
  }

  if (PORTB->ISFR & (1 << BUT2))
  {
    LCD1602_PrintXY("BUT2\0", 0, 0);
    while ((FPTB->PDIR & (1 << BUT2)) == 0)
      ;                                    //wait for button release
    PORTB->PCR[BUT2] |= PORT_PCR_ISF_MASK; //clear interrupt
  }

  if (PORTB->ISFR & (1 << BUT3))
  {
    LCD1602_PrintXY("BUT3\0", 0, 0);
    while ((FPTB->PDIR & (1 << BUT3)) == 0)
      ;                                    //wait for button release
    PORTB->PCR[BUT3] |= PORT_PCR_ISF_MASK; //clear interrupt
  }
}

void DMA0_IRQHandler(void)
{
  DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK; // clear interrupt
  DMA_DSR_BCR0 |= DMA_DSR_BCR_BCR(2);    // 2 bytes (16 bits) per transfer
}

int main(void)
{
  LCD1602_Init(); // initialize LCD
  LCD1602_Backlight(TRUE);
  KB_init();       // initialize Keyboard
  PIT_Init();      // initialize PIT
  RTC_init();      // initialize RTC
  BUTTOONS_init(); // initialize buttons

  LCD1602_PrintXY("Time:\0", 0, 1);

  while (1)
  {
    if (irqPIT)
    {
      loop();
      irqPIT = 0;
    }
    if (irqRTC)
    {
      display_time();
      irqRTC = 0;
    }

    __wfi(); // save energy and wait for interrupt
  }
}
