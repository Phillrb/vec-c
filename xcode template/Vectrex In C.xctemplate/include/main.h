//
//  main.h
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

#ifndef _MAIN_H
#define _MAIN_H

//
//Used to declare all the functions and structs used in the game
//

#include "stdbool.h" //use 'true' and 'false' instead of '1' and '0'

//Game states
typedef enum {
    GSNone,
    GSTitle
} GameState;

//Events during gameplay
typedef enum {
    GENone
} GameEvent;

//Music states
typedef enum {
    MSPlay,
    MSPlaying,
    MSPause,
    MSPausing,
    MSPaused,
    MSContinue,
    MSStop,
    MSStopping,
    MSStopped
} MusicState;

//Music tracks
typedef enum {
    GMusicTitle
} GMusic;

//Sounds during gameplay
#define noOfSounds 1 //Change when adding / removing sounds here
typedef enum  {
    GSoundExample
} GSound;

//Sound objects and their lengths of play
typedef struct Sound {
    GSound gameSound;
    unsigned int duration;
    unsigned int currentSoundFrame;
} Sound;

//Structure to pass around
typedef struct GameVars {
    GameState gameState;
    unsigned int currentFrame;
    unsigned int currentFrameMultiplier;
    MusicState musicState;
    GMusic musicTrack;
    Sound sounds[noOfSounds];
} GameVars;

//Drawing
void resetText();
void drawSpriteWithScaleAtPos(const int sprite[], int drawScale, int xPos, int yPos);
void drawTitle();
void drawALine();
void drawASprite(GameVars* gameVars);

//Sound
void prepMusic(GameVars* gameVars);
void runMusic(GameVars* gameVars);
void requestPlayMusic(GMusic musicTrack, GameVars* gameVars);
void runSound(GameVars* gameVars);
bool delaySoundEnd(GSound sound, int duration, GameVars* gameVars);
void requestPlaySound(GSound sound, GameVars* gameVars);
void playSound(Sound sound, GameVars* gameVars);
void stopSoundOnVoice3();
void playSoundForDuration(GSound sound, int duration, GameVars* gameVars);

//States
void runGameState(GameVars* gameVars);
void runTitle(GameVars* gameVars);
void moveToNextGameState(GameVars* gameVars, GameEvent gameEvent);
void waitForFramesThenMoveOnToNextState(int maxFrames, GameVars* gameVars);

//DEBUG
#ifdef DEBUG
void debugPrint(char * str);
#endif

#endif
