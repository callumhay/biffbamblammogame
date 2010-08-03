	
#ifndef __ESPBEAM_H__
#define __ESPBEAM_H__

#include "ESPUtil.h"
#include "ESPBeamSegment.h"

class ESPBeam {
public:
	static const int INFINITE_BEAM_LIFETIME				 = -1;

	ESPBeam(const Vector3D& beamLineVec, const Vector3D& rotationalVec, const ESPInterval& amplitudeVariationAmt, const ESPInterval& lineDistVariationAmt);
	~ESPBeam();

	ESPBeamSegment* GetStartSegment() const;
	void SetLifeTime(double lifeInSeconds);

	void Tick(double dT);
	void Draw() const;
	bool IsDead() const;

	const Vector3D& GetBeamLineVec() const;
	const Vector3D& GetOrthoBeamLineVec() const;
	const float GetRandomAmplitudeVariation() const;
	const float GetRandomLineDistanceVariation() const;

private:
	double currLifeTickCount;
	double lifeTimeInSecs;

	// Allowed interval of variation of the point over time
	Vector3D rotationalVec;							// Vector orthogonal to the beam line
	Vector3D beamLineVec;								// Vector from start to end of the beam (without any variations)
	ESPInterval amplitudeVariationAmt;	// Interval of allowed variation from the beam line
	ESPInterval lineDistVariationAmt;		// Interval of allowed variation along the beam line of any segment point

	//double variationSpd

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