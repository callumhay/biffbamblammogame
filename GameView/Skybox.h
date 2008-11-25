#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include "../Utils/Includes.h"

#include <vector>
#include <string>

class TextureCube;
class PolygonGroup;
class Camera;

/**
 * Encapsulates the polygons that make up the skybox and
 * the material applied to it.
 */
class Skybox {

protected:
	static const std::string SKYBOX_EFFECT_FILE;
	static const std::string TECHNIQUE_NAME;

	PolygonGroup* geometry;	// The geometry that constitutes this skybox
	TextureCube*  cubemap;	// The texture applied to this skybox

	// Skybox material variables --------------------------
	
	// The Cg effect and technique pointers
	CGeffect cgEffect;
	CGtechnique technique;

	// Skybox effect parameters
	// Transforms
	CGparameter wvpMatrixParam;
	CGparameter worldMatrixParam;
	// Location of the skybox-camera
	CGparameter skyboxCamParam;
	// Cubemap for the skybox
	CGparameter skyboxCubeSamplerParam;
	// Colour multiply for the skybox
	CGparameter colourMultParam;

	// ----------------------------------------------------

	Skybox(PolygonGroup* geom, TextureCube* tex);
	void LoadSkyboxCgFxParameters();
	virtual void SetupCgFxParameters(double dT);

public:
	virtual ~Skybox();

	void Draw(double dT, const Camera& camera);

	// Static creators
	static Skybox* CreateSkybox(const std::string& meshFilepath, 
														  const std::string cubeTexFilepaths[6]);
};

#endif
