#ifndef __CGFXINKBLOCK_H__
#define __CGFXINKBLOCK_H__

#include "../BlammoEngine/CgFxEffect.h"

class CgFxInkBlock : public CgFxMaterialEffect {
private:
	static const std::string INKBLOCK_TECHNIQUE_NAME;

protected:
	// CG parameters for ink block effect
	CGparameter timerParam;
	CGparameter displacementParam;
	CGparameter sharpnessParam;
	CGparameter speedParam;
	CGparameter turbDensityParam;
	
	// Values for the cg parameters
	float timer, displacement, sharpness, speed, turbulanceDensity;

	virtual void SetupBeforePasses(const Camera& camera);

public:
	CgFxInkBlock(MaterialProperties* properties);
	virtual ~CgFxInkBlock();

};
#endif