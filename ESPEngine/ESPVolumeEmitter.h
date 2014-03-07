/**
 * ESPVolumeEmitter.h
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

#ifndef __ESPVOLUMEEMITTER_H__
#define __ESPVOLUMEEMITTER_H__

#include "ESPEmitter.h"

/**
 * An area emitter - emits sprites from a defined area.
 */
class ESPVolumeEmitter : public ESPEmitter {
public:
	ESPVolumeEmitter();
	~ESPVolumeEmitter();

	// Rectangle-prism volume setters
	//void SetEmitArea(const Point3D& center, const Vector3D& rectHalfVert, const Vector3D& rectHalfHoriz);
	void SetEmitVolume(const Point3D& min, const Point3D& max);
	void SetEmitDirection(const Vector3D& dir);
    void SetEmitDirection(const Vector2D& dir);

protected:
	Vector3D CalculateRandomInitParticleDir() const;
	Point3D  CalculateRandomInitParticlePos() const;

private:
	// Super class for area shapes to emit from
	struct EmitVolume {
		virtual ~EmitVolume() {}
		virtual Point3D GetRandomPointInVolume() const = 0;
	};
	//struct EmitOval : public EmitVolume {
	//	Point3D center;
	//	float radius;
	//	virtual Point3D GetRandomPointInVolume() const;
	//};
	struct EmitAABB : public EmitVolume {
		Point3D min, max;
		virtual Point3D GetRandomPointInVolume() const;
	};

	Vector3D emitDir;													// The direction (in emitter coords) that the emitter fires towards
	ESPVolumeEmitter::EmitVolume* emitVolume;	// The area to emit particles from

	ESPVolumeEmitter(const ESPVolumeEmitter& e);
	ESPVolumeEmitter& operator=(const ESPVolumeEmitter& e);

};

/**
 * Set the emit direction for this area emitter.
 */
inline void ESPVolumeEmitter::SetEmitDirection(const Vector3D& dir) {
	this->emitDir = dir;
}

inline void ESPVolumeEmitter::SetEmitDirection(const Vector2D& dir) {
    this->emitDir[0] = dir[0];
    this->emitDir[1] = dir[1];
    this->emitDir[2] = 0.0f;
}
#endif
