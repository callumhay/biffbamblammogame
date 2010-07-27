
#ifndef __ESPBEAMSEGMENT_H__
#define __ESPBEAMSEGMENT_H__

#include "../BlammoEngine/Point.h"

class ESPBeamSegment {
public:
	ESPBeamSegment();
	~ESPBeamSegment();

	void SetEndPoint(const Point3D& endPt);
	const Point3D& GetEndPoint() const;
	ESPBeamSegment* AddESPBeamSegment();

	const std::list<ESPBeamSegment*>& GetChildSegments() const;

private:
	Point3D endPt;
	// 4 points to represent the quad...
	std::list<ESPBeamSegment*> childSegments;

	// Disallow copy and assign
	ESPBeamSegment(const ESPBeamSegment& b);
	ESPBeamSegment& operator=(const ESPBeamSegment& b);
};

inline void ESPBeamSegment::SetEndPoint(const Point3D& endPt) {
	this->endPt = endPt;
}

inline const Point3D& ESPBeamSegment::GetEndPoint() const {
	return this->endPt;
}

inline ESPBeamSegment* ESPBeamSegment::AddESPBeamSegment() {
	ESPBeamSegment* newBeamSeg = new ESPBeamSegment();
	this->childSegments.push_back(newBeamSeg);
	return newBeamSeg;
}

inline const std::list<ESPBeamSegment*>& ESPBeamSegment::GetChildSegments() const {
	return this->childSegments;
}

#endif // __ESPBEAMSEGMENT_H__