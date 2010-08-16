
#ifndef __CGFXPRISM_H__
#define __CGFXPRISM_H__

#include "../BlammoEngine/CgFxEffect.h"

class Texture2D;

/**
 * Material shader class for prism blocks and other prism-like
 * objects in the game.
 */
class CgFxPrism : public CgFxMaterialEffect {
public:
	CgFxPrism(MaterialProperties* properties);
	virtual ~CgFxPrism();

	/**
	 * Set the FBO texture (the texture where the scene thus far
	 * has been rendered to).
	 */
	void SetSceneTexture(const Texture2D* tex) {
		this->sceneTex = tex;
	}

protected:
	void SetupBeforePasses(const Camera& camera);

private:
	static const std::string DEFAULT_PRISM_TECHNIQUE_NAME;

	CGparameter indexOfRefractionParam;
	CGparameter warpAmountParam;
	CGparameter sceneWidthParam;
	CGparameter sceneHeightParam;
	CGparameter sceneSamplerParam;

	float indexOfRefraction, warpAmount;
	const Texture2D* sceneTex;

};
#endif // __CGFXPRISM_H__