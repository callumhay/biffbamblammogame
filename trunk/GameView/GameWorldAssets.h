#ifndef __GAMEWORLDASSETS_H__
#define __GAMEWORLDASSETS_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Mesh.h"
#include "../BlammoEngine/Skybox.h"
#include "../BlammoEngine/Light.h"

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
		delete this->background;
		this->background = NULL;
		delete this->playerPaddle;
		this->playerPaddle = NULL;
		delete this->styleBlock;
		this->styleBlock = NULL;
	}

	Mesh* GetWorldStyleBlock() const {
		return this->styleBlock;
	}

	virtual void Tick(double dT) {
		this->skybox->Tick(dT);	
	};

	virtual void DrawSkybox(const Camera& camera) {
		this->skybox->Draw(camera);
	}

	virtual void DrawBackgroundEffects(const Camera& camera) = 0;

	virtual void DrawBackgroundModel(const Camera& camera, const PointLight& keyLight, const PointLight& fillLight) {
		this->background->Draw(camera, keyLight, fillLight);
	}

	void DrawPaddle(const PlayerPaddle& p, const Camera& camera, const PointLight& keyLight, const PointLight& fillLight, const PointLight& ballLight) const {
		glPushMatrix();
		float paddleScaleFactor = p.GetPaddleScaleFactor();
		glScalef(paddleScaleFactor, paddleScaleFactor, paddleScaleFactor);
		this->playerPaddle->Draw(camera, keyLight, fillLight, ballLight);
		glPopMatrix();
	}

	// Factory methods
	static GameWorldAssets* CreateWorldAssets(GameWorld::WorldStyle world);

};
#endif