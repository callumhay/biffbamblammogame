/**
 * CgFxInkBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011-2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CGFXINKBLOCK_H__
#define __CGFXINKBLOCK_H__

#include "../BlammoEngine/CgFxEffect.h"

class CgFxInkBlock : public CgFxMaterialEffect {
private:
	static const char* INKBLOCK_TECHNIQUE_NAME;

protected:
	// CG parameters for ink block effect
	CGparameter timerParam;
	CGparameter displacementParam;
	CGparameter sharpnessParam;
	CGparameter speedParam;
	CGparameter turbDensityParam;
	
	// Values for the cg parameters
	float timer, displacement, sharpness, speed, turbulanceDensity;

	void SetupBeforePasses(const Camera& camera);

public:
	CgFxInkBlock(MaterialProperties* properties);
	~CgFxInkBlock();

};
#endif