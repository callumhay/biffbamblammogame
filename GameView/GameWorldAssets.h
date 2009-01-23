#ifndef __GAMEWORLDASSETS_H__
#define __GAMEWORLDASSETS_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Mesh.h"
#include "../BlammoEngine/Skybox.h"

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

	virtual void DrawSkybox(double dT, const Camera& camera) {
		this->skybox->Draw(dT, camera);
	}

	virtual void DrawBackgroundEffects(double dT, const Camera& camera) = 0;

	virtual void DrawBackgroundModel(double dT, const Camera& camera) {
		this->background->Draw(camera);
	}

	void DrawPaddle(const PlayerPaddle& p, const Camera& camera) const {
		Point2D paddleCenter = p.GetCenterPosition();	

		glPushMatrix();
		glTranslatef(paddleCenter[0], paddleCenter[1], 0);
		
		// Draw the paddle
		this->playerPaddle->Draw(camera);
		
		glPopMatrix();
	}

	// Factory methods
	static GameWorldAssets* CreateWorldAssets(GameWorld::WorldStyle world);

};
#endif