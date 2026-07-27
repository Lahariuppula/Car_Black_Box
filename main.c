/*
 * Author - Soundharya Lahari Uppula
 * 31-05-2026 5:00PM
*/
#include "main.h"
#pragma config WDTE = OFF

static void init_config(void)
{
    init_i2c(100000);
    init_ds1307();
    init_clcd();
    init_digital_keypad();
    init_adc();
    init_timer2();
    init_uart(9600);
    GIE=1;
    PEIE = 1;
}
void main(void)
{
    init_config();
    unsigned char event[3]= "ON";//ON C GN GR G1 G2 G3 G4
    unsigned char speed = 0,key;//0 - 99
    unsigned char operational_flag = DASHBOARD_SCREEN, reset_flag;
    char *gear[] = {"GN","GR","G1","G2","G3","G4"};
                    //0   1    2    3    4    5  
    unsigned char gr = 0;
    unsigned char menu_pos = 0;
    extern unsigned char return_time;
    ext_eeprom_24C02_str_write(0x00,"1111");
    log_event(event,speed);
    while(1)
    {
        speed = read_adc()/10;//0 - 1023(max) we need to scale down to 99 [0-99]
        if(speed > 99)
        {
            speed = 99;
        }
        for(unsigned char delay = 400;delay--;);
        key = read_digital_keypad(); 
        if(key == SW1)
        {
            strcpy(event,"C ");
            log_event(event,speed);
        }
        else if(key == SW2 && gr < 6)
        {
            strcpy(event,gear[gr]);//GN GR G1 G2 G3 G4
            gr++;
            log_event(event,speed);
        }
        else if(key == SW3 && gr > 0)//G4 G3 G2 G1 GR GN
        {
            gr--;
            strcpy(event,gear[gr]);
            log_event(event,speed);
        }
        /*SW4,SW5 Pressed to check which screen we are in.
          Only if we are in dashboard screen we need to change to login screen.*/
        else if((key == SW4 || key == SW5) && operational_flag == DASHBOARD_SCREEN) 
        {
            clear_screen();
            clcd_print("ENTER PASSWORD",LINE1(1));
            clcd_write(LINE2(4),INST_MODE);
            clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
            __delay_us(100);
            operational_flag = LOGIN_SCREEN;
            reset_flag = RESET_PASSWORD;          
            TMR2ON = 1;
        }
        else if (key == SW4_LP && operational_flag == MAIN_MENU_SCREEN)
        {
            switch (menu_pos)
            {
                case 0: /* View Log */

                    clear_screen();
                    clcd_print("LOGS", LINE1(0));
                    operational_flag = VIEW_LOGS;
                    reset_flag = RESET_VIEW_LOG;
                break;

                case 1: /* Clear Log */

                    clear_screen();
                    operational_flag = CLEAR_LOGS;
                    reset_flag = RESET_CLEAR_LOG;
                break;

                case 2: /* Download Log */
                    clear_screen();
                    log_event("DL", speed);
                    clcd_print("OPEN CUTECOM", LINE1(0));
                    download_logs();
                    __delay_ms(2000);
                    operational_flag = MAIN_MENU_SCREEN;
                    reset_flag = RESET_LOGIN_SCREEN;
                break;
                    
                case 3: /* Set Time */
                    clear_screen();
                    log_event("ST", speed);
                    operational_flag = SET_TIME;
                    reset_flag = RESET_TIME;
                break;
                    
                case 4: /* Change Password */
                    clear_screen();
                    operational_flag = CHANGE_PASSWORD;
                    reset_flag = RESET_PASSWORD;
                    TMR2ON = 1;
                break;      
            }
        }
        else if (key == SW4_LP && operational_flag == VIEW_LOGS)
        {
            operational_flag = MAIN_MENU_SCREEN;
            reset_flag = RESET_MENU;
            return_time = 5;
            clear_screen();
        }
        else if (key == SW5_LP && operational_flag == VIEW_LOGS)
        {
            operational_flag = DASHBOARD_SCREEN;
            return_time = 5;
            clear_screen();
        }
        else if (key == SW4_LP && operational_flag == CHANGE_PASSWORD)
        {
            operational_flag = MAIN_MENU_SCREEN;
            clear_screen();
        }
        else if (key == SW5_LP && operational_flag == MAIN_MENU_SCREEN)
        {
            operational_flag = DASHBOARD_SCREEN;
            clear_screen();
        }
        //dashboard,login
       switch(operational_flag)
       {
           case DASHBOARD_SCREEN:
               display_dashboard_screen(event,speed);
            break;
            
           case LOGIN_SCREEN:
               switch(login_screen(key,reset_flag))
               {
                   case RETURN_BACK:
                       operational_flag = DASHBOARD_SCREEN;
                       clear_screen();
                       TMR2ON = 0;
                       clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                       __delay_us(100);                      
                       break;
                   case LOGIN_SUCCESS:
                       operational_flag = MAIN_MENU_SCREEN;
                       clear_screen();
                       reset_flag = RESET_MENU;
                       TMR2ON = 1;
                       clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                       __delay_us(100);
                       continue;
                    break;
               }
            break;
            
           case MAIN_MENU_SCREEN:
                switch(menu_screen(key,reset_flag))
                {
                    case RETURN_BACK:
                        clear_screen();
                        operational_flag = DASHBOARD_SCREEN;
                        TMR2ON = 0;
                        break;

                    case 0:
                        menu_pos = 0;
                        break;

                    case 1: 
                        menu_pos = 1;
                        break;

                    case 2: 
                        menu_pos = 2;
                        break;

                    case 3: 
                        menu_pos = 3;
                        break;

                    case 4: 
                        menu_pos = 4;
                        break;
                }
            break;
            
            case VIEW_LOGS:
                view_logs(key,reset_flag);
            break;
            
            case CLEAR_LOGS:
                if(clear_logs(reset_flag)==SUCCESS)
                    __delay_ms(1000);
                if(reset_flag == RESET_CLEAR_LOG)
                    log_event("CL",speed);
                operational_flag = MAIN_MENU_SCREEN;
                reset_flag = RESET_LOGIN_SCREEN;
                clear_screen();
            break;

            case SET_TIME:
                if(set_time(key,reset_flag)== SUCCESS)
                {
                    return_time = 5;
                    operational_flag = MAIN_MENU_SCREEN;
                    reset_flag = RESET_MENU;
                    clear_screen();
                    continue;
                }
            break;

            case CHANGE_PASSWORD:
                switch(change_password(key,reset_flag))
                {
                    case SUCCESS:
                        __delay_ms(1000);
                        log_event("CP",speed);
                        operational_flag = MAIN_MENU_SCREEN;
                        reset_flag = RESET_LOGIN_SCREEN;
                        clear_screen();
                        break;
                    case RETURN_BACK:
                        operational_flag = DASHBOARD_SCREEN;
                        reset_flag = RESET_LOGIN_SCREEN;
                        break;
                }
            break; 
        }
        reset_flag = RESET_NOTHING;
    }
    return;
}