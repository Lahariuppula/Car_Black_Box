/* 
 * File:   car_black_box.h
 * Author: Lahari
 *
 * Created on 19 May, 2026, 10:14 AM
 */

#ifndef CAR_BLACK_BOX_H
#define	CAR_BLACK_BOX_H

void display_dashboard_screen(unsigned char event[], unsigned char speed);
void log_event(unsigned char event[],unsigned char speed);
void clear_screen(void);
unsigned char login_screen(unsigned char key,unsigned char reset_flag);
unsigned char menu_screen(unsigned char key,unsigned char reset_flag);
unsigned char view_logs(unsigned char key, unsigned char reset_flag);
unsigned char clear_logs(unsigned char reset_flag);
void download_logs(void);
unsigned char set_time(unsigned char key,unsigned char reset_flag);
unsigned char change_password(unsigned char key,unsigned char reset_flag);
#endif	/* CAR_BLACK_BOX_H */

