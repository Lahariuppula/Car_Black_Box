/*
* car_black_box_def.c 
*/
#include "main.h" 
unsigned char clock_reg[3];
char time[7]; // "HHMMSS"
char log[11]; //Time(HHMMSS-6),Event(2),Speed(2),Null(1) - Total 11 bytes needed
int pos = -1;
int event_count = -1;
unsigned char return_time=5;
unsigned int sec;
char *menu[]={"VIEW LOG","CLEAR LOG","DOWNLOAD LOG ","SET TIME","CHANGE PASSWD"};
char menu_pos;

static void get_time()
{
    clock_reg[0] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[1] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[2] = read_ds1307(SEC_ADDR); // SS -> BCD 
    // HH -> 
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';   
    // MM 
    time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';  
    // SS
    time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';
    time[6] = '\0';
}

void display_time(void)
{
    get_time();
    clcd_putch(time[0],LINE2(0));
    clcd_putch(time[1],LINE2(1));
    clcd_putch(':',LINE2(2));
    clcd_putch(time[2],LINE2(3));
    clcd_putch(time[3],LINE2(4));
    clcd_putch(':',LINE2(5));
    clcd_putch(time[4],LINE2(6));
    clcd_putch(time[5],LINE2(7));
}

void display_dashboard_screen(unsigned char event[],unsigned char speed)
{
    clcd_print("TIME     EV   SP",LINE1(0));
    //To display event
    clcd_print(event,LINE2(10));
    //To display speed
    clcd_putch((speed/10) + '0',LINE2(14));
    clcd_putch((speed%10) + '0',LINE2(15));
    //To display time
    display_time();
}

void store_log()
{
    //To store log in External EEPROM                                                           
    char addr = 0x05;
    if(pos++ == 10)
    {
        pos = 0;
    }
    addr = pos*10 + addr;//0*10+5=5,1*10+5=15,2*10+5=25
    ext_eeprom_24C02_str_write(addr,log);
    if(event_count < 9)
    {
        event_count++;
    }
}

void log_event(unsigned char event[],unsigned char speed)
{
    /* HHMMSSEVSP */
    get_time();
    
    strncpy(log,time,6);//HHMMSS
    strncpy(&log[6],event,2);//EV
    log[8] = (speed/10) + '0';
    log[9] = (speed%10) + '0';
    log[10] = '\0';
    store_log();
}

void clear_screen()
{
    clcd_write(CLEAR_DISP_SCREEN,INST_MODE);
    __delay_ms(2);
}

unsigned char login_screen(unsigned char key,unsigned char reset_flag)
{
    static char user_pass[4];
    static int i;
    static unsigned char attempts_left = 3;
    
    if(reset_flag == RESET_PASSWORD)
    {
        attempts_left = 3; 
        user_pass[0] = '\0';
        user_pass[1] = '\0';
        user_pass[2] = '\0';
        user_pass[3] = '\0';
        
        i = 0;
        key = 0xFF;
        return_time = 5;
    }
    if(return_time == 0)
    {
        /*Return to Dashboard*/
        return RETURN_BACK;
    }
    __delay_ms(50);
    if(key == SW4 && i < 4)
    {
        //'1'
        user_pass[i]='1';        
        clcd_putch('*',LINE2(i+4));//****
        i++;
        return_time = 5;
    }
    else if(key == SW5 && i < 4)
    {
        //'0'
        user_pass[i]='0';
        clcd_putch('*',LINE2(i+4));//****
        i++;
        return_time = 5;
    }
    /*Compare user_password with stored_pass*/
    if(i == 4)
    {
        char stored_pass[4];
        for(int j=0; j<4; j++)
        {
            stored_pass[j]=ext_eeprom_24C02_read(j);
        }
        if(strncmp(user_pass,stored_pass,4) == 0)
        {
            clear_screen();
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            clcd_print("LOGIN SUCCESS",LINE1(1));
            //if login success display main menu screen
            __delay_ms(1000);
            return LOGIN_SUCCESS;
        }
        else
        {
            attempts_left--;
            if(attempts_left == 0)
            {
                sec = 60;
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                clcd_print("USER BLOCKED",LINE1(1));
                clcd_print("Wait for ",LINE2(0)); //60 59 58 ... 0
                //__delay_ms(3000);
                clcd_print("Secs",LINE2(12));
                while(sec)
                {
                    clcd_putch(sec/10+'0',LINE2(9));
                    clcd_putch(sec%10+'0',LINE2(10));
                }
                attempts_left = 3;
            }
            else
            {
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);   
                clcd_print("WRONG PASSWORD",LINE1(1));
                clcd_putch(attempts_left+'0',LINE2(0));
                clcd_print("Attempts Left",LINE2(2));
                __delay_ms(2000);
            }
            clear_screen();
            clcd_print("ENTER PASSWORD",LINE1(1));
            i=0;
            return_time = 5;          
            clcd_write(LINE2(4),INST_MODE);
            clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
            __delay_us(100);
        }
    }
    return 0xFF;
}

