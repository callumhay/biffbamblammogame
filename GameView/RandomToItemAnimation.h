/**
 * RandomToItemAnimation.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __RANDOMTOITEMANIMATION_H__
#define __RANDOMTOITEMANIMATION_H__

#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Animation.h"
#include "../GameModel/GameItem.h"

class Texture;
class Texture2D;
class GameModel;
class Camera;
class GameESPAssets;
class ESPPointEmitter;

/**
 * Class that provides the animation for showing a random item (just picked up by the paddle)
 * turning into an actual item.
 */
class RandomToItemAnimation {
public:
	RandomToItemAnimation();
	~RandomToItemAnimation();

	void Start(const GameItem& actualItem, const Texture* gameItemTexture,
        const GameModel& gameModel, GameESPAssets* espAssets);
	void Stop();

	void Draw(const Camera& camera, double dT);

	bool GetIsActive() const { return this->isAnimating; }

private:
	const Texture* currItemFromRandomTexture;

	bool isAnimating;
	AnimationLerp<float> fadeAnim;
    AnimationLerp<float> scaleAnim;
	AnimationLerp<Point2D> itemMoveAnim;
    ESPPointEmitter* itemNameEffect;
};

#endif // __RANDOMTOITEMANIMATION_H__