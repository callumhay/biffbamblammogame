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
		
public:
	GameWorldAssets(Skybox* skybox, Mesh* bg, Mesh* paddle, Mesh* styleBlock) : 
			skybox(skybox), background(bg), playerPaddle(paddle), styleBlock(styleBlock) {
		assert(skybox != NULL);
		assert(bg != NULL);
		assert(paddle != NULL);
	}

	virtual ~GameWorldAssets() {
		delete this->skybox;
		this->skybox = NULL;

		// Tell the resource manager to release any of the loaded meshes
		ResourceManager::GetInstance()->ReleaseMeshResource(this->background);
		ResourceManager::GetInstance()->ReleaseMeshResource(this->playerPaddle);
		ResourceManager::GetInstance()->ReleaseMeshResource(this->styleBlock);
	}

	Mesh* GetWorldStyleBlock() const {
		return this->styleBlock;
	}

	Skybox* GetSkybox() const {
		return this->skybox;
	}

	virtual void Tick(double dT) {
		this->skybox->Tick(dT);
	};

	virtual void DrawSkybox(const Camera& camera) {
		this->skybox->Draw(camera);
	}

	virtual void DrawBackgroundEffects(const Camera& camera) = 0;
	virtual void DrawBackgroundModel(const Camera& camera, const PointLight& bgKeyLight, const PointLight& bgFillLight) = 0;
	virtual void FadeBackground(bool fadeout, float fadeTime) = 0;

	void DrawPaddle(const PlayerPaddle& p, const Camera& camera, const PointLight& keyLight, const PointLight& fillLight, const PointLight& ballLight) const {
		ColourRGBA paddleColour = p.GetColour();
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