#ifndef __ESPPOINTEMITTER_H__
#define __ESPPOINTEMITTER_H__

#include "ESPEmitter.h"

class ESPPointEmitter : public ESPEmitter {

protected:
	Point3D emitPt;					// The point from which all particles will be emitted from
	Vector3D emitDir;				// The direction (in emitter coords) that the emitter fires towards on average
	float emitAngleInRads;	// The angle of possible deviation from the emit dir

	ESPInterval radiusDeviationFromPt;	// Inclusive interval for how far from the emitPt this particle may spawn at

	Vector3D CalculateRandomInitParticleDir() const;

public:
	ESPPointEmitter();
	virtual ~ESPPointEmitter();

	virtual void Tick(const double dT);
	virtual void Draw(const Camera& camera);

	void SetEmitDirection(const Vector3D& dir);
	void SetEmitAngleInDegrees(int degs);
	void SetRadiusDeviationFromCenter(const ESPInterval& distFromCenter);

};
#endif