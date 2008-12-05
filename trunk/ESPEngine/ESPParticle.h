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

private:
	// Vectors defining the up, right and normal directions of
	// any drawn particle
	static const Vector3D PARTICLE_UP_VEC;
	static const Vector3D PARTICLE_NORMAL_VEC;
	static const Vector3D PARTICLE_RIGHT_VEC;

protected:
	// Total lifespan in seconds of this particle
	double totalLifespan;
	// Amount of time in seconds that have elapsed since this particle was created
	double currLifeElapsed;
	// Size of particle in units
	float size;
	// The colour of the particle
	Colour colour;
	// The alpha of the particle (0 is completely transparent, 1 is completely opaque)
	float alpha;

	// Kinematics variables for this particle
	Point3D  position;	// Position of the particle in world space
	Vector3D velocity;

	Matrix4x4 GetPersonalAlignmentTransform(const Camera& cam, const ESP::ESPAlignment alignment);

public: 
	ESPParticle();
	virtual ~ESPParticle();

	/**
	 * Is this particle dead or not.
	 * Returns: true if the particle is dead, false otherwise.
	 */
	bool IsDead() const {
		return this->currLifeElapsed >= this->totalLifespan;
	}

	void Revive(const Point3D& pos, const Vector3D& vel, const float size, const float totalLifespan);
	
	void Tick(const double dT);
	void Draw(const Camera& camera, const ESP::ESPAlignment alignment);

	static Matrix4x4 GetAlignmentTransform(const Camera& cam, const ESP::ESPAlignment alignment);

	// Getter and setter functions (mostly used by Effector objects)
	Point3D GetPosition() const {
		return this->position;
	}
	void SetPosition(const Point3D& p) {
		this->position = p;
	}

	void GetColour(Colour& rgb, float& alpha) {
		rgb = this->colour;
		alpha = this->alpha;
	}
	void SetColour(const Colour& rgb, float alpha) {
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