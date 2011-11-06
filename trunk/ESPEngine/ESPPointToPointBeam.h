/**
 * ESPPointToPointBeam.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __ESPPOINTTOPOINTBEAM_H__
#define __ESPPOINTTOPOINTBEAM_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Colour.h"

#include "ESPUtil.h"
#include "ESPBeam.h"

class Texture2D;

class ESPPointToPointBeam {
public:
	ESPPointToPointBeam();
	~ESPPointToPointBeam();

	void Tick(double dT);
	void Draw(const Camera& camera);

	void SetStartAndEndPoints(const Point3D& startPt, const Point3D& endPt);
	void SetColour(const ColourRGBA& colour);
	void SetAlpha(float alpha);
	//void SetTexture(const Texture2D* texture);
	void SetNumMainESPBeamSegments(size_t numSegments);
	void SetMainBeamAmplitude(const ESPInterval& a);
	void SetMainBeamThickness(const ESPInterval& thickness);

	
	static const int INFINITE_NUMBER_OF_BEAM_SHOTS = -1;
	void SetNumBeamShots(int numBeamShots);
	void SetTimeBetweenBeamShots(const ESPInterval& timeBetweenShotsInSeconds);
	void SetBeamLifetime(const ESPInterval& beamLifetimeInSeconds);

protected:
	std::list<ESPBeam*> aliveBeams;

	// Appearance
	Point3D startPt;	// Start of the beam
	Point3D endPt;		// End of the beam

	ColourRGBA colour;							// General colour of the beam (multiplies the texture colour if there is a texture)
	//const Texture2D* texture;			// The texture applied to the beam
	size_t numMainESPBeamSegments;	// Number of segments that make up the main beam that goes from startPt to endPt
	ESPInterval mainBeamAmplitude;	// Amplitude of the main beam measured perpendicular from the striaght line between startPt and endPt
	ESPInterval mainBeamThickness;	// The thickness of the main beam, measured in world units

	// Timing
	int totalNumBeamShots;								// Number of times the beam will shoot (INFINITE_NUMBER_OF_BEAM_SHOTS means it will keep shooting forever) 
	ESPInterval timeBetweenShotsInSecs;		// Time in seconds between beam shots
	ESPInterval lifeTimeInSecs;						// Time in seconds that each beam shot will last
	
	int numBeamsShot;										// Total number of beams that have been shot so far
	double timeSinceLastBeamSpawn;			// Time since the last beam was fired
	double timeUntilNextBeamSpawn;			// Time until the next beam will be fired

	bool ThereAreStillMoreBeamsToFire() const;
	void SpawnBeam();


	// Disallow copy and assign
	ESPPointToPointBeam(const ESPPointToPointBeam& b);
	ESPPointToPointBeam& operator=(const ESPPointToPointBeam& b);
};

// Sets the start and end points of the beam
inline void ESPPointToPointBeam::SetStartAndEndPoints(const Point3D& startPt, const Point3D& endPt) {
	this->startPt = startPt;
	this->endPt   = endPt;
}

// Sets the general colour of the beam
inline void ESPPointToPointBeam::SetColour(const ColourRGBA& colour) {
	this->colour = colour;
}

inline void ESPPointToPointBeam::SetAlpha(float alpha) {
	this->colour[3] = alpha;
}

// Sets the texture (if any) applied to the beam
//inline void ESPPointToPointBeam::SetTexture(const Texture2D* texture) {
//	this->texture = texture;
//}

// Set the total number of beam segments for the main beam travelling between startPt and endPt
inline void ESPPointToPointBeam::SetNumMainESPBeamSegments(size_t numSegments) {
	assert(numSegments > 0);
	this->numMainESPBeamSegments = numSegments;
}

// Set the amplitude in units from the central line of the beam to where the main beam
// can move to (to create lightning-like effects)
inline void ESPPointToPointBeam::SetMainBeamAmplitude(const ESPInterval& a) {
	this->mainBeamAmplitude = a;
}

// Set the thickness in world units of the main beam
inline void ESPPointToPointBeam::SetMainBeamThickness(const ESPInterval& thickness) {
	assert(thickness.minValue > 0);
	this->mainBeamThickness = thickness;
}

// Sets the total number of shots the beam will take
inline void ESPPointToPointBeam::SetNumBeamShots(int numBeamShots) {
	assert(numBeamShots > 0 || numBeamShots == INFINITE_NUMBER_OF_BEAM_SHOTS);
	this->totalNumBeamShots = numBeamShots;
}

// Sets the time in seconds between each shot of the beam
inline void ESPPointToPointBeam::SetTimeBetweenBeamShots(const ESPInterval& timeBetweenShotsInSeconds) {
	assert(timeBetweenShotsInSeconds.minValue >= 0 && timeBetweenShotsInSeconds.maxValue >= 0);
	this->timeBetweenShotsInSecs = timeBetweenShotsInSeconds;
}

// Sets the time in seconds of the lifetime of each beam
inline void ESPPointToPointBeam::SetBeamLifetime(const ESPInterval& beamLifetimeInSeconds) {
	assert((beamLifetimeInSeconds.minValue > 0 && beamLifetimeInSeconds.maxValue > 0) ||
		(beamLifetimeInSeconds.minValue == ESPBeam::INFINITE_BEAM_LIFETIME && beamLifetimeInSeconds.maxValue == ESPBeam::INFINITE_BEAM_LIFETIME));
	this->lifeTimeInSecs = beamLifetimeInSeconds;
}

// Query whether there are still more beams to fire from this beam emitter
inline bool ESPPointToPointBeam::ThereAreStillMoreBeamsToFire() const {
	return (this->numBeamsShot < this->totalNumBeamShots) || (this->totalNumBeamShots == INFINITE_NUMBER_OF_BEAM_SHOTS);
}

#endif // __ESPPOINTTOPOINTBEAM_H__