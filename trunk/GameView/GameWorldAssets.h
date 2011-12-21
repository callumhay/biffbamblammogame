/**
 * GameWorldAssets.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMEWORLDASSETS_H__
#define __GAMEWORLDASSETS_H__

#include "GameViewConstants.h"

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Mesh.h"
#include "../BlammoEngine/Skybox.h"
#include "../BlammoEngine/Light.h"
#include "../ResourceManager.h"

#include "../GameModel/GameWorld.h"
#include "../GameModel/PlayerPaddle.h"

// Abstract class for game world assets - this includes all
// the specific meshes, effects, etc. associated with a particular
// game world.
class GameWorldAssets {
public:
	GameWorldAssets(Skybox* skybox, Mesh* bg, Mesh* paddle, Mesh* styleBlock);
	virtual ~GameWorldAssets();

	Mesh* GetWorldStyleBlock() const {
		return this->styleBlock;
	}

	Skybox* GetSkybox() const {
		return this->skybox;
	}

	virtual void Tick(double dT) {
		this->skybox->Tick(dT);
        this->bgFadeAnim.Tick(dT);
	    this->currBGMeshColourAnim.Tick(dT);
	};

	virtual void DrawSkybox(const Camera& camera) {
		this->skybox->Draw(camera);
	}

	virtual GameWorld::WorldStyle GetStyle() const = 0;

	virtual void DrawBackgroundEffects(const Camera& camera) = 0;
	virtual void DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight, const BasicPointLight& bgFillLight) = 0;
	virtual void FadeBackground(bool fadeout, float fadeTime);
	virtual void ResetToInitialState();

	void DrawPaddle(const PlayerPaddle& p, const Camera& camera, CgFxEffectBase* replacementMat, 
                    const BasicPointLight& keyLight, const BasicPointLight& fillLight,
                    const BasicPointLight& ballLight) const {

		const ColourRGBA& paddleColour = p.GetColour();
		if (paddleColour.A() < EPSILON) {
			return;
		}

		glPushMatrix();
		
		float paddleScaleFactor  = p.GetPaddleScaleFactor();
		float paddleZRotationAmt = p.GetZRotation();

		glRotatef(paddleZRotationAmt, 0, 0, 1);
		glScalef(paddleScaleFactor, paddleScaleFactor, paddleScaleFactor);
		glColor4f(paddleColour.R(), paddleColour.G(), paddleColour.B(), paddleColour.A());
		this->playerPaddle->Draw(camera, replacementMat, keyLight, fillLight, ballLight);
		glPopMatrix();
	}

	// Factory methods
	static GameWorldAssets* CreateWorldAssets(GameWorld::WorldStyle world);

protected:
	static const float COLOUR_CHANGE_TIME;
	static const int NUM_COLOUR_CHANGES = 10;
	static const Colour COLOUR_CHANGE_LIST[NUM_COLOUR_CHANGES];

	Skybox* skybox;				// Skybox mesh and effect
	Mesh* background;			// Meshes that make up the background scenery
	Mesh* playerPaddle;		// Currently loaded player paddle mesh
	Mesh* styleBlock;

	AnimationMultiLerp<float> bgFadeAnim;			  // Fade animation (for the alpha) for when the background is being fadeded in/out
	AnimationMultiLerp<Colour> currBGMeshColourAnim;  // Colour animation progression of the background mesh


private:
    DISALLOW_COPY_AND_ASSIGN(GameWorldAssets);

};
#endif