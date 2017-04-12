/*
 * led_photo_pshik.с
 *
 * Created: 12.06.2013 16:00:00
 *  Author: Grigoriev Alexander
        http://pastebin.com/JLT2Gf7a
 */
 
//Изменение состояния светодиода в момент нажатия на кнопку:
//мк ATtiny13
//F_CPU = 9600000/8/2 = 4.8Mhz
// FUSE HIGH = 0xFF, LOW=0x69, (4.8MHz, CKDIV8)
 
#define F_CPU   9600000UL/8UL/2UL
 
// #define DEBUG 0
#include <avr/io.h>
#include <avr/interrupt.h>
#include <compat/deprecated.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include "airwick.h"
       
void init();
 
 
#define BLINK_WARNING   {blinkinfo(LED_K,5,5);}
#define RESET_COUNTER   {countDayTimer=0; MTimer=0; counter=0;}                                     //сброс таймера и единичного пуска}
#define PSHIK           {BLINK_WARNING; sbi(DDRB,GATE); sbi(PORTB,LED_K); sbi(PORTB,GATE); _delay_ms(1000); cbi(DDRB,GATE);cbi(PORTB,GATE); cbi(PORTB,LED_K);}
#define waitms                  50
 
 void keyscan()
 {
wdt_reset();
 
u8 long_key=0;
u8 set_hrs=0;   //eeprom_read_byte(&eset_hrs);
 
delay_ms(waitms);
 
if (bit_is_clear(PINB,BUTTON))
{
        long_key=0;
        delay_ms(waitms);
 
        //Режим длинного нажатия, включаем режим програмирования
        while (bit_is_clear(PINB,BUTTON))
        {
                long_key=1;                                                     //Устанавливаем флаг длинного нажатия
                if(++set_hrs >=(CALIBRATE_MODE+1)) set_hrs =1;                  //Пятый режим оставим для калибровки LED, и пускаем по кругу
                blinkinfo(LED_K,set_hrs,20);                                    //мигаем кол-во выбранных тактов, там же говорим собаке фу.
                _delay_ms(1000);                                                //пауза перед увеличением такта
                wdt_reset();                                                    //не забываем про собаку
                counter=set_hrs;
        }
       
        if (bit_is_set(PINB,BUTTON)){                                           //если кнопочку отпустили- записываем выбранный режим
                eeprom_update_byte(&eset_hrs,set_hrs);
                blinkinfo(LED_K, set_hrs, 40);                                  //Подтверждаем выбор более медленной индикацией
        }
       
}
if (!long_key)        {
        cbi(PORTB,LED_A); sbi(PORTB,LED_K);                         //Разовое нажатие вызывает обычный пшик
        counter=1; long_key=0;
        PSHIK;
RESET_COUNTER;}
 
 }
 
 
ISR(INT0_vect)  // Обработчик прерываний
{
keyscan();
}
 
 
 
 
#if USE_LIGHTSENSOR
/**
 * \Измеряем освещенность с помощью внутренней энергии ЦИ светодиода. :)
 * \вовзращаем потраченное на измерение время в тиках пока заряжен диод.
 */
//************************************
u16 LightLumen()
{
u16 j=65534;            //Для определения ночи необходимо увеличить время перебора, но я уменьшил в целях экономии памяти на тиньке 13,
                                        //для ламп люми. при прямом попадании света на диод ,результат функции около 16000. Также, желательно вытащить светодиод максимально наружу.
 
        DDRB|=  (1<<LED_A)|(1<<LED_K);  //ПОРТКИ НА ВЫХОД, НЕ ТЕ ПОРТЫ ЧТО НА МНЕ, А ТЕ ЧТО В  ТИНЬКЕ.
        sbi(PORTB,LED_A);                               //ОБРАТНОЕ НАПРЯЖЕНИЕ НА СВЕТОДИОД, ЗАРЯЖАЕМ ВНУТРЕННЮЮ ЕМКОСТЬ НОГ МИКРОКОНТРОЛЛЕРА, ЭНЕРГИЯ ЦИ НАРАСТАЕТ
        cbi(PORTB,LED_K);                               //БЕЗ ЗЕМЛИ НЕ ЗАРЯДИТСЯ
        cbi(DDRB, LED_A);                               //отключаем подтяжку
        cbi(PORTB,LED_A);
 
        // Считаем сколько времени потребуется, чтобы емкость разрядилась до логич. 0
        do
        {
                wdt_reset();
                if (bit_is_clear(PINB,LED_A)) break;
        }while (j--);
 
_delay_ms(10);
 
//blinkinfo(LED_K,1,2);                 //мигаем при каждом измерении.
 
return 65534-j;
}
#endif
 
 
void blinkinfo(u8 bit,u8 col, u8 time){
sPORTB=PORTB;
sbi(DDRB,bit); cbi(PORTB,bit);
sbi(DDRB,LED_A);        cbi(PORTB,LED_A);
//u8 t=0;
//while (t<col)
while (col--)
        {        //t++;
                 sbi(PORTB,bit);        
//				 wdt_reset();     
				 delay_ms(time);
				 
                 cbi(PORTB,bit);        
				 wdt_reset();     
				delay_ms(time);
        }
		
PORTB=sPORTB;

}
void delay_ms(u8 count) 
{ 
	while(count--) {_delay_ms(10);}
}

