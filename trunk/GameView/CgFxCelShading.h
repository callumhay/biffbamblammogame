/**
 * CgFxCelShading.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009-2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CGFXCELSHADING_H__
#define __CGFXCELSHADING_H__

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/Texture1D.h"

/**
 * Represents CgFx CelShader material.
 */
class CgFxCelShading : public CgFxMaterialEffect {

private:
	// Constants used for loading the CelShading effect
	static const char* BASIC_FG_TECHNIQUE_NAME;
	static const char* TEXTURED_FG_TECHNIQUE_NAME;
	static const char* BASIC_BG_TECHNIQUE_NAME;
	static const char* TEXTURED_BG_TECHNIQUE_NAME;

	static Texture* CelDiffuseTexture;

	// Cel shading texture: parameter and texture objects
	CGparameter celSamplerParam;
	
	CGparameter outlineWidthParam;
	CGparameter outlineColourParam;

protected:
	virtual void SetupBeforePasses(const Camera& camera);

public:
	CgFxCelShading(MaterialProperties* properties);
	virtual ~CgFxCelShading();

};

#endif