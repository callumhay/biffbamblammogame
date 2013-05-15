/**
 * CgFxPhong.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __CGFXPHONG_H__
#define __CGFXPHONG_H__

#include "../BlammoEngine/CgFxEffect.h"

class Camera;

/**
 * Represents CgFx CelShader material.
 */
class CgFxPhong : public CgFxMaterialEffect {

private:
	// Constants used for loading the CelShading effect
	static const char* BASIC_FG_TECHNIQUE_NAME;
	static const char* BASIC_FG_NO_OUTLINE_TECHNIQUE_NAME;
	static const char* TEXTURED_FG_TECHNIQUE_NAME;
	static const char* TEXTURED_FG_NO_OUTLINE_TECHNIQUE_NAME;
	static const char* BASIC_BG_TECHNIQUE_NAME;
	static const char* TEXTURED_BG_TECHNIQUE_NAME;

public:
	CgFxPhong(MaterialProperties* properties);
	~CgFxPhong();

};

#endif