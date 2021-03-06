///////////////////////////////////// INIT GAME
void initGame(){
  setWorld(world, WORLD_W, WORLD_H, tiles);
  //lastScore = score;
  if(score > highscore[RANKMAX-1]){ //if the score is better than the worse high score
    saveHighscore();
  }
  //scoreDisplayTimeLeft = 64;
  score = 0;
  kills = 0;
  currentWeapon = 0; //magnum
  ammo = 9999;
  nextShot = 0;
  shake_timeLeft = 0;
  playerLife = playerLifeMax;
  boss_freq = BOSSFREQ;
  boss_nextSpawn = boss_freq;
  activeMobs = INITNUMMOBS; //6 initial mobs
  do{
    do{
      playerX = random(0, WORLD_W) * 8;
      playerY = random(0, WORLD_H) * 8;
    }
    while(collideWorld(playerX, playerY, playerW, playerH));
    cameraX = playerX - LCDWIDTH/2 + playerW/2;
    cameraY = playerY - LCDHEIGHT/2 + playerW/2;
  } 
  while(!spawnMobs()); //do that until mobs are spawned without error
  //spawn crate
  spawnCrate();
  //reset bullets
  for(byte thisBullet = 0; thisBullet < NUMBULLETS; thisBullet++){
    bullets_active[thisBullet] = false;
  }
  //reset splashes
  for(byte thisSplash = 0; thisSplash < NUMSPLASH; thisSplash++){
    splash_active[thisSplash] = false;
  }
  blast_lifespan = 0; //reset explosion
  gb.battery.display(false);
  //gb.display.clear();
  //gb.display.setTextColor(BLACK, BLACK);
  //gb.display.setCursor(15,16);
  //gb.display.print("LET'S GO!");
  //displayScore();
  //gb.print("GOOD LUCK!");
  /*gb.setCursor(0,0);
   gb.println("MAIN MISSION");
   gb.println("1.Find a crate");
   gb.println("2.Kill aliens");
   gb.println("3.Go to 1.");
   gb.println("OPTIONAL");
   gb.println("(Stay alive)");*/
  //gb.display.update();
  //delay(500);
}

///////////////////////////////////// PLAY
void play(){
  initGame();
  //while(!gb.buttonPressed(BTN_C)){
  while(true){
    if(gb.update()){
      if(gb.buttons.pressed(BTN_C)){
        gb.sound.playCancel();
        gb.battery.display(true);
        pause();
        gb.battery.display(false);
        //gb.enableDisplayBattery = false;
      }
      boolean moved = false;
      if(gb.buttons.repeat(BTN_RIGHT, 1)){
        playerDir = 3;
        moved = true;
      } 
      else{
        if(gb.buttons.repeat(BTN_LEFT, 1)){
          playerDir = 1;
          moved = true;
        }
      }
      if(gb.buttons.repeat(BTN_DOWN, 1)){
        playerDir = 2;
        moved = true;
      }
      else{
        if(gb.buttons.repeat(BTN_UP, 1)){
          playerDir = 0;
          moved = true;
        }
      }
      if(moved){
        moveXYDS(playerX, playerY, playerDir, playerSpeed);
        if(collideWorld(playerX, playerY, playerW, playerH))
          moveXYDS(playerX, playerY, playerDir, -playerSpeed);
      }
      cameraX = playerX + playerW/2 - LCDWIDTH/2;
      cameraY = playerY + playerH/2 - LCDHEIGHT/2;
      shakeScreen();
      drawWorld(cameraX, cameraY);
      int x, y;
      screenCoord(playerX, playerY, x, y);
      gb.display.drawBitmap(x-1, y-1, playerSprite, playerDir, NOFLIP);
      //gb.fillRect(playerX_screen, playerY_screen, playerW, playerH, BLACK);
      byte thisSprite = 0;
      //if (playerMoved)
      //  thisSprite = (gb.frameCount/4)%8;
      //gb.drawBitmap(LCDWIDTH/2-2, LCDHEIGHT/2-2, playerSprite[thisSprite], 8, 8, playerDir, NOFLIP,  BLACK);
      moveMobs();
      drawMobs();
      if(nextShot)
        nextShot--;
      if(gb.buttons.repeat(BTN_A, 1) && !gb.buttons.repeat(BTN_B, 1)){
        shoot();
      }
      if (gb.buttons.repeat(BTN_B, 1)){
        playerSpeed = 2;
      } 
      else {
        playerSpeed = 1;
      }
      moveBullets();
      drawBullets();
      explode();
      drawSplashes();
      collideCrate();
      drawCrate();
      gb.display.setTextColor(BLACK, BLACK);
      //life remaining
      for(byte i=0; i<=playerLifeMax; i++){
        if(i<=playerLife){
          gb.display.drawBitmap(LCDWIDTH-i*9, 0, fullHeart);
        }
        else{
          gb.display.drawBitmap(LCDWIDTH-i*9, 0, fullHeart);
          gb.display.drawBitmap(LCDWIDTH-i*9, 0, emptyHeart);
        }
      }
      if(!playerLife){
        if((gb.getFrameCount()%2)==0){
          shake_magnitude = 2;
          shake_timeLeft = 1;
        }
      }
      drawAmmoOverlay();
      displayScore();
      for(byte thisMob=0; thisMob<activeMobs; thisMob++){
        if(gb.collideRectRect(mobs_x[thisMob],mobs_y[thisMob], mobs_size[thisMob], mobs_size[thisMob],
        playerX, playerY, playerW, playerH)){
          playerLife--;
          shake_magnitude = 2;
          shake_timeLeft = 4;
          if(mobs_size[thisMob] == boss_size){
            playerLife--;
            shake_magnitude = 3;
            shake_timeLeft = 4;
          }
          gb.sound.play(player_damage_sound, 0);
          //gb.buzz(500,100);
          spawnMob(thisMob);
          if(playerLife < 0){ 
            //gb.display.print("TRY AGAIN!");

            /*for(byte i=0; i<250; i++){
             gb.display.fillRect(8*random(0,1+LCDWIDTH/8),8*random(0,1+LCDHEIGHT/8),8,8,WHITE);
             gb.display.setCursor(12,16);
             gb.display.setTextColor(BLACK, WHITE);
             gb.display.print("GAME OVER!");
             gb.display.update();
             }*/

            byte timer=0;
            while(1){
              if(gb.update()){
                drawMobs();
                drawBullets();
                drawSplashes();
                drawCrate();
                drawAmmoOverlay();
                displayScore();
                drawWorld(cameraX, cameraY);
                gb.display.drawBitmap(x-1, y-1, playerSprite, playerDir, NOFLIP);
                gb.display.setTextColor(BLACK,WHITE);
                gb.display.fillRect(0,0,timer*2,LCDHEIGHT,WHITE);
                gb.display.fillRect(LCDWIDTH-timer*2,0,timer*2,LCDHEIGHT,WHITE);
                gb.display.setCursor(12,1);
                gb.display.print("GAME OVER!");
                timer++;
                if(timer==((LCDWIDTH/4)+10))
                  break;
              }
            }
            while(1){
              if(gb.update()){
                gb.display.setTextColor(BLACK, BLACK);
                if(score > highscore[RANKMAX-1]){ //if the score is better than the worse high score
                  gb.display.setCursor(2+random(0,2),0+random(0,2));
                  gb.display.print("NEW HIGHSCORE");
                }
                else{ 
                  gb.display.setCursor(12,1);
                  gb.display.print("GAME OVER!");
                }
                gb.display.setCursor(0,12);
                gb.display.print("You made $");
                gb.display.print(score);
                gb.display.print("\nby killing\n");
                gb.display.print(kills);
                gb.display.print(" crabs.");
                gb.display.setCursor(0,40);
                gb.display.print("A:accept");
                if(gb.buttons.pressed(BTN_A)){
                  gb.sound.playOK();
                  break;
                }
              }
            }
            initGame();
            break;
          }
        }
      }

    }
  } 
}

