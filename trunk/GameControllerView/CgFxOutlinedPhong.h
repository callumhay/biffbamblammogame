#ifndef __CGFXOUTLINEDPHONG_H__
#define __CGFXOUTLINEDPHONG_H__

#include "CgFxEffect.h"
#include <string>

class Camera;

/**
 * Represents CgFx CelShader material.
 */
class CgFxOutlinedPhong : public CgFxEffect {

private:

	// Constants used for loading the CelShading effect
	static const std::string EFFECT_FILE;
	static const std::string BASIC_TECHNIQUE_NAME;
	static const std::string TEXTURED_TECHNIQUE_NAME;

public:

	CgFxOutlinedPhong(MaterialProperties* properties);
	virtual ~CgFxOutlinedPhong();

};

#endif