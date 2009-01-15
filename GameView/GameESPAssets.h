#ifndef __GAMESPASSETS_H__
#define __GAMESPASSETS_H__

#include <list>

#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Camera.h"

#include "../ESPEngine/ESPParticleColourEffector.h"
#include "../ESPEngine/ESPParticleScaleEffector.h"

class LevelPiece;
class GameBall;
class GameItem;
class ESPEmitter;
class ESPPointEmitter;
class Texture2D;

/**
 * Stores, draws and changes emitter/sprite/particle assets for the game.
 */
class GameESPAssets {

private:
	// Currently active particle systems
	std::list<ESPEmitter*> activeGeneralEmitters;

	// Standard effectors for the various ESP effects
	ESPParticleColourEffector particleFader;

	ESPParticleColourEffector particleFaderUberballTrail;
	ESPParticleScaleEffector particlePulseUberballAura;

	ESPParticleScaleEffector particleSmallGrowth;
	ESPParticleScaleEffector particleMediumGrowth;


	// Various textures used as particles for various effects
	std::vector<Texture2D*> bangTextures;
	Texture2D* circleGradientTex;
	//Texture2D* starTex;
	//Texture2D* starOutlineTex;
	//Texture2D* circleBullseyeTex;
	//Texture2D* circleOutlineTex;

	// Standalone ESP effects
	//ESPPointEmitter* ghostBallEmitterAura;
	ESPPointEmitter* uberBallEmitterAura;
	ESPPointEmitter* uberBallEmitterTrail;

	// Used for tweaking...
	Vector2D oldBallDir;

	// Initialization functions for effect stuffs
	void InitESPTextures();
	void InitStandaloneESPEffects();

public:
	GameESPAssets();
	~GameESPAssets();

	// Specific effects that can be made to occur in the game
	void AddBallBounceEffect(const Camera& camera, const GameBall& ball);
	void AddBasicBlockBreakEffect(const Camera& camera, const LevelPiece& block);
	void AddItemAcquiredEffect(const Camera& camera, const GameItem& item);
	void SetItemEffect(const GameItem& item, bool activate);


	// Draw functions for various particle effects in the game
	void DrawParticleEffects(double dT, const Camera& camera);
	void DrawItemDropEffects(double dT, const Camera& camera, const GameItem& item);
	void DrawUberBallEffects(double dT, const Camera& camera, const GameBall& ball);



};
#endif