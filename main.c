/*
 * File:   main.c
 * Author: Komala
 *
 * Created on September 13, 2023, 9:49 AM
 */


#include <xc.h>
#include "clcd.h"
#include "matrix_keypad.h"
#include "main.h"
#include "timers.h"

#pragma config WDTE = OFF
int operation_mode, reset_flag;
int min, sec;
int ret, flag = 0, heat_flag = 0;
char sec_arr[3], min_arr[3];

static void init_config(void) {
    init_clcd();
    init_matrix_keypad();
    init_timer2();
    /*for the buzzer*/
    BUZZ_DDR = 0;
    BUZZER = OFF;
    /*for the fan*/
    FAN_DDR = 0;
    FAN = OFF;
    /*for door*/
    //DOOR_DDR = 1;
    //DOOR = CLOSE;
    
    PEIE = 1;
    GIE = 1;
}

void main(void) {
    unsigned char key;
    init_config();
    power_on();
    clear_dis();
    operation_mode = NOT_PRESSED;
    
    
    while(1)
    {
        key = read_matrix_keypad(STATE);
        if(operation_mode == MICRO || operation_mode == GRILL || operation_mode == CONVECTION)
        {
            ;
        }
            
        else if(key == 1)
        {
            operation_mode = MICRO;
            reset_flag = MICRO_RESET;
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            clear_dis();
            
            clcd_print(" Power = 900W   ", LINE2(0));
           
           __delay_ms(3000);
           clear_dis();
        }
        else if(key == 2) {
            clear_dis();
            operation_mode = GRILL; 
            reset_flag = GRILL_MODE_RESET;
             
        }
        else if(key == 3) {
            clear_dis();
            operation_mode = CONVECTION; 
            reset_flag = CONVECTION_RESET;
        }
        else if(key == 4) {
            if(operation_mode == NOT_PRESSED)
            {
                min = 0;
                sec = 30;
                TMR2ON = ON;
                FAN = ON;
                operation_mode = TIME_DISPLAY;
            }
            //operation_mode = START;
            else if(operation_mode == TIME_DISPLAY)
            {
                sec = sec + 30;
                if(sec >= 60)
                {
                    min++;
                    sec = sec - 60;
                }
            }
            else if(operation_mode == PAUSE)
            {
               TMR2ON = ON;
                FAN = ON;
                operation_mode = START;
            }
            
        }
        else if(key == 5) {
            operation_mode = PAUSE;
        }
        else if(key == 6) {
            operation_mode = STOP;
            clear_dis();
        }
        
       mode_select(key);
    }
        
    
   
}

void mode_select(unsigned char key) {
    switch(operation_mode)
    {
        case NOT_PRESSED:
            modes_display();
            break;
        case START:
        case TIME_DISPLAY:
            time_dis();
            break;
        case MICRO:
            set_time(key);
            break;
        case GRILL:
            set_time(key);
            break;
        case CONVECTION:
            if(heat_flag == 0) 
            {
                ret = set_temp(key);
                if(ret == FAILURE)
                {
                    flag = 1;
                    reset_flag = CONVECTION_RESET;
                }
                else if(ret == SUCCESS)
                {
                    TMR2ON = OFF;
                    flag = 1;
                    heat_flag = 1;
                    clear_dis();
                    reset_flag = RESET_TIME;
                }
                else
                {
                    flag = 0;
                }
            }
            else
            {
                flag = 0;
                set_time(key);
            }
            break;
        case PAUSE:
            TMR2ON = OFF;
            FAN = OFF;
            break;
        case STOP:
            TMR2ON = OFF;
            FAN = OFF;
            operation_mode = NOT_PRESSED;
            break;
    }
    if(flag == 0)
    {
        reset_flag = NONE_RESET;
    }
}


    
    

