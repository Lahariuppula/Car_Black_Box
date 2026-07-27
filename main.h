/* 
 * File:   main.h
 * Author: Lahari
 *
 * Created on 19 May, 2026, 9:47 AM
 */


#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#include "adc.h"
#include "clcd.h"
#include "digital_keypad.h"
#include "ds1307.h"
#include "i2c.h" 
#include "car_black_box.h"
#include <string.h>
#include "EEPROM.h"
#include "timers.h"
#include "uart.h"

#define DASHBOARD_SCREEN        0x00    /* when in dashboard screen */
#define LOGIN_SCREEN            0x01    /* when in login screen */
#define MAIN_MENU_SCREEN        0x02    /* when in menu screen */

#define VIEW_LOGS               0x03    /* when in view log screen */
#define CLEAR_LOGS              0x04    /* when in clear log screen */
#define DOWNLOAD_LOGS           0x05    /* when in download log screen */
#define CHANGE_PASSWORD      	0x06    /* when in change password screen */
#define SET_TIME    	 		0x07    /* when in set time screen */

#define RESET_NOTHING           0x10    
#define RESET_MENU              0x20    
#define RESET_LOGIN_SCREEN      0x30   
#define RESET_VIEW_LOG          0x40
#define RESET_CLEAR_LOG         0x50
#define RESET_PASSWORD          0x60
#define RESET_TIME              0x70

#define RETURN_BACK             0x0A    /* login failed, return back to dashboard */
#define LOGIN_SUCCESS           0x0B    /* password matched , login successful */
#define SUCCESS                 0x0C
#define FAILURE                 0x0D
#endif	/* MAIN_H */

