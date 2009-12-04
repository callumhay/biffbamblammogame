
#ifndef __CGFXPRISM_H__
#define __CGFXPRISM_H__

#include "../BlammoEngine/CgFxEffect.h"

/**
 * Material shader class for prism blocks and other prism-like
 * objects in the game.
 */
class CgFxPrism : public CgFxMaterialEffect {
public:
	CgFxPrism(MaterialProperties* properties);
	~CgFxPrism();

protected:
	//void SetupBeforePasses(const Camera& camera);

private:
	static const std::string DEFAULT_PRISM_TECHNIQUE_NAME;

};
#endif // __CGFXPRISM_H__