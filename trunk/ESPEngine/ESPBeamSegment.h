/**
 * ESPBeamSegment.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __ESPBEAMSEGMENT_H__
#define __ESPBEAMSEGMENT_H__

#include "../BlammoEngine/Point.h"

class ESPBeam;

class ESPBeamSegment {
public:
	ESPBeamSegment(ESPBeamSegment* parent);
	~ESPBeamSegment();

	void SetDefaultPointOnLine(const Point3D& pt);
	const Point3D& GetEndPoint() const;
	ESPBeamSegment* AddESPBeamSegment();

	const std::list<ESPBeamSegment*>& GetChildSegments() const;

	void Tick(double dT, const ESPBeam& parentBeam);

private:
	Point3D defaultPtOnLine;
	Point3D endPt;
	double timeCounter;

	// 4 points to represent the quad...
	ESPBeamSegment* parentSegment;
	std::list<ESPBeamSegment*> childSegments;

	// Disallow copy and assign
	ESPBeamSegment(const ESPBeamSegment& b);
	ESPBeamSegment& operator=(const ESPBeamSegment& b);
};

inline void ESPBeamSegment::SetDefaultPointOnLine(const Point3D& pt) {
	this->defaultPtOnLine = pt;
	this->endPt = pt;
}

inline const Point3D& ESPBeamSegment::GetEndPoint() const {
	return this->endPt;
}

inline ESPBeamSegment* ESPBeamSegment::AddESPBeamSegment() {
	ESPBeamSegment* newBeamSeg = new ESPBeamSegment(this);
	this->childSegments.push_back(newBeamSeg);
	return newBeamSeg;
}

inline const std::list<ESPBeamSegment*>& ESPBeamSegment::GetChildSegments() const {
	return this->childSegments;
}

#endif // __ESPBEAMSEGMENT_H__