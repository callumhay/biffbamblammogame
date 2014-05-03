/**
 * ESPParticle.h
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

#ifndef __ESPPARTICLE_H__
#define __ESPPARTICLE_H__

#include "../BlammoEngine/IPositionObject.h"
#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Colour.h"

#include "ESPUtil.h"

class Camera;

class ESPParticle : public IPositionObject {
public: 
	static const int INFINITE_PARTICLE_LIFETIME = -1;
	static const int INFINITE_PARTICLE_LIVES		= -1;

	// Vectors defining the up, right and normal directions of
	// any drawn particle
	static const Vector3D PARTICLE_UP_VEC;
	static const Vector3D PARTICLE_NORMAL_VEC;
	static const Vector3D PARTICLE_RIGHT_VEC;

	ESPParticle();
	virtual ~ESPParticle();

	void GetPersonalAlignmentTransform(const Matrix4x4& modelMat, const Matrix4x4& modelInvTMat, 
        const Camera& cam, const ESP::ESPAlignment alignment, const Point3D& localPos, Matrix4x4& result);

	/**
	 * Is this particle dead or not.
	 * Returns: true if the particle is dead, false otherwise.
	 */
	bool IsDead() const {
		return this->totalLifespan != INFINITE_PARTICLE_LIFETIME && this->currLifeElapsed >= this->totalLifespan;
	}

	virtual void Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan);
	virtual void Tick(const double dT);
	virtual void Draw(const Matrix4x4& modelMat, const Matrix4x4& modelInvTMat, const Camera& camera, const ESP::ESPAlignment& alignment);
	virtual void Kill() {
		this->currLifeElapsed = this->totalLifespan;
	}

	// Getter and setter functions (mostly used by Effector objects)
	const Point3D& GetPosition() const {
		return this->position;
	}
	void SetPosition(const Point3D& p) {
		this->position = p;
	}
    void SetPosition(float x, float y, float z) {
        this->position[0] = x;
        this->position[1] = y;
        this->position[2] = z;
    }

    // Inherited from IPositionObject
    Point3D GetPosition3D() const {
        return this->position;
    }

	const Vector2D& GetScale() const {
		return this->size;
	}
	void SetScale(const Vector2D& scale) {
		this->size = scale;
	}
    void SetScale(float x, float y) {
        this->size[0] = x;
        this->size[1] = y;
    }
	void MultiplyInitSizeScale(const Vector2D& mult) {
		this->size[0] = mult[0] * this->initSize[0];
		this->size[1] = mult[1] * this->initSize[1];
	}
	void SetNewInitSizeScale(const Vector2D& size) {
		this->initSize = size;
	}

	Vector3D GetVelocity() const {
		return this->speed * this->velocityDir;
	}
	void SetVelocity(const Vector3D& v) {
        if (v.IsZero()) { 
            velocityDir = Vector3D(0,0,0); 
            this->speed = 0.0f; 
        }
        else {
            this->speed = v.length();
            assert(this->speed > 0);
		    this->velocityDir = v/this->speed;
        }
	}

	void GetColour(Colour& rgb, float& alpha) {
		rgb = this->colour;
		alpha = this->alpha;
	}

	Colour4D GetColour() const {
		Colour4D c;
		c.rgba[0] = this->colour[0];
		c.rgba[1] = this->colour[1];
		c.rgba[2] = this->colour[2];
		c.rgba[3] = alpha;
		return c;
	}

	void SetColour(const ColourRGBA& rgba) {
		this->colour = rgba.GetColour();
		this->alpha = rgba.A();
	}

	void SetColour(float r, float g, float b, float alpha) {
		this->colour = Colour(r, g, b);
		this->alpha = alpha;
	}

	void SetColour(const Colour& rgb, float alpha) {
		this->colour = rgb;
		this->alpha = alpha;
	}

	void SetAlpha(float alpha) {
		this->alpha = alpha;
	}
    float GetAlpha() const {
        return this->alpha;
    }

    void ResetLifespanLength(double secs) {
        this->totalLifespan   = secs;
        this->currLifeElapsed = 0.0;
    }
	double GetLifespanLength() const {
		return this->totalLifespan;
	}
	double GetCurrentLifeElapsed() const {
		return this->currLifeElapsed;
	}

	float GetRotation() const {
		return this->rotation;
	}
	void SetRotation(float r) {
		this->rotation = r;
	}

protected:
	// Total lifespan in seconds of this particle
	double totalLifespan;
	// Amount of time in seconds that have elapsed since this particle was created
	double currLifeElapsed;
	// x,y,z Size of particle in units
	Vector2D initSize, size;
	// Rotation of particle in degrees
	float rotation;
	// The colour of the particle
	Colour colour;
	// The alpha of the particle (0 is completely transparent, 1 is completely opaque)
	float alpha;

	// Kinematics variables for this particle
	Point3D  position;	// Position of the particle in world space
	
    Vector3D velocityDir;   // Direction (normalized) of the velocity
    float speed;            // Always positive

	static float minMaxPtSize[2];
	

private:
	// Disallow copy and assign
	ESPParticle(const ESPParticle& p);
	ESPParticle& operator=(const ESPParticle& p);

};
#endif