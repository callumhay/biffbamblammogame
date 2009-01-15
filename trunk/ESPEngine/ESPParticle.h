#ifndef __ESPPARTICLE_H__
#define __ESPPARTICLE_H__

#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Colour.h"

#include "ESPUtil.h"

class Camera;

class ESPParticle {

protected:
	// Vectors defining the up, right and normal directions of
	// any drawn particle
	static const Vector3D PARTICLE_UP_VEC;
	static const Vector3D PARTICLE_NORMAL_VEC;
	static const Vector3D PARTICLE_RIGHT_VEC;

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
	double alpha;

	// Kinematics variables for this particle
	Point3D  position;	// Position of the particle in world space
	Vector3D velocity;

	Matrix4x4 GetPersonalAlignmentTransform(const Camera& cam, const ESP::ESPAlignment alignment);

public: 
	static const int INFINITE_PARTICLE_LIFETIME = -1;

	ESPParticle();
	virtual ~ESPParticle();

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
	virtual void Kill() {
		this->currLifeElapsed = this->totalLifespan;
	}

	// Getter and setter functions (mostly used by Effector objects)
	Point3D GetPosition() const {
		return this->position;
	}
	void SetPosition(const Point3D& p) {
		this->position = p;
	}

	Vector2D GetScale() const {
		return this->size;
	}
	void SetScale(const Vector2D& scale) {
		this->size = scale;
	}
	void MultiplyInitSizeScale(float mult) {
		this->size = mult * this->initSize;
	}

	

	Vector3D GetVelocity() const {
		return this->velocity;
	}
	void SetVelocity(const Vector3D& v) {
		this->velocity = v;
	}

	void GetColour(Colour& rgb, double& alpha) {
		rgb = this->colour;
		alpha = this->alpha;
	}
	void SetColour(const Colour& rgb, double alpha) {
		this->colour = rgb;
		this->alpha = alpha;
	}

	double GetLifespanLength() const {
		return this->totalLifespan;
	}
	double GetCurrentLifeElapsed() const {
		return this->currLifeElapsed;
	}

};
#endif