void power_on(void)
{
    for(int i = 0; i<16; i++)
    {
        clcd_putch(BLOCK, LINE1(i));
    }
    clcd_print("  Powering ON   ", LINE2(0));
    clcd_print(" Microwave Oven ", LINE3(0));
    for(int i = 0; i<16; i++)
    {
        clcd_putch(BLOCK, LINE4(i));
    }
    __delay_ms(1500);
    
}
void modes_display(void)
{
    clcd_print("1.Micro", LINE1(0));
    clcd_print("2.Grill", LINE2(0));
    clcd_print("3.Convection", LINE3(0));
    clcd_print("4.Start", LINE4(0));
    
}
void clear_dis(void) 
{
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
}
void set_time(unsigned char key) {
    static int wait, blink, blink_pos, key_count;
    if(reset_flag >= 0x11)
    {
        
        key = ALL_RELEASED;
        sec = 0;
        min = 0;
        key_count = 0;
        blink = 0;
        blink_pos = 0;
        //wait = 0;
       
        clcd_print("SET TIME (MM:SS)", LINE1(0));
        clcd_print("TIME- 00:00", LINE2(0));
        clcd_print("*:CLEAR  #:ENTER", LINE4(0));
    }
    /*if(wait++ == 20) {      /non blocking delay/
        wait = 0;
        blink = !blink;
    }*/
    
    if(key != ALL_RELEASED && key != '*' && key != '#')
    {
        
        key_count++;
        if(key_count <= 2)
        {
            sec = sec * 10 + key;
            blink_pos = 0;
        }
        else if(key_count > 2 && key_count < 5)
        {
            min = min *10 + key;
            blink_pos = 1;
        }
        if(key_count < 2) {
            blink_pos = 0;
        }
        else if(key_count >= 2 && key_count < 5) {
            blink_pos = 1;
        }
    }
    
    else if(key == '*')
    {
        if(key_count <= 2)
        {
            sec = 0;
            key_count = 0;
        }
        else if(key_count > 2 && key_count < 5)
        {
            min = 0;
            key_count = 2;
        }
    }
    else if(key == '#')
    {
        clear_dis();
        TMR2ON = 1;
        FAN = ON;
        operation_mode = TIME_DISPLAY;
    }
    
    
    sec_arr[0] = sec / 10 + '0'; //19---> 1
    sec_arr[1] = sec % 10 + '0';//19---> 9
    sec_arr[2] = '\0';
    
    min_arr[0] = min / 10 + '0'; //19---> 1
    min_arr[1] = min % 10 + '0';//19---> 9
    min_arr[2] = '\0';
            
    
    
    if(wait++ == 25)
    {
        wait = 0;
        blink = !blink;
        clcd_print(min_arr, LINE2(6));
        clcd_print(sec_arr, LINE2(9));
        
    }
    
    if(blink)
    {
        switch(blink_pos)
        {
            case 0:
                clcd_print("  ", LINE2(9));
                break;
            case 1:
                clcd_print("  ", LINE2(6));
                break;
        }
    }
    
        
}
void door_status_check(void) {
    if(DOOR == OPEN) 
    {
        BUZZER = ON;
        FAN = OFF;
        TMR2ON = 0;
        clear_dis();
        clcd_print("Door Status:OPEN", LINE2(0));
        clcd_print("Please Close", LINE3(0));
        while(DOOR == OPEN)
        {
            ;
        }
        clear_dis();
        
    }
        TMR2ON = 1;
        BUZZER = OFF;
        FAN = ON;
        
}

void time_dis(void) 
{
    door_status_check();
    clcd_print(" TIME =  ", LINE1(0));
  
    min_arr[0] = min / 10 + '0'; //19---> 1
    min_arr[1] = min % 10 + '0';//19---> 9
    min_arr[2] = '\0';
            
    clcd_print(min_arr, LINE1(9));
    
    clcd_putch(':', LINE1(11));
        
            
    sec_arr[0] = sec / 10 + '0'; //19---> 1
    sec_arr[1] = sec % 10 + '0';//19---> 9
    sec_arr[2] = '\0';
    /*
     clcd_putch(sec / 10 + '0', LINE1(12));
      clcd_putch(sec % 10 + '0', LINE1(13));  //taking each number and converting them to char and printing at a time
     */
    
    clcd_print(sec_arr, LINE1(12));
    
    clcd_print(" 4.Start/Resume ", LINE2(0));
    clcd_print(" 5.Pause        ", LINE3(0));
    clcd_print(" 6.Stop         ", LINE4(0));
    
    if((min == 0) && (sec == 0))
    {
        TMR2ON = 0;
        FAN = OFF;
        clear_dis();
        clcd_print("Cooking Time UP", LINE2(0));
        //Turn on buzzer
        BUZZER = ON;
        __delay_ms(2000);
        //Turn off buzzer
        BUZZER = OFF;
        
        clear_dis();
        operation_mode = NOT_PRESSED;
    }
    
    
}

char set_temp(unsigned char key) {
    static int wait, blink, key_count, temp;
    if(reset_flag == CONVECTION_RESET)
    {
        
        key = ALL_RELEASED;
        key_count = 0;
        blink = 0;
        temp = 0;
        wait = 0;
        clcd_print("SET TEMP. (  C )", LINE1(0));
        clcd_putch(DEGREE, LINE1(12));
        clcd_print("TEMP :  000", LINE2(0));
        clcd_print("*:CLEAR  #:ENTER", LINE4(0));
    }
    if(key != ALL_RELEASED && key != '*' && key != '#')
    {
        
        key_count++;
        if(key_count <= 3)
        {
            temp = temp * 10 + key;
        }
        
    }
    else if(key == '*')
    {
        key_count = 0;
        temp = 0;
    }
    else if(key == '#')
    {
        clear_dis();
        if(temp > 250) 
        {
            clcd_print(" Invalid Temp.", LINE2(0));
            BUZZER = ON;
            __delay_ms(2000);
            BUZZER = OFF;
            clear_dis();
            return FAILURE;
        }
        else
        {
            clcd_print("  Pre-Heating", LINE1(0));
            sec = 180; // 3-min
            TMR2ON = ON;
            FAN = ON;
            
            clcd_print("Time Rem.= ",LINE3(0));
            while(sec)
            {
                clcd_putch((sec / 100 + '0'), LINE3(11));
                clcd_putch(((sec/10) % 10 + '0'), LINE3(12));
                clcd_putch((sec%10 + '0'), LINE3(13));   
            }
            return SUCCESS;
        }
    }
    if(wait++ == 25)
    {
        wait = 0;
        blink = !blink;
        clcd_putch((temp / 100 + '0'), LINE2(8));
        clcd_putch(((temp/10) % 10 + '0'), LINE2(9));
        clcd_putch((temp%10 + '0'), LINE2(10));    
    }
    if(blink)
    {
        clcd_print("   ", LINE2(8));
    }
    return 0x11;
}
