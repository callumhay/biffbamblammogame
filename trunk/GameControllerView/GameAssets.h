#ifndef __GAMEASSETS_H__
#define __GAMEASSETS_H__

#include "../GameModel/GameWorld.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/GameBall.h"
#include "../GameModel/PlayerPaddle.h"

class Mesh;

// Includes all the models, textures, etc. for the game.
class GameAssets {

private:
	int currLoadedStyle;

	Mesh* playerPaddle;
	Mesh* block;
	Mesh* ball;

	void DeleteAssets();
	void LoadDecoStyleAssets();

public:
	GameAssets();
	~GameAssets();

	void LoadAssets(GameWorld::WorldStyle assetSet);

	void DrawLevelPieceMesh(const LevelPiece& p);
	void DrawGameBall(const GameBall& b);
	void DrawPaddle(const PlayerPaddle& p);
};

#endif