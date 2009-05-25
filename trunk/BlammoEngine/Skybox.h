#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include "BasicIncludes.h"
#include "Colour.h"
#include "Camera.h"

class Texture;
class PolygonGroup;

/**
 * Encapsulates the polygons that make up the skybox and
 * the material applied to it.
 */
class Skybox {

protected:
	Texture*  skyboxTex;	// The texture applied to this skybox
	Skybox(Texture* tex);

	static void DrawSkyboxGeometry(int texTiling = 1, float width = 0.5f);

public:
	virtual ~Skybox();

	virtual void Tick(double dT){};
	virtual void Draw(const Camera& camera);

	// Static creators
	//static Skybox* CreateSkybox(const std::string cubeTexFilepaths[6]);
	static Skybox* CreateSkybox(const std::string &texFilepath);
};

#endif
