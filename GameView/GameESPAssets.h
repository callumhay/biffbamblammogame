#ifndef __GAMESPASSETS_H__
#define __GAMESPASSETS_H__

#include <list>

#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Camera.h"

#include "../ESPEngine/ESPParticleColourEffector.h"
#include "../ESPEngine/ESPParticleScaleEffector.h"

class LevelPiece;
class GameBall;
class ESPEmitter;
class Texture2D;

/**
 * Stores, draws and changes emitter/sprite/particle assets for the game.
 */
class GameESPAssets {

private:
	// Currently active particle systems
	std::list<ESPEmitter*> activeParticleEmitters;

	// Standard effectors for the various ESP effects
	ESPParticleColourEffector particleFader;
	ESPParticleScaleEffector particleSmallGrowth;

	// Various textures used as particles for various effects
	std::vector<Texture2D*> bangTextures;

	void InitESPTextures();

public:
	GameESPAssets();
	~GameESPAssets();

	// Specific effects that occur in the game
	void AddBallBounceESP(const GameBall& ball);
	void AddBasicBlockBreakEffect(const LevelPiece& block);

	void DrawParticleEffects(double dT, const Camera& camera);

};
#endif