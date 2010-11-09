#ifndef __BBBGAMECONTROLLER_H__
#define __BBBGAMECONTROLLER_H__

#include "../BlammoEngine/BasicIncludes.h"

class GameModel;
class GameDisplay;

/**
 * Abstract superclass for a generalized game controller to be used in a game.
 */
class BBBGameController {
public:
	virtual ~BBBGameController() {};

	enum VibrateAmount { NoVibration = 0, VerySoftVibration, SoftVibration, MediumVibration, HeavyVibration, VeryHeavyVibration };
	virtual void Vibrate(double lengthInSeconds, const VibrateAmount& leftMotorAmt, const VibrateAmount& rightMotorAmt) = 0;

	virtual bool ProcessState()  = 0;
	virtual void Sync(double dT) = 0;

	virtual bool IsConnected() const = 0;

protected:
	BBBGameController(GameModel* model, GameDisplay* display) : model(model), display(display) {
		assert(model != NULL);
		assert(display != NULL);
	};
	
	GameModel* model; 
	GameDisplay* display;

private:
	DISALLOW_COPY_AND_ASSIGN(BBBGameController);
};

#endif // __BBBGAMECONTROLLER_H__