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

protected:
	Skybox* skybox;				// Skybox mesh and effect
	Mesh* background;			// Meshes that make up the background scenery
	Mesh* playerPaddle;		// Currently loaded player paddle mesh
	Mesh* styleBlock;
	
	PointLight bgFillLight, bgKeyLight;
	std::list<AnimationLerp<Colour>> colourAnims;

		
public:
	GameWorldAssets(Skybox* skybox, Mesh* bg, Mesh* paddle, Mesh* styleBlock) : 
			skybox(skybox), background(bg), playerPaddle(paddle), styleBlock(styleBlock) {
		assert(skybox != NULL);
		assert(bg != NULL);
		assert(paddle != NULL);

		// Initialize background key and fill lights
		this->bgKeyLight	= PointLight(Point3D(-25.0f, 20.0f, 55.0f), GameViewConstants::GetInstance()->DEFAULT_BG_KEY_LIGHT_COLOUR,   0.0f);
		this->bgFillLight = PointLight(Point3D(30.0f, 11.0f, -15.0f), GameViewConstants::GetInstance()->DEFAULT_BG_FILL_LIGHT_COLOUR,  0.025f);
	}
	virtual ~GameWorldAssets() {
		delete this->skybox;
		this->skybox = NULL;

		// Tell the resource manager to release any of the loaded meshes
		ResourceManager::GetInstance()->ReleaseMeshResource(this->background);
		ResourceManager::GetInstance()->ReleaseMeshResource(this->playerPaddle);
		ResourceManager::GetInstance()->ReleaseMeshResource(this->styleBlock);
	}

	void ToggleBackgroundLights(bool turnOn) {
		this->colourAnims.clear();

		// Add animations to dim and turn off the lights or turn the lights back on
		AnimationLerp<Colour> keyLightColAnim(this->bgKeyLight.GetDiffuseColourPtr());
		keyLightColAnim.SetLerp(1.0f, turnOn ? GameViewConstants::GetInstance()->DEFAULT_BG_KEY_LIGHT_COLOUR : GameViewConstants::GetInstance()->BLACKOUT_LIGHT_COLOUR);
		this->colourAnims.push_back(keyLightColAnim);
		AnimationLerp<Colour> fillLightColAnim(this->bgFillLight.GetDiffuseColourPtr());
		fillLightColAnim.SetLerp(1.0f, turnOn ? GameViewConstants::GetInstance()->DEFAULT_BG_FILL_LIGHT_COLOUR : GameViewConstants::GetInstance()->BLACKOUT_LIGHT_COLOUR);
		this->colourAnims.push_back(fillLightColAnim);
	}

	Mesh* GetWorldStyleBlock() const {
		return this->styleBlock;
	}

	Skybox* GetSkybox() const {
		return this->skybox;
	}

	virtual void Tick(double dT) {
		this->skybox->Tick(dT);

		// Tick any animations
		std::list<std::list<AnimationLerp<Colour>>::iterator> toRemove;
		for (std::list<AnimationLerp<Colour>>::iterator animIter = this->colourAnims.begin(); 
			animIter != this->colourAnims.end();) {
				bool isFinished = animIter->Tick(dT);
				if (isFinished) {
					animIter = this->colourAnims.erase(animIter);
				}
				else {
					animIter++;
				}
		}
	};

	virtual void DrawSkybox(const Camera& camera) {
		this->skybox->Draw(camera);
	}

	virtual void DrawBackgroundEffects(const Camera& camera) = 0;

	virtual void DrawBackgroundModel(const Camera& camera) = 0;

	void DrawPaddle(const PlayerPaddle& p, const Camera& camera, const PointLight& keyLight, const PointLight& fillLight, const PointLight& ballLight) const {
		ColourRGBA paddleColour = p.GetPaddleColour();
		if (paddleColour.A() < EPSILON) {
			return;
		}

		glPushMatrix();
		
		float paddleScaleFactor = p.GetPaddleScaleFactor();

		glScalef(paddleScaleFactor, paddleScaleFactor, paddleScaleFactor);
		glColor4f(paddleColour.R(), paddleColour.G(), paddleColour.B(), paddleColour.A());
		this->playerPaddle->Draw(camera, keyLight, fillLight, ballLight);

		glPopMatrix();
	}

	// Factory methods
	static GameWorldAssets* CreateWorldAssets(GameWorld::WorldStyle world);

};
#endif