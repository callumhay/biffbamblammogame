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
#include "GameItemAssets.h"

// Includes all the models, textures, etc. for the game.
class GameAssets {

private:
	GameWorldAssets* worldAssets;	// World-related assets
	GameESPAssets* espAssets;			// Emitter/Sprite/Particle assets
	GameItemAssets* itemAssets;		// Item-related assets (item drops, timers, etc.)

	// Level-related meshes
	LevelMesh* levelMesh;

	// Regular meshes - these persist throughout the entire game
	Mesh* ball;				// Ball used to break blocks
	Mesh* spikeyBall;	// What happens to the ball when it becomes uber

	Mesh* paddleLaserAttachment;	// Laser attachment for the paddle

	// Special effects - persistant special effects in the game
	CgFxPostRefract* invisiBallEffect;
	CgFxVolumetricEffect* ghostBallEffect;

	void DeleteWorldAssets();
	void DeleteLevelAssets();
	void DeleteRegularMeshAssets();
	void DeleteRegularEffectAssets();

	// Asset loading helper functions
	void LoadRegularMeshAssets();
	void LoadRegularEffectAssets();

public:
	GameAssets();
	~GameAssets();

	void LoadWorldAssets(GameWorld::WorldStyle style);
	void LoadLevelAssets(const GameLevel* level);

	// LoadMinimalAssets(...)				// e.g., just font and load screen
	// LoadStartScreenAssets(...)		// e.g., start-screen + minimal
	// LoadPlayableGameAssets(...)	// e.g., Ball-related, Item-related, Timer-related 

	void Tick(double dT);

	// Draw functions ******************************************************************************
	void DrawPaddle(double dT, const PlayerPaddle& p, const Camera& camera) const;

	void DrawSkybox(const Camera& camera);
	void DrawBackgroundModel(const Camera& camera);
	void DrawBackgroundEffects(const Camera& camera);

	void DrawLevelPieces(const Camera& camera) const;
	void DrawGameBall(double dT, const GameBall& b, const Camera& camera, Texture2D* sceneTex) const;
	void DrawItem(double dT, const Camera& camera, const GameItem& gameItem) const;
	void DrawTimers(const std::list<GameItemTimer*>& timers, int displayWidth, int displayHeight);

	// Public Effects-related functions ************************************************************
	void DrawParticleEffects(double dT, const Camera& camera) {
		this->espAssets->DrawParticleEffects(dT, camera);
	}
	void DrawPostProcessingESPEffects(double dT, const Camera& camera, Texture2D* sceneTex) {
		this->espAssets->DrawPostProcessingESPEffects(dT, camera, sceneTex);
	}
	void AddBallBounceESP(const Camera& camera, const GameBall& ball) {
		this->espAssets->AddBallBounceEffect(camera, ball);
	}
	void AddBasicBlockBreakEffect(const Camera& camera, const LevelPiece& block) {
		this->espAssets->AddBasicBlockBreakEffect(camera, block);
	}
	void AddBombBlockBreakEffect(const Camera& camera, const LevelPiece& bomb) {
		this->espAssets->AddBombBlockBreakEffect(camera, bomb);
	}
	void SetItemEffect(const GameItem& item, bool activate) {
		this->espAssets->SetItemEffect(item, activate);
	}

	void AddItemDropEffect(const Camera& camera, const GameItem& item) {
		this->espAssets->AddItemDropEffect(camera, item);
	}
	void RemoveItemDropEffect(const Camera& camera, const GameItem& item) {
		this->espAssets->RemoveItemDropEffect(camera, item);
	}

	void AddProjectileEffect(const Camera& camera, const Projectile& projectile) {
		this->espAssets->AddProjectileEffect(camera, projectile);
	}
	void RemoveProjectileEffect(const Camera& camera, const Projectile& projectile) {
		this->espAssets->RemoveProjectileEffect(camera, projectile);
	}

	void KillAllActiveEffects() {
		this->espAssets->KillAllActiveEffects();
	}

	// Public Getter Functions **********************************************************************
	LevelMesh* GetLevelMesh() const {
		return this->levelMesh;
	}

};

#endif