unsigned char menu_screen(unsigned char key,unsigned char reset_flag)
{
    static char menu_pos;
    if (return_time == 0)
    {
        return RETURN_BACK;
    }
    if(reset_flag == RESET_MENU)
    {
        return_time = 5;
        menu_pos = 0;
        clear_screen();
    }
    
    if (key == SW4 && menu_pos > 0) //scrolling up
    {
        return_time = 5;
        menu_pos--;
        clear_screen();
    }
    
    else if (key == SW5 && menu_pos <= 3) //scrolling down
    {
        return_time = 5;
        menu_pos++;
        clear_screen();
    }
    if(menu_pos == 4)
    {
       clcd_print(menu[menu_pos - 1],LINE1(2));
       clcd_print(menu[menu_pos],LINE2(2));
       clcd_putch('*',LINE2(0));
    }
    else if(menu_pos < 4)
    {
        clcd_print(menu[menu_pos],LINE1(2));
        clcd_print(menu[menu_pos+1],LINE2(2));
        clcd_putch('*',LINE1(0));
    }
    return menu_pos;
}

unsigned char view_logs(unsigned char key, unsigned char reset_flag)
{
    static int index;
    char log[11];
    log[10] = 0;
    int position;
    unsigned char add;
    if(event_count == -1)
    {
        clcd_print("                ", LINE1(0));
        clcd_print("NO LOGS",LINE2(0));
    }
    else
    {
        if(reset_flag == RESET_VIEW_LOG)
        {
            index = 0;
        }
        if (key == SW4)
        {
            if(index < event_count)
                index++;
            else
                index = 0;
        }
        else if (key == SW5) 
        {
            if(index > 0)
                index--;
            else
                index = event_count;
        }
        position = index;
        for(char i = 0; i < 10; i++)
        {
            add = position * 10 + 5 + i;
            log[i] = ext_eeprom_24C02_read(add);
        }
        /*printing logs on CLCD */

        /* displaying index */
        clcd_putch(index + '0', LINE2(0));

        /*displaying hours value */
        clcd_putch(log[0], LINE2(2));
        clcd_putch(log[1], LINE2(3));
        clcd_putch(':', LINE2(4));

        /*displaying minutes value */
        clcd_putch(log[2], LINE2(5));
        clcd_putch(log[3], LINE2(6));
        clcd_putch(':', LINE2(7));

        /*displaying seconds value */
        clcd_putch(log[4], LINE2(8));
        clcd_putch(log[5], LINE2(9));

        /*displaying event character */
        clcd_putch(log[6], LINE2(11));
        clcd_putch(log[7], LINE2(12));

        /*displaying speed value */
        clcd_putch(log[8], LINE2(14));
        clcd_putch(log[9], LINE2(15));

        __delay_us(500);
    }
    return SUCCESS;
}

