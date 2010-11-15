/**
 * BlockStatusEffectRenderer.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009-2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BLOCKSTATUSEFFECTRENDERER_H__
#define __BLOCKSTATUSEFFECTRENDERER_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Camera.h"

#include "../ESPEngine/ESPParticleColourEffector.h"
#include "../ESPEngine/ESPParticleScaleEffector.h"
#include "../ESPEngine/ESPParticleRotateEffector.h"

#include "../GameModel/LevelPiece.h"

#include "CgFxFireBallEffect.h"

class ESPEmitter;
class ESPPointEmitter;
class Texture2D;

/**
 * Class responsible for drawing status effects on blocks in the currently
 * loaded level of the game.
 */
class BlockStatusEffectRenderer {
public:
	BlockStatusEffectRenderer();
	~BlockStatusEffectRenderer();

	void AddLevelPieceStatus(const LevelPiece& piece, const LevelPiece::PieceStatus& status);
	void RemoveLevelPieceStatus(const LevelPiece& piece, const LevelPiece::PieceStatus& status);
	void RemoveAllLevelPieceStatus(const LevelPiece& piece);

	void Draw(double dT, const Camera& camera);

private:
	typedef std::list<ESPEmitter*> EmitterList;
	typedef std::map<LevelPiece::PieceStatus, EmitterList> StatusEffectMap;
	typedef std::map<const LevelPiece*, StatusEffectMap> PieceStatusEffectMap;
	PieceStatusEffectMap pieceStatusEffects;

	// Persistant Textures
	std::vector<Texture2D*> smokePuffTextures;
	Texture2D* fireMiasma;
	Texture2D* gritTexture;
	CgFxFireBallEffect fireEffect;
	
	void SetupTextures();
	
	


	// Persistant Effects structures
	ESPParticleColourEffector fireColourEffector;
	ESPParticleColourEffector semiFaderEffector;
	ESPParticleColourEffector fullFaderEffector;
	ESPParticleScaleEffector  particleLargeGrowth;
	ESPParticleScaleEffector  particleMediumGrowth;
	ESPParticleRotateEffector smokeRotatorCW;
	ESPParticleRotateEffector smokeRotatorCCW;

	// Effects Factory methods for each of the status effects
	ESPPointEmitter* BuildBlockOnFireFlameEffect(const LevelPiece& piece, bool spinCW);
	ESPPointEmitter* BuildBlockOnFireSmokeEffect(const LevelPiece& piece);
	ESPPointEmitter* BuildBlockOnFireScortchEffect(const LevelPiece& piece);

	DISALLOW_COPY_AND_ASSIGN(BlockStatusEffectRenderer);

};

#endif // __BLOCKSTATUSEFFECTRENDERER_H__