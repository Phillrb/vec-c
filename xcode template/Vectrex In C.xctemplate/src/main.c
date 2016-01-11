//
//  main.c
//
//  Created by Phillip Riscombe-Burton on 05/04/2015.
//  Copyright (c) 2015 Phillip Riscombe-Burton. All rights reserved.
//
//	Permission is hereby granted, free of charge, to any person obtaining a
//	copy of this software and associated documentation files (the
//	"Software"), to deal in the Software without restriction, including
//	without limitation the rights to use, copy, modify, merge, publish,
//	distribute, sublicense, and/or sell copies of the Software, and to
//	permit persons to whom the Software is furnished to do so, subject to
//	the following conditions:
//
//	The above copyright notice and this permission notice shall be included
//	in all copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "vectrex.h"
#include "main.h"
#include "graphics.h"
#include "stdbool.h"
#include "sounds.h"
#include "tunes.h"

// NOTE: By using -mint8 compiler flag there is no need to explictly use
// the int8 type all the time

//DRAWING LIMITS
const int drawScaleSprite = 0x40;
const int drawScaleScreen = 0xFF;
const int screenMaxFromCentre = 63;
const int menuXpos = -40;

//
//The 'cartridge init block' in crt0.s executes and then calls this main function.
//You can change the startup music, title and info in that block.
//

// Main entry point and game loop
int main()
{
    //Put printStr char height and width back to defaults (after splash changed them)
    charHeight = 0xf8;
    charWidth = 0x50;
    
    //Initialize settings that get passed around everywhere
    GameVars gameVars = {
        GSTitle,    //state
        0,          //frame
        0,          //frame multiplier
        MSPlay,   //Music state - no tune on start up credits
        GMusicTitle, //Current music track
        //Sounds array - index must match enum
        GSoundExample, 5, 0 //sound name, duration, frame
    };

    //Setup joysticks
    //p1LR, P1UD, P2LR, P2UD
    joyEnableFlags(true, true, false, false);
    joyAnalogResolution(0x00); //v accurate
    
    //MAIN LOOP
    while (true)
    {
        //Music selection and preparation
        prepMusic(&gameVars);
        
        // wait for frame boundary (one frame = 30,000 cyles = 50 Hz)
        waitRecal();
        
        //Music execution
        runMusic(&gameVars);
        
        //Execute current state
        runGameState(&gameVars);

        //Game sound effects and music management
        runSound(&gameVars);
    }
}



/* 
 * STATE ENGINE
 */

void runGameState(GameVars* gameVars)
{
    //Game State manager
    switch (gameVars->gameState) {
        
        //------------------------------
        //TODO: Declare states here
        //------------------------------
            
        //Title screen
        case GSTitle:
        {
            runTitle(gameVars);
        }
            break;
        
        default:
            break;
    }
}

//Deal with transcient game states
void moveToNextGameState(GameVars* gameVars, GameEvent gameEvent)
{
    switch (gameVars->gameState) {
        
        //------------------------------
        //TODO: Create state flow here
        //------------------------------
            
        //Title to Menu
        case GSTitle: {
            //TODO change state: gameVars->gameState = GS...
            gameVars->musicState = MSStop;
            
        } break;
         
        default: break;
    }

    //Run new state
    gameVars->currentFrame = 0; //Reset frame
    gameVars->currentFrameMultiplier = 0; //Reset frame multiplier
}

void waitForFramesThenMoveOnToNextState(int maxFrames, GameVars* gameVars)
{
    //pause until limit
    gameVars->currentFrame++;
    if(gameVars->currentFrame >= maxFrames)
    {
        moveToNextGameState(gameVars, GENone);
    }
}

/*
 *  TITLE SCREEN
 */

void runTitle(GameVars* gameVars)
{
#ifdef DEBUG
    debugPrint("TITLE\x80");
#endif
    
    //Display Title
    drawTitle();
    
    //Examples
    drawALine();
    drawASprite(gameVars);
    
    //Display Title for duration of tune - then move on
    waitForFramesThenMoveOnToNextState(80, gameVars);
}

/*
 * SOUND
 */

//Music selection
void prepMusic(GameVars* gameVars)
{

    //Take action on transcient music states
    switch(gameVars->musicState)
    {
        case MSPlay:
        {
            musicFlag = true;
            gameVars->musicState = MSPlaying;
        }
            break;

        case MSPause:
        {
            musicFlag = false;
            gameVars->musicState = MSPausing;
            clearSound();
        }
            break;
            
        case MSContinue:
        {
            musicFlag = 0x80;
            gameVars->musicState = MSPlaying;
        }
            break;
            
        case MSStop:
        {
            musicFlag = false;
            gameVars->musicState = MSStopping;
            clearSound();
        }
            break;
            
        default:
        {
            
        }
            break;
    }
    
    //Continue to select song to play
    if(gameVars->musicState == MSPlaying)
    {
        //------------------------------
        //TODO DECLARE TUNES HERE !!!!
        //------------------------------
        
        switch (gameVars->musicTrack) {
            case GMusicTitle:
            {
                //inbuilt music - custom music doesn't need adst and twang here
                adsr = adsr_8;
                twang = twang_flat;
                initMusicCHK(music_8);
            }
                break;

            default:
                break;
        }
    }
}

