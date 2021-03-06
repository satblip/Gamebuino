/* 
 * File:   Gamebuino.cpp
 * Author: Rodot
 * 
 * Created on October 1, 2013, 5:39 PM
 */

#include "Gamebuino.h"
char loader[] = "LOADER";
PROGMEM uint16_t startupSound[] = {0x1E, 0x10, 0x301E, 0x10, 0x602E, 0x602C, 0x602A, 0x6028, 0x6026, 0x6024, 0x6022, 0x80, 0x0000};

Gamebuino::Gamebuino() {
}

void Gamebuino::begin(char*  name, const uint8_t *logo) {
    timePerFrame = 50;
    nextFrameMillis = 0;
    frameCount = 0;
	frameEndMicros = 1;

    backlight.begin();
    buttons.begin();
    battery.begin();
    sound.begin();
    display.begin(SCR_CLK, SCR_DIN, SCR_DC, SCR_CS, SCR_RST);
	
    backlight.set(BACKLIGHT_MAX);
	display.persistance = true;
	
	uint8_t offset = 0;
	if(logo)
		offset = pgm_read_byte(logo)+2; //offset by the logo width
	display.setCursor(offset,12); 
	display.print(name);
	display.drawBitmap(0, 12, logo);
	
    sound.play(startupSound, 0);
	while(1){
		if(update()){
			if(buttons.pressed(BTN_A))
				break;
			if(buttons.pressed(BTN_B)){
				sound.setGlobalVolume(0);
			}
			if(!sound.getGlobalVolume()){
				display.drawChar(72,31,'x',BLACK, WHITE, 1);
			}
			if(buttons.pressed(BTN_C))
				changeGame();
			
		}
	}
	display.persistance = false;
	battery.show = true;
}

void Gamebuino::begin(char* name){
	begin(name, 0);
}

void Gamebuino::begin(){
	begin("");
}

boolean Gamebuino::update() {
    if (((nextFrameMillis - millis()) > timePerFrame) && frameEndMicros) { //if time to render a new frame is reached and the frame end has ran once
        nextFrameMillis = millis() + timePerFrame;
        frameCount++;

        frameEndMicros = 0;
        frameStartMicros = micros();

        backlight.update();
        buttons.update();
        battery.update();

        return true;

    } else {
        if (!frameEndMicros) { //runs once at the end of the frame

            sound.update();
            updatePopup();
			displayBattery();
            display.update(); //send the buffer to the screen
			if(!display.persistance)
				display.clear(); //clear the buffer

            frameEndMicros = micros(); //measure the frame's end time
            frameDurationMicros = frameEndMicros - frameStartMicros;

            //            display.setTextColor(BLACK);
            //            display.setCursor(0, 40);
            //            display.print(frameDurationMicros / timePerFrame);
            //            display.print(" ");
            //            display.print(2048 - freeRam());

            //            display.setCursor(0, 32);
            //            display.print("CPU:");
            //            display.print(frameDurationMicros / timePerFrame);
            //            display.println("/1000");
            //            display.print("RAM:");
            //            display.print(2048 - freeRam());
            //            display.println("/2048");
        }
        return false;
    }
}

uint32_t Gamebuino::getFrameCount() {
    return frameCount;
}

void Gamebuino::setFrameRate(uint8_t fps) {
    timePerFrame = 1000 / fps;
}

void Gamebuino::setTimePerFrame(uint8_t time) {
    timePerFrame = time;
}

uint8_t Gamebuino::getCpuLoad(){
	return(frameDurationMicros/(10*timePerFrame));
}