///////////////////////////////////// DISPLAY SCORE
void displayScore(){
  gb.display.setTextColor(BLACK, WHITE);
  gb.display.setCursor(0,0);
  //gb.display.print(gb.frameDurationMicros);
  //gb.display.print(gb.freeRam());
  gb.display.print('$');
  gb.display.println(score);
  //gb.display.println(gb.getCpuLoad());
  /*gb.display.print(kills);
   gb.display.print("/");
   gb.display.print(activeMobs);*/

  /*if(scoreDisplayTimeLeft){
   scoreDisplayTimeLeft--;
   gb.display.setTextColor(BLACK, WHITE);
   if(lastScore){
   gb.display.setCursor(0,40);
   gb.display.print("You made $");
   gb.display.print(lastScore);
   gb.display.print("    ");
   }
   }*/
}

///////////////////////////////////// SHAKE SCREEN
void shakeScreen(){
  if(shake_timeLeft){
    shake_timeLeft--;
    cameraX += random(-shake_magnitude,shake_magnitude+1);
    cameraY += random(-shake_magnitude,shake_magnitude+1);
  }
}

///////////////////////////////////// LOAD HIGHSCORE
void loadHighscore(){
  for(byte thisScore = 0; thisScore < RANKMAX; thisScore++){
    for(byte i=0; i<NAMELENGTH; i++){
      name[thisScore][i] = EEPROM.read(i + thisScore*(NAMELENGTH+2));
    }
    highscore[thisScore] = EEPROM.read(NAMELENGTH + thisScore*(NAMELENGTH+2)) & 0x00FF; //LSB
    highscore[thisScore] += (EEPROM.read(NAMELENGTH+1 + thisScore*(NAMELENGTH+2)) << 8) & 0xFF00; //MSB
    highscore[thisScore] = (highscore[thisScore]==0xFFFF) ? 0 : highscore[thisScore];
  }
}  

///////////////////////////////////// SAVE HIGHSCORE
void saveHighscore(){
  gb.keyboard(name[RANKMAX-1], NAMELENGTH);
  highscore[RANKMAX-1] = score;
  for(byte i=RANKMAX-1; i>0; i--){ //bubble sorting FTW
    if(highscore[i-1] < highscore[i]){
      char tempName[NAMELENGTH];
      assignArray(tempName, name[i-1], NAMELENGTH);
      assignArray(name[i-1], name[i], NAMELENGTH);
      assignArray(name[i], tempName, NAMELENGTH);
      unsigned int tempScore;
      tempScore = highscore[i-1];
      highscore[i-1] = highscore[i];
      highscore[i] = tempScore;
    }
    else{
      break;
    }
  }
  for(byte thisScore = 0; thisScore < RANKMAX; thisScore++){
    for(byte i=0; i<NAMELENGTH; i++){
      EEPROM.write(i + thisScore*(NAMELENGTH+2), name[thisScore][i]);
    }
    EEPROM.write(NAMELENGTH + thisScore*(NAMELENGTH+2), highscore[thisScore] & 0x00FF); //LSB
    EEPROM.write(NAMELENGTH+1 + thisScore*(NAMELENGTH+2), (highscore[thisScore] >> 8) & 0x00FF); //MSB
  }
  displayHighScores();
}

























