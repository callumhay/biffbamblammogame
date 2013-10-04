/**
 * ESPParticle.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
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

	Matrix4x4 GetPersonalAlignmentTransform(const Camera& cam, const ESP::ESPAlignment alignment);

	/**
	 * Is this particle dead or not.
	 * Returns: true if the particle is dead, false otherwise.
	 */
	bool IsDead() const {
		return this->totalLifespan != INFINITE_PARTICLE_LIFETIME && this->currLifeElapsed >= this->totalLifespan;
	}

	virtual void Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan);
	virtual void Tick(const double dT);
	virtual void Draw(const Camera& camera, const ESP::ESPAlignment alignment);
	virtual void DrawAsPointSprite(const Camera& camera, const Vector3D& translation);
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

	const Vector3D& GetVelocity() const {
		return this->velocity;
	}
	void SetVelocity(const Vector3D& v) {
		this->velocity = v;
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
	Vector3D velocity;

	static float minMaxPtSize[2];
	

private:
	// Disallow copy and assign
	ESPParticle(const ESPParticle& p);
	ESPParticle& operator=(const ESPParticle& p);

};
#endif