uint16_t Gamebuino::freeRam() {
    //from http://www.controllerprojects.com/2011/05/23/determining-sram-usage-on-arduino/
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

int8_t Gamebuino::menu(char** items, uint8_t length) {
#if (ENABLE_GUI > 0)
    int8_t activeItem = 0;
    int8_t currentY = LCDHEIGHT;
    int8_t targetY = 0;
    boolean exit = false;
    int8_t answer = -1;
    while (1) {
        if (update()) {
            if (buttons.pressed(BTN_A) || buttons.pressed(BTN_B) || buttons.pressed(BTN_C)) {
                exit = true; //time to exit menu !
                targetY = -8 * length - 2; //send the menu out of the screen
                if (buttons.pressed(BTN_A)) {
                    answer = activeItem;
                    sound.playOK();
                } else {
                    sound.playCancel();
                }
            }
            if (exit == false) {
                if (buttons.pressed(BTN_DOWN)) {
                    activeItem++;
                    sound.playTick();
                }
                if (buttons.pressed(BTN_UP)) {
                    activeItem--;
                    sound.playTick();
                }
                //don't go out of the menu
                if (activeItem == length) activeItem = 0;
                if (activeItem < 0) activeItem = length - 1;

                targetY = -8 * activeItem + 11; //center the menu on the active item
            } else { //exit :
                if ((currentY - targetY) <= 1)
                    return (answer);
            }
            //draw a fancy menu
            currentY = (currentY + targetY) / 2;
            display.setCursor(0, currentY);
            display.setTextSize(1);
            display.setTextColor(BLACK);
            display.setTextWrap(false);
            for (byte i = 0; i < length; i++) {
                if (i == activeItem)
                    display.setCursor(3, currentY + 8 * activeItem);
                display.println(items[i]);
            }

            //display.fillRect(0, currentY + 3 + 8 * activeItem, 2, 2, BLACK);
            display.drawFastHLine(0, currentY + 8 * activeItem - 1, LCDWIDTH, WHITE);
            display.drawRoundRect(0, currentY + 8 * activeItem - 2, LCDWIDTH, 11, 3, BLACK);
        }
    }
#endif
}

void Gamebuino::keyboard(char* text, uint8_t length) {
#if (ENABLE_GUI > 0)
    memset(text, 0, length); //clear the text
    //active character in the typing area
    int8_t activeChar = 0;
    //selected char on the keyboard
    int8_t activeX = 0;
    int8_t activeY = 2;
    //position of the keyboard on the screen
    int8_t currentX = LCDWIDTH;
    int8_t currentY = LCDHEIGHT;
    int8_t targetX = 0;
    int8_t targetY = 0;

    while (1) {
        if (update()) {
            //move the character selector
            if (buttons.repeat(BTN_DOWN, 4)) {
                activeY++;
                sound.playTick();
            }
            if (buttons.repeat(BTN_UP, 4)) {
                activeY--;
                sound.playTick();
            }
            if (buttons.repeat(BTN_RIGHT, 4)) {
                activeX++;
                sound.playTick();
            }
            if (buttons.repeat(BTN_LEFT, 4)) {
                activeX--;
                sound.playTick();
            }
            //don't go out of the keyboard
            if (activeX == KEYBOARD_W) activeX = 0;
            if (activeX < 0) activeX = KEYBOARD_W - 1;
            if (activeY == KEYBOARD_H) activeY = 0;
            if (activeY < 0) activeY = KEYBOARD_H - 1;
            //set the keyboard position on screen
            targetX = -6 * activeX + LCDWIDTH / 2 - 3;
            targetY = -8 * activeY + LCDHEIGHT / 2 - 4 - 8;
            //smooth the keyboard displacement
            currentX = (targetX + currentX) / 2;
            currentY = (targetY + currentY) / 2;
            //type character
            if (buttons.pressed(BTN_A)) {
                if (activeChar < length) {
					byte thisChar = activeX + KEYBOARD_W * activeY;
					if((thisChar == 10)||(thisChar == 13)) //avoid line feed and carriage return
						continue;
                    text[activeChar] = thisChar;
                }
                activeChar++;
                sound.playOK();
                if (activeChar > length)
                    activeChar = length;
            }
            //erase character
            if (buttons.pressed(BTN_B)) {
                activeChar--;
                sound.playCancel();
                if (activeChar >= 0)
                    text[activeChar] = ' ';
                else
                    activeChar = 0;
            }
            //leave menu
            if (buttons.pressed(BTN_C)) {
			sound.playOK();
			    while (1) {
					if (update()) {
						display.setCursor(0,0);
						display.println("You entered\n");
						for (uint8_t i = 0; i < length; i++) {
							display.print(text[i]);
						}
						display.println("\n\n\nA:okay B:edit");
						if(buttons.pressed(BTN_A)){
							sound.playOK();
							return;
						}
						if(buttons.pressed(BTN_B)){
							sound.playCancel();
							break;
						}
					}
				}
            }
            //draw the keyboard
            for (int8_t y = 0; y < KEYBOARD_H; y++) {
                for (int8_t x = 0; x < KEYBOARD_W; x++) {
                    display.drawChar(currentX + x * 6, currentY + y * 8, x + y * KEYBOARD_W, BLACK, BLACK, 1);
                    //if((x==activeX) && (y == activeY))
                    //display.drawChar(currentX + x * 6, currentY + y * 8, x + y* KEYBOARD_W, WHITE, BLACK, 1);
                }
            }
            //erase some pixels around the selected character
            display.drawFastHLine(currentX + activeX * 6 - 1, currentY + activeY * 8 - 2, 7, WHITE);
            display.drawFastHLine(currentX + activeX * 6 - 1, currentY + activeY * 8 - 1, 7, WHITE);
            display.drawFastHLine(currentX + activeX * 6 - 1, currentY + activeY * 8 + 8, 7, WHITE);
            //draw the selection rectangle
            display.drawRoundRect(currentX + activeX * 6 - 2, currentY + activeY * 8 - 3, 9, 13, 3, BLACK);
            //draw keyboard outline
            display.drawRoundRect(currentX - 6, currentY - 6, KEYBOARD_W * 6 + 12, KEYBOARD_H * 8 + 12, 8, BLACK);
            //text field
            display.drawLine(0, 38, LCDWIDTH, 38, BLACK);
            display.fillRect(0, 39, LCDWIDTH, 9, WHITE);
            //typed text
            display.setCursor(0, 40);
            display.setTextColor(BLACK, BLACK);
            for (uint8_t i = 0; i < length; i++) {
                display.print(text[i]);
            }
            //blinking cursor
            if (((frameCount % 8) < 4) && (activeChar < length))
                display.drawChar(6 * activeChar, 40, '_', BLACK, BLACK, 1);
        }
    }
#endif
}

void Gamebuino::popup(char* text, uint8_t duration){
#if (ENABLE_GUI > 0)
    popupText = text;
    popupTimeLeft = duration;
#endif
}

void Gamebuino::updatePopup(){
#if (ENABLE_GUI > 0)
    if (popupTimeLeft){
        uint8_t yOffset = 0;
        if(popupTimeLeft<12){
            yOffset = 12-popupTimeLeft;
        }
        display.setTextSize(1);
        display.setTextColor(BLACK, WHITE);
        display.fillRoundRect(0,LCDHEIGHT+yOffset-11,84,11,3,WHITE);
        display.drawRoundRect(0,LCDHEIGHT+yOffset-11,84,11,3,BLACK);
        display.setCursor(4, LCDHEIGHT+yOffset-9);
        display.print(popupText);
        popupTimeLeft--;
    }
#endif
}

void Gamebuino::adjustVolume(){
#if (ENABLE_GUI > 0) || (NUM_CHANNELS > 0)
  while(1){
    if(update()==true){
      byte volume = sound.getGlobalVolume();
      display.setTextSize(1);
      display.setTextColor(BLACK, BLACK);
      display.setCursor(24, 16);
      display.println("VOLUME");
      display.drawRoundRect(24,28,36,7,3,BLACK);
      if(volume)
        display.fillRoundRect(24,28,12*volume,7,3,BLACK);
      if(buttons.pressed(BTN_RIGHT) || buttons.pressed(BTN_UP)){
        sound.setGlobalVolume(volume + 1);
        sound.playOK();
      }
      if(buttons.pressed(BTN_LEFT) || buttons.pressed(BTN_DOWN)){
        sound.setGlobalVolume(volume - 1);
        sound.playCancel();
      }
      if(buttons.pressed(BTN_C)){
        break;
      }
    }
  }
#endif
}

void Gamebuino::displayBattery(){
#if (ENABLE_BATTERY > 0)
    display.setTextColor(BLACK, WHITE);
	display.setCursor(79,0);
	if(!battery.getLevel()){
		if((frameCount % 16) < 8) { //blink
			display.fillRect(79,0,5,8,WHITE);
			display.print(char(7));
			if(!(frameCount % 16)){
				sound.playTick();
			}
		}
	}
	else
		if(battery.show){
			display.fillRect(79,0,5,8,WHITE);
			display.print(char(7 + battery.getLevel() - 1));
		}
#endif
}

void Gamebuino::changeGame(){
	display.clear();
	display.print("LOADING...");
	display.update();
	load_game(loader);
	display.persistance = false;
	while(1){
		if(update()){
			display.println("\nNo SD card or\nno LOADER.HEX\n\nA:Exit");
			if(buttons.pressed(BTN_A))
				break;
		}
	}
}

boolean Gamebuino::collidePointRect(int16_t x1, int16_t y1 ,int16_t x2 ,int16_t y2, int16_t w, int16_t h){
  if((x1>=x2)&&(x1<x2+w))
    if((y1>=y2)&&(y1<y2+h))
	  return true;
  return false;
}

boolean Gamebuino::collideRectRect(int16_t x1, int16_t y1, int16_t w1, int16_t h1 ,int16_t x2 ,int16_t y2, int16_t w2, int16_t h2){
  if(collidePointRect(x1, y1, x2, y2, w2, h2))
    return true;
  if(collidePointRect(x1+w1-1, y1, x2, y2, w2, h2))
    return true;
  if(collidePointRect(x1, y1+h1-1, x2, y2, w2, h2))
    return true;
  if(collidePointRect(x1+w1-1, y1+h1-1, x2, y2, w2, h2))
    return true;
  return false;
}