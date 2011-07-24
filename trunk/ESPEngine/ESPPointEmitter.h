/**
 * ESPPointEmitter.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __ESPPOINTEMITTER_H__
#define __ESPPOINTEMITTER_H__

#include "ESPEmitter.h"

/** 
 * A point emitter - emits sprites from a defined point in space.
 */
class ESPPointEmitter : public ESPEmitter {
public:
	ESPPointEmitter();
	virtual ~ESPPointEmitter();

    void OverwriteEmittedPosition(const Point3D& pt);
	void SetEmitPosition(const Point3D& pt);
	void SetEmitDirection(const Vector3D& dir);
	void SetToggleEmitOnPlane(bool emitOnPlane, const Vector3D& planeNormal = Vector3D(0, 0, 1));
	void SetEmitAngleInDegrees(int degs);
	
	Vector3D GetEmitDirection() const {
		return this->emitDir;
	}

protected:
	virtual Vector3D CalculateRandomInitParticleDir() const;
	virtual Point3D  CalculateRandomInitParticlePos() const;

private:
	Point3D emitPt;					// The point from which all particles will be emitted from
	Vector3D emitDir;			// The direction (in emitter coords) that the emitter fires towards on average
	
	float emitAngleInRads;	// The angle of possible deviation from the emit dir
	
	bool emitOnPlane;				// Whether or not the emitter emits particles on a plane defined by planeNormal
	Vector3D planeNormal;		// The normal vector defining the plane to emit on in the case of emitOnPlane == true

	// Disallow copy and assign
	ESPPointEmitter(const ESPPointEmitter& e);
	ESPPointEmitter& operator=(const ESPPointEmitter& e);

};
#endif