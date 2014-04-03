/**
 * ESPBeam.h
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

#ifndef __ESPBEAM_H__
#define __ESPBEAM_H__

#include "../BlammoEngine/Colour.h"

#include "ESPUtil.h"
#include "ESPBeamSegment.h"

class ESPBeam {
public:
	static const int INFINITE_BEAM_LIFETIME = -1;

	ESPBeam(const ColourRGBA& colour, const Vector3D& beamLineVec, const Vector3D& rotationalVec,
            const ESPInterval& amplitudeVariationAmt, const ESPInterval& lineDistVariationAmt);
	~ESPBeam();

	ESPBeamSegment* GetStartSegment() const;
	void SetLifeTime(double lifeInSeconds);
	void SetThickness(float thickness);

    void SetColour(const ColourRGBA& colour) { this->colour = colour; }
    void SetColour(const Colour& colour) { this->colour.SetColour(colour); }
    void SetAlpha(float alpha) { this->colour[3] = alpha; }
    
	void Tick(double dT);
	void Draw(const Point3D& startPt, const Point3D& endPt, const Camera& camera) const;
	bool IsDead() const;

	const Vector3D& GetBeamLineVec() const;
	const Vector3D& GetOrthoBeamLineVec() const;
	const float GetRandomAmplitudeVariation() const;
	const float GetRandomLineDistanceVariation() const;

    double GetCurrentLifeElapsed() const { return this->currLifeTickCount; }
    double GetLifespanLength() const { return this->lifeTimeInSecs; }

private:
	double currLifeTickCount;
	double lifeTimeInSecs;

    ColourRGBA colour;

	// Allowed interval of variation of the point over time
	Vector3D rotationalVec;             // Vector orthogonal to the beam line
	Vector3D beamLineVec;               // Vector from start to end of the beam (without any variations)
	ESPInterval amplitudeVariationAmt;	// Interval of allowed variation from the beam line
	ESPInterval lineDistVariationAmt;   // Interval of allowed variation along the beam line of any segment point
	float thickness;

	ESPBeamSegment* startSegment;	// The root/starting beam segment

	// Disallow copy and assign
	ESPBeam(const ESPBeam& b);
	ESPBeam& operator=(const ESPBeam& b);
};

inline ESPBeamSegment* ESPBeam::GetStartSegment() const {
	return this->startSegment;
}

inline void ESPBeam::SetLifeTime(double lifeInSeconds) {
	this->lifeTimeInSecs = lifeInSeconds;
}

inline void ESPBeam::SetThickness(float thickness) {
	assert(thickness > 0.0f);
	this->thickness = thickness;
}

inline void ESPBeam::Tick(double dT) {
	this->currLifeTickCount += dT;
	// Tick all of the segments
	this->startSegment->Tick(dT, *this);
}

inline bool ESPBeam::IsDead() const {
	return (this->lifeTimeInSecs != ESPBeam::INFINITE_BEAM_LIFETIME) && (this->lifeTimeInSecs <= this->currLifeTickCount);
}

inline const Vector3D& ESPBeam::GetBeamLineVec() const {
	return this->beamLineVec;
}

inline const Vector3D& ESPBeam::GetOrthoBeamLineVec() const {
	return this->rotationalVec;
}

inline const float ESPBeam::GetRandomAmplitudeVariation() const {
	return this->amplitudeVariationAmt.RandomValueInInterval();
}

inline const float ESPBeam::GetRandomLineDistanceVariation() const {
	return this->lineDistVariationAmt.RandomValueInInterval();
}

#endif // __ESPBEAM_H__