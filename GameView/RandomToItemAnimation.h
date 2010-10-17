#ifndef __RANDOMTOITEMANIMATION_H__
#define __RANDOMTOITEMANIMATION_H__

#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Animation.h"
#include "../GameModel/GameItem.h"

class Texture;
class Texture2D;
class GameModel;
class Camera;

/**
 * Class that provides the animation for showing a random item (just picked up by the paddle)
 * turning into an actual item.
 */
class RandomToItemAnimation {
public:
	RandomToItemAnimation();
	~RandomToItemAnimation();

	void Start(const Texture* gameItemTexture, const GameModel& gameModel);
	void Stop();

	void Draw(const Camera& camera, double dT);

	bool GetIsActive() const { return this->isAnimating; }

private:
	Texture2D* randomItemTexture;
	const Texture* currItemFromRandomTexture;

	bool isAnimating;
	AnimationLerp<float> initialFadeInAnim;
	AnimationLerp<float> randomFadeOutItemFadeInAnim;	// Holds a value 1 to 0 (1 is full alpha for fade out item and no alpha for fade in)
	AnimationLerp<float> finalFadeOutAnim;
	AnimationLerp<Point2D> itemMoveAnim;
};

#endif // __RANDOMTOITEMANIMATION_H__