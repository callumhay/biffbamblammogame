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
	static const std::string BASIC_FG_TECHNIQUE_NAME;
	static const std::string BASIC_FG_NO_OUTLINE_TECHNIQUE_NAME;
	static const std::string TEXTURED_FG_TECHNIQUE_NAME;
	static const std::string TEXTURED_FG_NO_OUTLINE_TECHNIQUE_NAME;
	static const std::string BASIC_BG_TECHNIQUE_NAME;
	static const std::string TEXTURED_BG_TECHNIQUE_NAME;

public:
	CgFxPhong(MaterialProperties* properties);
	~CgFxPhong();

};

#endif