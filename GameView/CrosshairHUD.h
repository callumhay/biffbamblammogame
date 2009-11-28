#ifndef __CROSSHAIRHUD_H__
#define __CROSSHAIRHUD_H__

#include "../BlammoEngine/BasicIncludes.h"

class Texture;

class CrosshairHUD {
public:
	CrosshairHUD();
	~CrosshairHUD();

	void Draw(int screenWidth, int screenHeight, float alpha) const;

private:
	static const int CROSSHAIR_WIDTH				= 128;
	static const int CROSSHAIR_HEIGHT				= CROSSHAIR_WIDTH;
	static const int CROSSHAIR_HALF_WIDTH		= CROSSHAIR_WIDTH / 2;
	static const int CROSSHAIR_HALF_HEIGHT	= CROSSHAIR_HEIGHT / 2;

	Texture* crosshairTex;	// Crosshair texture resource
	
};

#endif