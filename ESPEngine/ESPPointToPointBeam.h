/**
 * ESPPointToPointBeam.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#ifndef __ESPPOINTTOPOINTBEAM_H__
#define __ESPPOINTTOPOINTBEAM_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Colour.h"

#include "ESPAbstractEmitter.h"
#include "ESPUtil.h"
#include "ESPBeam.h"

class ESPEffector;
class Texture2D;

class ESPPointToPointBeam : public ESPAbstractEmitter {
public:
	ESPPointToPointBeam();
	~ESPPointToPointBeam();

	void Tick(double dT);
	void Draw(const Camera& camera);
    bool IsDead() const { return (this->aliveBeams.empty() && !this->ThereAreStillMoreBeamsToFire()); }

    void SetStartPoint(const Point3D& startPt) { this->startPt = startPt; }

	void SetStartAndEndPoints(const Point3D& startPt, const Point3D& endPt);
    void SetStartAndEndPoints(const Point2D& startPt, const Point2D& endPt);

	void SetColour(const ColourRGBA& colour);
	void SetAlpha(float alpha);
	//void SetTexture(const Texture2D* texture);
	void SetNumMainESPBeamSegments(size_t numSegments);
	void SetMainBeamAmplitude(const ESPInterval& a);
	void SetMainBeamThickness(const ESPInterval& thickness);

    void SetEnableDepth(bool enable) { this->depthEnabled = enable; }
	
	static const int INFINITE_NUMBER_OF_BEAM_SHOTS = -1;
	void SetNumBeamShots(int numBeamShots);
	void SetTimeBetweenBeamShots(const ESPInterval& timeBetweenShotsInSeconds);
	void SetBeamLifetime(const ESPInterval& beamLifetimeInSeconds);

    void SetRemainingBeamLifetimeMax(double lifeTimeInSecs);

    void AddCopiedEffector(const ESPEffector& effector);
    void ClearEffectors();

protected:
	std::list<ESPBeam*> aliveBeams;
    std::list<ESPEffector*> effectors;

	// Appearance
	Point3D startPt;	// Start of the beam
	Point3D endPt;		// End of the beam

    bool depthEnabled;
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
    void UpdateBeamGeometry();
    void UpdateBeamColours();

    void GetBeamInfo(Vector3D& beamVec, Vector3D& orthToBeamVec, float& avgSegLength, float& fractionAvgSegLength) const;

	// Disallow copy and assign
	ESPPointToPointBeam(const ESPPointToPointBeam& b);
	ESPPointToPointBeam& operator=(const ESPPointToPointBeam& b);
};

// Sets the start and end points of the beam
inline void ESPPointToPointBeam::SetStartAndEndPoints(const Point3D& startPt, const Point3D& endPt) {
	this->startPt = startPt;
	this->endPt   = endPt;
    this->UpdateBeamGeometry();
}

inline void ESPPointToPointBeam::SetStartAndEndPoints(const Point2D& startPt, const Point2D& endPt) {
    this->startPt[0] = startPt[0]; this->startPt[1] = startPt[1]; this->startPt[2] = 0;
    this->endPt[0] = endPt[0]; this->endPt[1] = endPt[1]; this->endPt[2] = 0;
    this->UpdateBeamGeometry();
}

// Sets the general colour of the beam
inline void ESPPointToPointBeam::SetColour(const ColourRGBA& colour) {
	this->colour = colour;
    // Update all the beam colours as well...
    this->UpdateBeamColours();
}

inline void ESPPointToPointBeam::SetAlpha(float alpha) {
	this->colour[3] = alpha;
    // Update all the beam colours as well...
    this->UpdateBeamColours();
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