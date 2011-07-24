/**
 * CgFxPhong.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009-2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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