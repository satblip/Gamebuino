#ifndef SETTINGS_C
#define	SETTINGS_C

//GUI
#define KEYBOARD_W 16
#define KEYBOARD_H 8

//screen
#define SCR_CONTRAST 57
#define SCR_CLK 13
#define SCR_DIN 11
#define SCR_DC  9
#define SCR_CS  A2
#define SCR_RST 8

//buzzer
#define VOLUME_GLOBAL_DEFAULT 0
#define VOLUME_GLOBAL_MAX 3
#define VOLUME_CHANNEL_MAX 3

//battery voltage monitor
#define BAT_PIN A6
#define NUM_LVL 4
#define BAT_LVL_CRITIC 3300
#define BAT_LVL_LOW    3400
#define BAT_LVL_MED    3600
#define BAT_LVL_HIGH   3800

//SD card
#define SD_CS A3

//screens back light
#define BACKLIGHT_PIN 10
//auto back-light levels
#define BACKLIGHT_MIN 0
#define BACKLIGHT_MAX 255

//ambient light sensor
#define AMBIENTLIGHT_PIN A7
//auto back-light levels
#define AMBIENTLIGHT_MIN 800 //800
#define AMBIENTLIGHT_MAX 1000 //1000
#define AMBIENTLIGHT_SMOOTHING 16

//number of buttons
#define NUM_BTN         7
//buttons ID
#define BTN_LEFT        0
#define BTN_UP          1
#define BTN_RIGHT       2
#define BTN_DOWN        3
#define BTN_A           4
#define BTN_B           5
#define BTN_C           6
//buttons pins
#define BTN_LEFT_PIN    5
#define BTN_UP_PIN      7
#define BTN_RIGHT_PIN   6
#define BTN_DOWN_PIN    4
#define BTN_A_PIN       A1
#define BTN_B_PIN       2
#define BTN_C_PIN       A0

#endif /* SETTINGS_C */