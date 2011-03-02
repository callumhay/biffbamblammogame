/**
 * ESPLineEmitter.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __ESPLINEEMITTER_H__
#define __ESPLINEEMITTER_H__

#include "ESPEmitter.h"

#include "../BlammoEngine/Collision.h"


class ESPLineEmitter : public ESPEmitter {
public:
	ESPLineEmitter();
	~ESPLineEmitter();

	void SetEmitLine(const Collision::LineSeg3D& line);
	void SetEmitDirection(const Vector3D& dir);
	void SetEmitAngle(float angleInDegrees);

protected:
	Vector3D CalculateRandomInitParticleDir() const;
	Point3D  CalculateRandomInitParticlePos() const;

private:
	Collision::LineSeg3D  emitLine; // The line along which particles are emitted
	Vector3D emitDir;								// The direction off the line which particles are emitted in
	float emitAngle;								// The angle variation from the emit dir that particle can be emitted within

};
#endif