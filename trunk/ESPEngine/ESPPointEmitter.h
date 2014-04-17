/**
 * ESPPointEmitter.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
    void SetEmitPosition(const Vector3D& pt);
	void SetEmitDirection(const Vector3D& dir);
    void SetEmitDirection(const Vector2D& dir);
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
	DISALLOW_COPY_AND_ASSIGN(ESPPointEmitter);
};

/**
 * Set the center emit point for this point emitter.
 */
inline void ESPPointEmitter::SetEmitPosition(const Point3D& pt) {
	this->emitPt = pt;
}

inline void ESPPointEmitter::SetEmitPosition(const Vector3D& pt) {
    this->emitPt[0] = pt[0];
    this->emitPt[1] = pt[1];
    this->emitPt[2] = pt[2];
}

/**
 * Set the emit direction for this point emitter.
 */
inline void ESPPointEmitter::SetEmitDirection(const Vector3D& dir) {
	this->emitDir = dir;

	assert(!this->emitOnPlane || Vector3D::cross(this->planeNormal, this->emitDir) != Vector3D(0,0,0));
    assert(!this->emitDir.IsZero());
}

inline void ESPPointEmitter::SetEmitDirection(const Vector2D& dir) {
    this->emitDir[0] = dir[0];
    this->emitDir[1] = dir[1];
    this->emitDir[2] = 0;

    assert(!this->emitOnPlane || Vector3D::cross(this->planeNormal, this->emitDir) != Vector3D(0,0,0));
    assert(!this->emitDir.IsZero());
}
#endif