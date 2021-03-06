/* 
 * File:   Gamebuino.h
 * Author: Rodot
 *
 * Created on October 1, 2013, 5:39 PM
 */

#ifndef GAMEBUINO_H
#define	GAMEBUINO_H

#define load_game (*((void(*)(char*))0x7ffc))

#include <Arduino.h>
#include <avr/pgmspace.h>
#include "settings.c"
#include "Backlight.h"
#include "Buttons.h"
#include "Battery.h"
#include "Sound.h"
#include "Display.h"

class Gamebuino {
public:
    Gamebuino();
    Backlight backlight;
    Buttons buttons;
    Battery battery;
    Sound sound;
    Display display;

	void begin(char* name, const uint8_t *logo);
	void begin(char* name);
    void begin();
    boolean update();
    uint32_t getFrameCount();
    void setFrameRate(uint8_t fps);
    void setTimePerFrame(uint8_t time);
	uint8_t getCpuLoad();
    
    uint16_t freeRam();
    uint16_t frameDurationMicros;
    uint32_t frameStartMicros, frameEndMicros;
    
    int8_t menu(char** items, uint8_t length);
    void keyboard(char* text, uint8_t length);
    void popup(char* text, uint8_t duration);
	void adjustVolume();
	void changeGame();
	
	boolean collidePointRect(int16_t x1, int16_t y1 ,int16_t x2 ,int16_t y2, int16_t w, int16_t h);
	boolean collideRectRect(int16_t x1, int16_t y1, int16_t w1, int16_t h1 ,int16_t x2 ,int16_t y2, int16_t w2, int16_t h2);
    
private:
    uint8_t timePerFrame;
    uint32_t frameCount;
    uint32_t nextFrameMillis;
    void updatePopup();
    char* popupText;
    uint8_t popupTimeLeft;
    void displayBattery();
};

#endif	/* GAMEBUINO_H */

