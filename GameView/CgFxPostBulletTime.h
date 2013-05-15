/**
 * CgFxPostBulletTime.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __CGFXPOSTBULLETTIME_H__
#define __CGFXPOSTBULLETTIME_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/CgFxEffect.h"

#include "../GameModel/BallBoostModel.h"

class CgFxPostBulletTime : public CgFxPostProcessingEffect {
public:
	CgFxPostBulletTime(FBObj* outputFBO);
	~CgFxPostBulletTime();

    void UpdateBulletTimeState(const BallBoostModel& boostModel);
    void UpdateAndDraw(double dT, const BallBoostModel* boostModel, FBObj*& sceneIn, FBObj*& renderOut);
    
private:
    static const char* POSTBULLETTIME_TECHNIQUE_NAME;

    // Texture sampler parameters
    CGparameter sceneSamplerParam;
    // Tweakable parameters
    CGparameter desaturateFractionParam;    // How much black and white we want (0 is none, 1 is all)
    CGparameter sampleDistanceParam;        // Discretization of the radial blur
    CGparameter sampleStrengthParam;        // Amount of radial blur

    // Parameter values
    AnimationLerp<float> desaturateFrac;
    AnimationLerp<float> sampleDistance;
    AnimationLerp<float> sampleStrength;

	FBObj* resultFBO;

    void Draw(int screenWidth, int screenHeight, double dT);

    DISALLOW_COPY_AND_ASSIGN(CgFxPostBulletTime);
};

#endif // __CGFXPOSTBULLETTIME_H__