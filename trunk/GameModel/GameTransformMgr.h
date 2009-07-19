#ifndef __GAMETRANSFORMMGR_H__
#define __GAMETRANSFORMMGR_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Animation.h"

/**
 * Class for delegating interpolation and transformation work and data
 * from the game model - this will handle cases of transforming the level
 * by flipping it, rotating it, moving it around, etc.
 */
class GameTransformMgr {

private:
	enum TransformAnimationType { UpsideDownFlip, PaddleCam };

	static const double SECONDS_TO_FLIP;
	static const double SECONDS_TO_ENTEROREXIT_PADDLECAM;

	bool isFlipped;
	float currGameDegRotX, currGameDegRotY;
	std::map<TransformAnimationType, std::list<AnimationLerp<float>>> animations;

public:
	GameTransformMgr();
	~GameTransformMgr();

	void FlipGameUpsideDown();
	void SetPaddleCamera(bool putCamInsidePaddle);

	void Tick(double dT);

	Matrix4x4 GetGameTransform() const;

};
#endif