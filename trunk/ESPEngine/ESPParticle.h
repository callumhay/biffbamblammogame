#ifndef __ESPPARTICLE_H__
#define __ESPPARTICLE_H__

#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Point.h"

class ESPParticle {

private:
	static int NUM_INSTANCES;
	static GLuint PARTICLE_QUAD_DISPLIST;

protected:
	// Total lifespan in seconds of this particle
	double totalLifespan;
	// Amount of time in seconds that have elapsed since this particle was created
	double currLifeElapsed;
	// Size of particle in units
	float size;

	// Kinematics variables for this particle
	Point3D  position;
	Vector3D velocity;

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
	void Draw();
};
#endif