/**
 * ScoreTypes.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __SCORETYPES_H__
#define __SCORETYPES_H__

#include "../BlammoEngine/Point.h"

class ScoreTypes {
public:
    enum BonusTypes { UndefinedBonus = 0, PaddleBullseyeBonus, PaddleDaredevilBonus, SpeedyPaddleBonus };

private:
    ScoreTypes() {};
    ~ScoreTypes() {};

    DISALLOW_COPY_AND_ASSIGN(ScoreTypes);
};

#endif // __SCORETYPES_H__