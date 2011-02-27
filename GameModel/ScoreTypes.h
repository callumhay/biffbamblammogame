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