unsigned char clear_logs(unsigned char reset_flag)
{
    clcd_print("LOGS CLEARED", LINE1(2));
    clcd_print("SUCCESSFULLY", LINE2(2));
    if(reset_flag == RESET_CLEAR_LOG)
    {
        pos = -1;
        event_count = -1;
        return SUCCESS;
    }
    return FAILURE;
    
}

void download_logs(void)
{
    int index = -1;
    char log[11];
    log[10] = 0;
    int position = 0;
    unsigned char add;
    if (event_count == -1)
    {
        puts("NO LOGS AVAILABLE");
    }
    else
    {
        puts("DOWLOADED LOGS: ");
        putchar('\n');
        puts("#     Time        Event       Speed");
        putchar('\n');
        putchar('\r');
        
        while(index < event_count)
        {
            position = index + 1;
            index++;
            for(char i = 0; i < 10; i++)
            {
                add = position * 10 + 5 + i;
                log[i] = ext_eeprom_24C02_read(add);
            }
            /* printing all logs in cutecom */

            /* printing index */
            putchar(index + '0');
            puts("   ");

            /*printing hours value */
            putchar(log[0]);
            putchar(log[1]);
            putchar(':');

            /*printing minute  value */
            putchar(log[2]);
            putchar(log[3]);
            putchar(':');

            /*printing second  value */
            putchar(log[4]);
            putchar(log[5]);
            puts("      ");

            /*printing event  character  */
            putchar(log[6]);
            putchar(log[7]);
            puts("            ");

            /*printing speed value */
            putchar(log[8]);
            putchar(log[9]);
            putchar('\n');
            putchar('\r');
        }
    }
}

unsigned char set_time(unsigned char key,unsigned char reset_flag)
{
    static unsigned int new_time[3];
    static unsigned int blink_pos;
    static unsigned char wait;
    static unsigned char blink;
    static char t_done = 0;
    char buffer;
    
    if(reset_flag == RESET_TIME)
    {
        get_time();
        /*Storing values of new time hours */
        new_time[0] = (time[0] & 0x0F) * 10 + (time[1] & 0x0F);

        /*Storing values of new time minute*/
        new_time[1] = (time[2] & 0x0F) * 10 + (time[3] & 0x0F);

        /*Storing values of new time second */
        new_time[2] = (time[4] & 0x0F) * 10 + (time[5] & 0x0F);
        clcd_print("Time (HH:MM:SS)", LINE1(0));
        
        blink_pos = 2;
        wait = 0;
        blink = 0;
        t_done = 0;
        key = ALL_RELEASED;
    }
    if(t_done)
    {
        return FAILURE;
    }
    switch(key)
    {
        case SW4:
            new_time[blink_pos]++;
            break;
        case SW5:
            blink_pos = (blink_pos + 1) % 3;
            break;
        case SW4_LP:
            get_time();
            buffer = ((new_time[0] / 10) << 4) | new_time[0] % 10; 
            clock_reg[0] = (clock_reg[0] & 0xC0) | buffer; 
            write_ds1307(HOUR_ADDR, clock_reg[0]); 


            buffer = ((new_time[1] / 10) << 4) | new_time[1] % 10; 
            clock_reg[1] = (clock_reg[1] & 0x80) | buffer; 
            write_ds1307(MIN_ADDR, clock_reg[1]); 


            buffer = ((new_time[2] / 10) << 4) | new_time[2] % 10; 
            clock_reg[2] = (clock_reg[2] & 0x80) | buffer; 
            write_ds1307(SEC_ADDR, clock_reg[2]); 


            clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
            clcd_print("TIME CHANGED", LINE1(2));
            clcd_print("SUCCESSFULLY", LINE2(2));

        t_done = 1;
        __delay_ms(1000);
        return SUCCESS;
    }
    if (new_time[0] > 23)
        new_time[0] = 0;
    if (new_time[1] > 59)
        new_time[1] = 0;
    if (new_time[2] > 59)
        new_time[2] = 0;
    
    if (wait++ == 1)
    {
        wait = 0;
        blink = !blink;
        /*logic to blink at the current pos*/
        if (blink)
        {
            switch (blink_pos)
            {
                case 0:
                    clcd_print("  ", LINE2(0));
                    __delay_ms(160);
                    break;
                case 1:
                    clcd_print("  ", LINE2(3));
                    __delay_ms(160);
                    break;
                case 2:
                    clcd_print("  ", LINE2(6));
                    __delay_ms(160);
                    break;
            }
        }
    }
    /* Displaying hours field */
    clcd_putch(new_time[0] / 10 + '0', LINE2(0));
    clcd_putch(new_time[0] % 10 + '0', LINE2(1));
    clcd_putch(':', LINE2(2));

    /* Displaying mins field */
    clcd_putch(new_time[1] / 10 + '0', LINE2(3));
    clcd_putch(new_time[1] % 10 + '0', LINE2(4));
    clcd_putch(':', LINE2(5));

    /* Displaying secs field */
    clcd_putch(new_time[2] / 10 + '0', LINE2(6));
    clcd_putch(new_time[2] % 10 + '0', LINE2(7));
    
    return FAILURE;
}

