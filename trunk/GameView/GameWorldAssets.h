/**
 * GameWorldAssets.h
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

#ifndef __GAMEWORLDASSETS_H__
#define __GAMEWORLDASSETS_H__

#include "GameViewConstants.h"
#include "Skybox.h"

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Mesh.h"
#include "../BlammoEngine/Light.h"
#include "../ResourceManager.h"
#include "../GameModel/GameWorld.h"
#include "../GameModel/PlayerPaddle.h"

class GameAssets;

// Abstract class for game world assets - this includes all
// the specific meshes, effects, etc. associated with a particular
// game world.
class GameWorldAssets {
public:
	GameWorldAssets(GameAssets* assets, Skybox* skybox, Mesh* bg, Mesh* paddle, Mesh* styleBlock);
	virtual ~GameWorldAssets();

	Mesh* GetWorldStyleBlock() const {
		return this->styleBlock;
	}

	Skybox* GetSkybox() const {
		return this->skybox;
	}

	virtual void Tick(double dT, const GameModel&) {
        this->bgFadeAnim.Tick(dT);
	    this->currBGMeshColourAnim.Tick(dT);
	};

	virtual void DrawSkybox(const Camera& camera) {
		this->skybox->Draw(camera);
	}

	virtual GameWorld::WorldStyle GetStyle() const = 0;

	virtual void DrawBackgroundEffects(const Camera& camera) = 0;
	virtual void DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight, const BasicPointLight& bgFillLight) = 0;
    virtual void FastDrawBackgroundModel();

    virtual void DrawBackgroundPostOutlinePreEffects(const Camera& camera) { UNUSED_PARAMETER(camera); }

	virtual void FadeBackground(bool fadeout, float fadeTime);
	virtual void ResetToInitialState();

    virtual void LoadFGLighting(GameAssets* assets, const Vector3D& fgKeyPosOffset = Vector3D(0,0,0), 
        const Vector3D& fgFillPosOffset = Vector3D(0,0,0)) const;
    virtual void LoadBGLighting(GameAssets* assets) const;
    void LoadLightingForLevel(GameAssets* assets, const GameLevel& level) const;
    
	void DrawPaddle(const PlayerPaddle& p, const Camera& camera, CgFxEffectBase* replacementMat, 
                    const BasicPointLight& keyLight, const BasicPointLight& fillLight,
                    const BasicPointLight& ballLight) const;

    void DrawGhostPaddle(const PlayerPaddle& p, const Camera& camera);

    float GetOutlineMinDistance() const;
    float GetOutlineMaxDistance() const;
    float GetOutlineContrast() const;
    float GetOutlineOffset() const;
    
    float GetAlpha() const;

	// Factory methods
	static GameWorldAssets* CreateWorldAssets(GameWorld::WorldStyle world, GameAssets* assets);

protected:
	static const float COLOUR_CHANGE_TIME;
    
	Skybox* skybox;      // Skybox mesh and effect
	Mesh* background;    // Meshes that make up the background scenery
	Mesh* playerPaddle;  // Currently loaded player paddle mesh
	Mesh* styleBlock;    // Currently loaded solid block mesh

	AnimationMultiLerp<float> bgFadeAnim;			  // Fade animation (for the alpha) for when the background is being fadeded in/out
	AnimationMultiLerp<Colour> currBGMeshColourAnim;  // Colour animation progression of the background mesh

    // Cel-outline specific parameters
    float outlineMinDistance;
    float outlineMaxDistance;
    float outlineContrast;
    float outlineOffset;

    const Colour& GetColourChangeListAt(int i) const;
    void UpdateColourChangeList(const std::vector<Colour>& newList);

private:
    std::vector<Colour> colourChangeList;

    DISALLOW_COPY_AND_ASSIGN(GameWorldAssets);

};

inline void GameWorldAssets::FastDrawBackgroundModel() {
    this->background->FastDraw();
}

inline const Colour& GameWorldAssets::GetColourChangeListAt(int i) const {
    return this->colourChangeList[i];
}

inline float GameWorldAssets::GetOutlineMinDistance() const {
    return this->outlineMinDistance;
}

inline float GameWorldAssets::GetOutlineMaxDistance() const {
    return this->outlineMaxDistance;
}

inline float GameWorldAssets::GetOutlineContrast() const {
    return this->outlineContrast;
}

inline float GameWorldAssets::GetOutlineOffset() const {
    return this->outlineOffset;
}

inline float GameWorldAssets::GetAlpha() const {
    return this->bgFadeAnim.GetInterpolantValue();
}

#endif