#ifndef __GAMEASSETS_H__
#define __GAMEASSETS_H__

#include "../GameModel/GameWorld.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/GameBall.h"
#include "../GameModel/PlayerPaddle.h"

#include <map>

class Mesh;

// Includes all the models, textures, etc. for the game.
class GameAssets {

private:
	GameWorld::WorldStyle currLoadedStyle;

	std::map<GameWorld::WorldStyle, Mesh*>::iterator meshIter;
	std::map<GameWorld::WorldStyle, Mesh*> playerPaddles;
	std::map<GameWorld::WorldStyle, Mesh*> blocks;

	//Mesh* playerPaddle;
	//Mesh* block;
	Mesh* ball;

	void DeleteAssets();
	void LoadDecoStyleAssets();
	void LoadCyberpunkStyleAssets();

public:
	GameAssets();
	~GameAssets();

	void LoadAllAssets();
	void SetCurrentAssetStyle(GameWorld::WorldStyle style) {
		this->currLoadedStyle = style;
	}

	void DrawLevelPieceMesh(const LevelPiece& p);
	void DrawGameBall(const GameBall& b);
	void DrawPaddle(const PlayerPaddle& p);
};

#endif