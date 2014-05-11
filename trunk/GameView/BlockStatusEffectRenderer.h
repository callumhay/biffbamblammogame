/**
 * BlockStatusEffectRenderer.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#include "CgFxPostRefract.h"

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

	void Draw(double dT, const Camera& camera, const Texture2D* sceneTexture);

private:
	typedef std::list<ESPEmitter*> EmitterList;
	typedef std::map<LevelPiece::PieceStatus, EmitterList> StatusEffectMap;
	typedef std::map<const LevelPiece*, StatusEffectMap> PieceStatusEffectMap;
	PieceStatusEffectMap pieceStatusEffects;

	// Persistent Textures
	std::vector<Texture2D*> smokePuffTextures;
    std::vector<Texture2D*> cloudTextures;
	Texture2D* gritTexture;
	Texture2D* rectPrismTexture;
	Texture2D* frostTexture;

	// Effects (shaders) used to render various status effects
	CgFxFireBallEffect fireEffect;
	CgFxPostRefract iceBlockEffect;
	
	void SetupTextures();
	
	// Persistent Effects structures
	ESPParticleColourEffector fireColourEffector;
	ESPParticleColourEffector semiFaderEffector;
	ESPParticleColourEffector fullFaderEffector;
	ESPParticleScaleEffector  particleLargeGrowth;
	ESPParticleScaleEffector  particleMediumGrowth;
	ESPParticleRotateEffector smokeRotatorCW;
	ESPParticleRotateEffector smokeRotatorCCW;

	// Effects Factory methods for each of the status effects
	// ... OnFire status effect builders
	ESPPointEmitter* BuildBlockOnFireFlameEffect(const LevelPiece& piece, bool spinCW);
	ESPPointEmitter* BuildBlockOnFireSmokeEffect(const LevelPiece& piece);
	ESPPointEmitter* BuildBlockOnFireScortchEffect(const LevelPiece& piece);

	// ... Ice Cube (Frozen) status effect builders
	ESPPointEmitter* BuildIceBlockEffect(const LevelPiece& piece);
	GLuint BuildIceBlockDL(const LevelPiece& piece);

	DISALLOW_COPY_AND_ASSIGN(BlockStatusEffectRenderer);

};

#endif // __BLOCKSTATUSEFFECTRENDERER_H__