/* 
 * File:   main.h
 * Author: Komala
 *
 * Created on September 13, 2023, 9:50 AM
 */

#ifndef MAIN_H
#define	MAIN_H

#define NOT_PRESSED         0x01
#define MICRO               0x02
#define GRILL               0X03
#define CONVECTION          0x04
#define START               0x05
#define STOP                0x07
#define PAUSE               0x08

#define RESET_TIME          0x13
#define TIME_DISPLAY        0x06

#define MICRO_RESET         0x11
#define GRILL_MODE_RESET    0x12
#define CONVECTION_RESET    0x14
#define NONE_RESET          0x0F
#define BUZZER              RC1
#define BUZZ_DDR            TRISC1
#define ON                  1
#define OFF                 0

#define FAN                 RC2
#define FAN_DDR             TRISC2

#define DOOR                RB3
//#define DOOR_DDR        TRISB3
#define OPEN                0
#define CLOSE               1
#define DEGREE              0xDF
#define FAILURE             1
#define SUCCESS             0


void power_on(void);
void modes_display(void);
void clear_dis(void);
void mode_select(unsigned char);
void set_time(unsigned char);
void time_dis(void);
char set_temp(unsigned char);

#endif	/* MAIN_H */

