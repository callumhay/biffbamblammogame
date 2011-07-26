/**
 * GameProgressIO.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMEPROGRESSIO_H__
#define __GAMEPROGRESSIO_H__

#include "../BlammoEngine/BasicIncludes.h"

class GameModel;

/** 
 * Static class for saving and loading game progress from file.
 */
class GameProgressIO {
public:
    static bool LoadGameProgress(GameModel* model);
    static bool SaveGameProgress(const GameModel* model);

private:
    static const char* PROGRESS_FILENAME;

    GameProgressIO() {};
    ~GameProgressIO() {};
    DISALLOW_COPY_AND_ASSIGN(GameProgressIO);
};

#endif // __GAMEPROGRESSIO_H__