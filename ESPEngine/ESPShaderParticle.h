#ifndef __ESPSHADERPARTICLE_H__
#define __ESPSHADERPARTICLE_H__

#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/TextLabel.h"

#include "../GameModel/Onomatoplex.h"

#include "ESPUtil.h"
#include "ESPParticle.h"

class TextureFontSet;
class CgFxEffectBase;

class ESPShaderParticle : public ESPParticle{
protected:
	// The shader effect for this particle
	CgFxEffectBase* shaderEffect;

public: 
	ESPShaderParticle(CgFxEffectBase* effect);
	virtual ~ESPShaderParticle();

	virtual void Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan);
	virtual void Tick(const double dT);
	virtual void Draw(const Camera& camera, const ESP::ESPAlignment alignment);

};
#endif