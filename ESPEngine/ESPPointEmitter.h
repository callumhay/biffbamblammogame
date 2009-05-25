#ifndef __ESPPOINTEMITTER_H__
#define __ESPPOINTEMITTER_H__

#include "ESPEmitter.h"

/** 
 * A point emitter - emits sprites from a defined point in space.
 */
class ESPPointEmitter : public ESPEmitter {

private:
	Point3D emitPt;					// The point from which all particles will be emitted from
	Vector3D emitDir;				// The direction (in emitter coords) that the emitter fires towards on average
	float emitAngleInRads;	// The angle of possible deviation from the emit dir

protected:
	virtual Vector3D CalculateRandomInitParticleDir() const;
	virtual Point3D  CalculateRandomInitParticlePos() const;

public:
	ESPPointEmitter();
	virtual ~ESPPointEmitter();

	void SetEmitPosition(const Point3D& pt);
	void SetEmitDirection(const Vector3D& dir);
	void SetEmitAngleInDegrees(int degs);
};
#endif