#ifndef __GAMEASSETS_H__
#define __GAMEASSETS_H__

#include "../GameModel/GameWorld.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/GameBall.h"
#include "../GameModel/PlayerPaddle.h"
#include "../GameModel/GameItem.h"
#include "../GameModel/GameItemTimer.h"

class Texture3D;
class LevelMesh;
class CgFxPostRefract;
class CgFxVolumetricEffect;
class ESPEmitter;

// Compositional classes for asssets
#include "GameWorldAssets.h"
#include "GameESPAssets.h"

// Includes all the models, textures, etc. for the game.
class GameAssets {

private:
	GameWorldAssets* worldAssets;	// World-related assets
	GameESPAssets* espAssets;			// Emitter/Sprite/Particle assets

	// Level-related meshes
	LevelMesh* levelMesh;

	// Regular meshes - these persist throughout the entire game
	Mesh* ball;				// Ball used to break blocks
	Mesh* spikeyBall;	// What happens to the ball when it becomes uber
	Mesh* item;				// Item, picked up by the player paddle

	// Special effects - persistant special effects in the game
	CgFxPostRefract* invisiBallEffect;
	CgFxVolumetricEffect* ghostBallEffect;


	// Item related textures and drawlists
	std::map<std::string, Texture2D*> itemTextures;
	std::map<std::string, Texture2D*> itemTimerTextures;
	std::map<std::string, Texture2D*> itemTimerFillerTextures;

	void DeleteWorldAssets();
	void DeleteLevelAssets();
	void DeleteRegularMeshAssets();
	void DeleteRegularEffectAssets();

	// Asset loading helper functions
	void LoadRegularMeshAssets();
	void LoadRegularEffectAssets();

	void LoadItemTextures();
	void UnloadItemTextures();

public:
	GameAssets();
	~GameAssets();

	void LoadWorldAssets(GameWorld::WorldStyle style);
	void LoadLevelAssets(const GameLevel* level);

	// LoadMinimalAssets(...)			// e.g., Fonts, Loadscreens
	// LoadBasicGameAssets(...)		// e.g., Ball-related, Item-related, Timer-related 

	// Draw functions ******************************************************************************
	void DrawPaddle(const PlayerPaddle& p, const Camera& camera) const;
	void DrawBackground(double dT, const Camera& camera);
	void DrawLevelPieces(const Camera& camera) const;
	void DrawGameBall(double dT, const GameBall& b, const Camera& camera, Texture2D* sceneTex) const;
	void DrawItem(const GameItem& gameItem, const Camera& camera) const;
	void DrawTimers(const std::list<GameItemTimer*>& timers, int displayWidth, int displayHeight);
	void DrawParticleEffects(double dT, const Camera& camera);

	// Public Setter Functions **********************************************************************
	void AddBallBounceESP(const Camera& camera, const GameBall& ball) {
		this->espAssets->AddBallBounceEffect(camera, ball);
	}

	void AddBasicBlockBreakEffect(const Camera& camera, const LevelPiece& block) {
		this->espAssets->AddBasicBlockBreakEffect(camera, block);
	}

	void SetItemEffect(const GameItem& item, bool activate) {
		this->espAssets->SetItemEffect(item, activate);
	}

	// Public Getter Functions **********************************************************************
	LevelMesh* GetLevelMesh() const {
		return this->levelMesh;
	}

private:

	// TODO: MOVE ALL THIS STUFF INTO GAMEVIEWCONSTANTS...

	static const std::string ITEM_MESH;
	static const std::string ITEM_LABEL_MATGRP;
	static const std::string ITEM_END_MATGRP;

	// Item timer colours
	static const Colour ITEM_GOOD_COLOUR;
	static const Colour ITEM_BAD_COLOUR;
	static const Colour ITEM_NEUTRAL_COLOUR;

	// Item-related texture assets
	// TODO: make these into arrays???
	static const std::string ITEM_SLOWBALL_TEXTURE;
	static const std::string ITEM_FASTBALL_TEXTURE;
	static const std::string ITEM_UBERBALL_TEXTURE;
	static const std::string ITEM_INVISIBALL_TEXTURE;

	static const std::string ITEM_TIMER_SLOWBALL_TEXTURE;
	static const std::string ITEM_TIMER_FASTBALL_TEXTURE;
	static const std::string ITEM_TIMER_UBERBALL_TEXTURE;
	static const std::string ITEM_TIMER_INVISIBALL_TEXTURE;

	static const std::string ITEM_TIMER_FILLER_SPDBALL_TEXTURE;
	static const std::string ITEM_TIMER_FILLER_UBERBALL_TEXTURE;
	static const std::string ITEM_TIMER_FILLER_INVISIBALL_TEXTURE;

};

#endif