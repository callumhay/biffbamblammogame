#ifndef __DECOSKYBOX_H__
#define __DECOSKYBOX_H__

#include "../BlammoEngine/Skybox.h"

class DecoSkybox : public Skybox {

protected:
	static const float COLOUR_EPSILON;
	static const float COLOUR_CHANGE_SPEED;
	static const int NUM_COLOUR_CHANGES = 10;
	static const Colour COLOUR_CHANGE_LIST[NUM_COLOUR_CHANGES];
	static const std::string DECO_SKYBOX_TEXTURES[6];

	int currColourIndex;
	
	DecoSkybox(PolygonGroup* geom, TextureCube* tex);
	virtual void SetupCgFxParameters();

public:
	
	virtual ~DecoSkybox();
	static DecoSkybox* CreateDecoSkybox(const std::string& meshFilepath);

	virtual void Tick(double dT);

};

#endif