void init(){
 
          wdt_enable(WDTO_4S);
                  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
 
                  GIMSK |= (1<<INT0);   // 0b01000000;          // Разрешение прерываний INT0 на входе PB1
                  GIFR  |= (1<<INTF0);                 
                                 
          //MCUCR = (1<<SE)|(1<<SM1)|(0<<SM0);
          MCUCR |= (0<<ISC01)|(0<<ISC00);                       // при любом перепаде (нужно отслеживать нажатие и отжатие)
         
 
                  DDRB  |= (1<<LED_K)|(1<<LED_A)|(1<<GATE)|(1<<INFO);    //output
          PORTB &=~(1<<LED_K)|(1<<LED_A)|(1<<GATE)|(1<<INFO);    //gnd
          DDRB  &=~(1<<BUTTON);          //input
          PORTB |= (1<<BUTTON);          //pullup
                sei();
}
 
int __attribute__((naked))
main(void)
{
       
        init();
         
#if USE_LIGHTSENSOR
       
        if (counter==5)    // Режим калибровки светодиода, должен быть включен свет.
        {
        blinkinfo(LED_K,1,100);
         eeprom_update_word(&eLight_level,LightLumen()+2000); RESET_COUNTER;
        blinkinfo(LED_K,1,100);
}
#endif
 
 
 
//***********  Обрабатываем датчик света ********
#if USE_LIGHTSENSOR
 
u8 LedSensorStatus=(LightLumen()>=eeprom_read_word(&eLight_level))?0:1; // Темно -0, Светло -1 : Текущее освещение
 
 
if (!(LastLedSensorStatus==LedSensorStatus)) // Изменилось освещение ?
		{
                if (LedSensorStatus<LastLedSensorStatus) {
						if(countDayTimer>3*Per_Minute) //Если выключили свет и прошло 3 минуты- пшик
                                                {PSHIK;RESET_COUNTER;}
									  					 }
												 else //Если включили свет               
													     {
								blinkinfo(LED_K,10,2);  //Мигнуть длинной очередью
												         }

				countDayTimer=0;
                LastLedSensorStatus=LedSensorStatus; //Запоминаем текущее состояние освещения
				
}
  
if (countDayTimer>3600) countDayTimer=0; //Переполнение счетчика
 
 countDayTimer+=LedSensorStatus;  //Если горит свет - наматывать счетчик
 
 
blinkinfo(LED_K,1+LedSensorStatus,2); // в дневное время моргнуть дважды, ночью одинарной вспышкой
#else
blinkinfo(LED_K,1,2);                   //имитация работы если не используем лед, как датчик света
#endif

 
// *********************  Запускаем счетчик ***************
u8 set_hrs=eeprom_read_byte(&eset_hrs); //считываем режим работы
if(set_hrs >=CALIBRATE_MODE+1) set_hrs =1; // бегаем по кругу, в меню. 12345-12345-....
if(++MTimer==((set_hrs)*Per_Watch)){PSHIK;RESET_COUNTER;} //Увеличиваем счетчик и пшикаем если подошло время
      
// if ((MTimer>3500)) RESET_COUNTER;
 
sleep_mode(); //засыпаем и ждем сторожа или кнопку

while(1){}     
 
}