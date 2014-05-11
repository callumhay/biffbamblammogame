/**
 * DecoWorldAssets.h
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

#ifndef __DECOWORLDASSETS_H__
#define __DECOWORLDASSETS_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Animation.h"

#include "GameWorldAssets.h"

#include "../ESPEngine/ESPVolumeEmitter.h"
#include "../ESPEngine/ESPParticleScaleEffector.h"
#include "../ESPEngine/ESPParticleColourEffector.h"
#include "../ESPEngine/ESPParticleRotateEffector.h"

class CgFxVolumetricEffect;

class DecoWorldAssets : public GameWorldAssets {
public:
	DecoWorldAssets(GameAssets* assets);
	~DecoWorldAssets();

	GameWorld::WorldStyle GetStyle() const;
	void DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight, const BasicPointLight& bgFillLight);
	void DrawBackgroundEffects(const Camera& camera);
	void FadeBackground(bool fadeout, float fadeTime);
	void ResetToInitialState();

	void Tick(double dT, const GameModel& model);

    void LoadFGLighting(GameAssets* assets, const Vector3D& fgKeyPosOffset = Vector3D(0,0,0), 
        const Vector3D& fgFillPosOffset = Vector3D(0,0,0)) const;
    void LoadBGLighting(GameAssets* assets) const;

private:

	enum RotationState { RotateCW, RotateCCW };		// CCW is moving in postive degrees, CW is negative...

	double rotationLimitfg, rotationLimitbg;		// Limit of the arc of rotation for the beams
	double rotationSpd;								// Rotation speed of beams in degrees/second
	
	// Current rotation amount of each beam and state
	double beamRotationfg1, beamRotationfg2, beamRotationbg1, beamRotationbg2;				
	RotationState rotationStatefg1, rotationStatefg2, rotationStatebg1, rotationStatebg2;

	// Background meshes and effects specific to deco world
	Mesh* skybeam;
	CgFxVolumetricEffect* beamEffect;

	Texture2D* spiralTexSm;
	Texture2D* spiralTexMed;
	Texture2D* spiralTexLg;
	ESPVolumeEmitter spiralEmitterSm, spiralEmitterMed, spiralEmitterLg;
	ESPParticleRotateEffector rotateEffectorCW, rotateEffectorCCW;
	
	void InitializeEmitters();
	void RotateSkybeams(double dT);

    DISALLOW_COPY_AND_ASSIGN(DecoWorldAssets);
};

inline GameWorld::WorldStyle DecoWorldAssets::GetStyle() const {
	return GameWorld::Deco;
}

/**
 * Fade the background out or in based on the given parameter, the fade
 * will occur over the given amount of time.
 */
inline void DecoWorldAssets::FadeBackground(bool fadeout, float fadeTime) {
    GameWorldAssets::FadeBackground(fadeout, fadeTime);
}

/**
 * Reset the world to its initial state - makes sure that nothing is faded/invisible etc.
 * and is ready to be see for the first time by the player.
 */
inline void DecoWorldAssets::ResetToInitialState() {
    GameWorldAssets::ResetToInitialState();

	this->beamRotationfg1 = 0.0f;
	this->beamRotationfg2 = 0.0f;
	this->beamRotationbg1 = 0.0f;
	this->beamRotationbg2 = 0.0f;
}

#endif