unsigned char change_password(unsigned char key,unsigned char reset_flag)
{
    static char new_pwd[9];
    static int pwd_pos = 0;
    static char pwd_changed = 0;
    static unsigned char toggle_cursor = 0;
    static unsigned int blink_delay = 0;
    
    if (blink_delay++ == 5)
    {
        blink_delay = 0;
        toggle_cursor = !toggle_cursor;
    }
    
    if(reset_flag == RESET_PASSWORD)
    {
        strncpy(new_pwd, "    ", 4);
        pwd_pos = 0;
        pwd_changed = 0;
        return_time = 5;
    }
    
    if (!return_time)
        return RETURN_BACK;
    if (pwd_changed)
        return FAILURE;
    
    if (pwd_pos < 4)
    {
        clcd_print("ENTER NEW PWD:  ", LINE1(0));
        if (toggle_cursor == 0)
        {
            clcd_putch((unsigned char) 0xFF, LINE2(pwd_pos));
        }
        else
        {
            clcd_putch(' ', LINE2(pwd_pos));
        }
    }
    else if (pwd_pos > 3 && pwd_pos < 8)
    {
        clcd_print("RE-ENTER NEW PWD", LINE1(0));
        if (toggle_cursor == 0)
        {
            clcd_putch((unsigned char) 0xFF, LINE2(pwd_pos));
        }
        else
        {
            clcd_putch(' ', LINE2(pwd_pos));
        }
    }
    
    switch (key)
    {
        case SW5: /* storing new password as 0*/
            new_pwd[pwd_pos] = '0';
            clcd_putch('*', LINE2(pwd_pos));
            pwd_pos++;
            return_time = 5;
            if (pwd_pos == 4)
                clcd_print("                 ", LINE2(0));
        break;

        case SW4: /* storing new password as 1 */
            new_pwd[pwd_pos] = '1';
            clcd_putch('*', LINE2(pwd_pos));
            pwd_pos++;
            return_time = 5;
            if (pwd_pos == 4)
                clcd_print("                 ", LINE2(0));
        break;
    }
    if(pwd_pos == 8)
    {
        if(strncmp(new_pwd, &new_pwd[4],4) == 0) /* if both entered password, matched */
        {
            /* successfully changing password */
            new_pwd[8] = 0;
            ext_eeprom_24C02_str_write(0x00, &new_pwd[4]); /* storing new password */
            pwd_pos++;
            clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
            clcd_print("PASSWORD SAVED", LINE1(0));
            clcd_print("SUCCESSFULLY", LINE2(2));
            pwd_changed = 1;
            __delay_ms(1000);
            return SUCCESS;
        }
        else
        {
            clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
            clcd_print("FAILED TO CHANGE", LINE1(0));
            clcd_print("PASSWORD", LINE2(5));
            pwd_changed = 1;
            __delay_ms(1000);
            return SUCCESS;
        }
    }
    return FAILURE;
}