//Play any music
void runMusic(GameVars* gameVars)
{
    //Execute music commands
    switch (gameVars->musicState) {
        case MSPlaying:
        case MSStopping:
        case MSPausing:
        {
            doSound();
        }
            break;
            
        default:
            break;
    }
    
    //Move transcient music states on
    switch (gameVars->musicState) {
        case MSStopping:
        {
            gameVars->musicState = MSStopped;
        }
            break;
            
        case MSPausing:
        {
            gameVars->musicState = MSPaused;
        }
            break;
            
        default:
            break;
    }

}

//Request a new music track to play
void requestPlayMusic(GMusic musicTrack, GameVars* gameVars)
{
    gameVars->musicTrack = musicTrack;
    gameVars->musicState = MSPlay;
}

//In-game sound effects are using voice 3

void runSound(GameVars* gameVars)
{
    //Check through all sounds and see if any are playing
    int i=0;
    while (i < noOfSounds)
    {
        //Sound about to start?
        if (gameVars->sounds[i].currentSoundFrame == 1)
        {
            playSound(gameVars->sounds[i], gameVars);
        }
        
        //Sound playing?
        if (gameVars->sounds[i].currentSoundFrame > 0) {
            //Play or stop playing
            playSoundForDuration((GSound)i, gameVars->sounds[i].duration, gameVars);
        }
        i++;
    }
}

//Will stop a sound when it expires
void playSoundForDuration(GSound sound, int duration, GameVars* gameVars)
{
    //Play sound for its number of frames
    if(delaySoundEnd(sound, duration, gameVars))
    {
        stopSoundOnVoice3();
    }
}

//Check for sound end
bool delaySoundEnd(GSound sound, int duration, GameVars* gameVars)
{
    ((Sound)gameVars->sounds[sound]).currentSoundFrame++;
    if(((Sound)gameVars->sounds[sound]).currentSoundFrame >= duration)
    {
        ((Sound)gameVars->sounds[sound]).currentSoundFrame = 0;
        return true;
    }
    return false;
}

//Stop all sounds on voice 3
void stopSoundOnVoice3()
{
    soundByte(SCAllVoicesOff, SCVoiceGenerators); //Turn off all voices
    soundByte(0, SCVoice3Volume);    //volume on voice 3
}

//Mark sound as playing - use GSound as index
void requestPlaySound(GSound sound, GameVars* gameVars)
{
    ((Sound)gameVars->sounds[sound]).currentSoundFrame = 1;
}

//Play correct noise for sound on voice 1
void playSound(Sound sound, GameVars* gameVars)
{
    soundByte(SCVoice3Only, SCVoiceGenerators); //Turn on voice 3 only
    soundByte(15, SCVoice3Volume);    //full volume on voice 3
    
    //------------------------------
    //TODO DECLARE SOUNDS HERE!!!
    //------------------------------
    
    switch (sound.gameSound) {
        case GSoundExample:
        {
            //Play sound off bat
            soundByte(GS6, SCVoice3Data);  //Note to voice 3
        }
            break;
        
        default:
            break;
    }
}


/*
 * DRAWING
 */

//Draw 'title'
void drawTitle()
{
    resetText();
    moveToD(-22, 0);
    printStr("TITLE!\x80");
}

//Underline 'title' as example
void drawALine()
{
    reset0Ref();
    moveToD(-15, -7);
    drawLineD(31, 0);
}

//Draw the example square sprite at different sizes
void drawASprite(GameVars* gameVars)
{
    drawSpriteWithScaleAtPos(exampleSprite, gameVars->currentFrame, 0, -20);
}

//Generic draw sprite
void drawSpriteWithScaleAtPos(const int sprite[], int drawScale, int xPos, int yPos)
{
    reset0Ref();
    scale = drawScaleScreen;
    moveToD(xPos, yPos);
    scale = drawScale;
    
    drawVLp(sprite,0);
}

void resetText()
{
    reset0Ref();
    intensA(0x5f);
    scale = drawScaleScreen;
}

#ifdef DEBUG
void debugPrint(char * str)
{
    resetText();
    moveToD(-40, -60);
    intensA(0x5f);
    printStr(str);
}
#endif
