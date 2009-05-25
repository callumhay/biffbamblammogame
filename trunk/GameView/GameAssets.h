#ifndef __GAMEASSETS_H__
#define __GAMEASSETS_H__

#include "../BlammoEngine/Light.h"

#include "../GameModel/GameWorld.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/GameBall.h"
#include "../GameModel/PlayerPaddle.h"
#include "../GameModel/GameItem.h"
#include "../GameModel/GameItemTimer.h"

class GameModel;
class Texture3D;
class LevelMesh;
class CgFxPostRefract;
class CgFxVolumetricEffect;
class ESPEmitter;
class LivesLeftHUD;

// Compositional classes for asssets
#include "GameWorldAssets.h"
#include "GameESPAssets.h"
#include "GameItemAssets.h"

// Includes all the models, textures, etc. for the game.
class GameAssets {

private:
	GameWorldAssets* worldAssets;	// World-related assets
	GameESPAssets* espAssets;			// Emitter/Sprite/Particle assets
	GameItemAssets* itemAssets;		// Item-related assets (item drops, timers, etc.)

	LivesLeftHUD* lifeHUD;

	// Level-related meshes
	std::map<const GameLevel*, LevelMesh*> loadedLevelMeshes;

	// Regular meshes - these persist throughout the entire game
	Mesh* ball;				// Ball used to break blocks
	Mesh* spikeyBall;	// What happens to the ball when it becomes uber
	Mesh* paddleLaserAttachment;	// Laser attachment for the paddle

	// In-game lights for typical geometry: key, fill and ball
	PointLight fgKeyLight, fgFillLight, ballLight;
	// In-game lights just for the ball
	PointLight ballKeyLight, ballFillLight;
	// In-game lights just for the paddle
	PointLight paddleKeyLight, paddleFillLight;

	// Special effects - persistant special effects in the game
	CgFxPostRefract* invisiBallEffect;
	CgFxVolumetricEffect* ghostBallEffect;

	void DeleteWorldAssets();
	void DeleteRegularEffectAssets();

	// Asset loading helper functions
	void LoadRegularMeshAssets();
	void LoadRegularEffectAssets();

	void ToggleLights(bool turnOn);

public:
	GameAssets();
	~GameAssets();

	void LoadWorldAssets(const GameWorld* world);
	void Tick(double dT);

	// Draw functions ******************************************************************************
	void DrawPaddle(double dT, const PlayerPaddle& p, const Camera& camera) const;

	void DrawSkybox(const Camera& camera);
	void DrawBackgroundModel(const Camera& camera);
	void DrawBackgroundEffects(const Camera& camera);

	void DrawLevelPieces(double dT, const GameLevel* currLevel, const Camera& camera);
	void DrawGameBalls(double dT, GameModel& gameModel, const Camera& camera, Texture2D* sceneTex, const Vector2D& worldT);
	void DrawItem(double dT, const Camera& camera, const GameItem& gameItem) const;
	void DrawTimers(const std::list<GameItemTimer*>& timers, int displayWidth, int displayHeight);

	void DebugDrawLights() const;

	// Public Getter Functions **********************************************************************
	LevelMesh* GetLevelMesh(const GameLevel* currLevel) const {
		std::map<const GameLevel*, LevelMesh*>::const_iterator iter = this->loadedLevelMeshes.find(currLevel);
		if (iter == this->loadedLevelMeshes.end()) {
			assert(false);
			return NULL;
		}
		return iter->second;
	}

	void ActivateItemEffects(const GameModel& gameModel, const GameItem& item, const Camera& camera);
	void DeactivateItemEffects(const GameModel& gameModel, const GameItem& item);

	GameESPAssets* GetESPAssets() const {
		return this->espAssets;
	}

	LivesLeftHUD* GetLifeHUD() const {
		return this->lifeHUD;
	}

};

#endif