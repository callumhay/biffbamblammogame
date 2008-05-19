#ifndef __GAMESTATE_H__
#define __GAMESTATE_H__

class GameModel;

class GameState {
protected:
	GameModel* gameModel;

public:
	GameState(GameModel* gm) : gameModel(gm) {}
	virtual ~GameState() {}

	virtual void Tick(double seconds) = 0;
	virtual void BallReleaseKeyPressed() = 0;
	virtual void MovePaddleKeyPressed(float dist);
	


};
#endif