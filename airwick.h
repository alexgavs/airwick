/*
 * led_photo_pshik.h
 *
 * Created: 24.05.2013 20:00:11
 *  Author: Grigoriev Alexander
 */
 
 
#ifndef LED_PHOTO_PSHIK_H_
#define LED_PHOTO_PSHIK_H_
 
#define  u8 unsigned char               //  0 to 255
#define  u16 unsigned int               //  0 to 65535
#define  u32 unsigned long int  //      0 to 4294967295
 
#define USE_LIGHTSENSOR 1
 
#ifdef DEBUG
#define Per_Watch       3
#define Per_Minute      1
#else
#define Per_Watch       705
#define Per_Minute      13		      // 60/5.105 = ~12 ?????? ????????? ? ????? ????????? - 5.105?? , ??? ????????? ????????? ???????  = ???????? ???-?? ??????  ?? 12
#endif
 
#define BUTTON          PB1
#define LED_A           PB4
#define LED_K           PB0
#define GATE            PB2
#define INFO            PB3
 
#define CALIBRATE_MODE  5
 
#define SRAM __attribute__((section(".noinit")))
 
SRAM            u16             MTimer;
SRAM            u8              counter;
SRAM            u16             countDayTimer;
SRAM            u8              LastLedSensorStatus;
 
EEMEM           u8              eset_hrs=2;
EEMEM           u16             eLight_level=16000;
EEMEM           u8              eNight;
                        u8              sPORTB,sDDRB;
 
void delay_ms(u8 count);
void blinkinfo(u8 bit,u8 col, u8 time);
 
#endif /* LED_PHOTO_PSHIK_H_ */
 