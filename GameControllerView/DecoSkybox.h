#ifndef __DECOSKYBOX_H__
#define __DECOSKYBOX_H__

#include "Skybox.h"

#include "../Utils/Colour.h"

#include <string>

class DecoSkybox : public Skybox {

protected:
	static const float COLOUR_EPSILON;
	static const float COLOUR_CHANGE_SPEED;
	static const int NUM_COLOUR_CHANGES = 10;
	static const Colour COLOUR_CHANGE_LIST[NUM_COLOUR_CHANGES];
	static const std::string DECO_SKYBOX_TEXTURES[6];

	int currColourIndex;
	Colour currColour;

	DecoSkybox(PolygonGroup* geom, TextureCube* tex);
	virtual void SetupCgFxParameters(double dT);

public:
	
	virtual ~DecoSkybox();
	static DecoSkybox* CreateDecoSkybox(const std::string& meshFilepath);

};

#endif