
#ifndef __ESPBEAMSEGMENT_H__
#define __ESPBEAMSEGMENT_H__

class ESPBeamSegment {
public:
	ESPBeamSegment() {};
	~ESPBeamSegment() {
		for (std::list<ESPBeamSegment*>::iterator iter = this->childSegments.begin(); iter != this->childSegments.end(); ++iter) {
			ESPBeamSegment* seg = *iter;
			delete seg;
			seg = NULL;
		}
		this->childSegments.clear();
	}

	void SetEndPoint(const Point3D& endPt) {
		this->endPt = endPt;
	}
	const Point3D& GetEndPoint() const {
		return this->endPt;
	}

	ESPBeamSegment* AddESPBeamSegment() {
		ESPBeamSegment* newBeamSeg = new ESPBeamSegment();
		this->childSegments.push_back(newBeamSeg);
		return newBeamSeg;
	}

private:
	Point3D endPt;
	// 4 points to represent the quad...
	std::list<ESPBeamSegment*> childSegments;
};

#endif // __ESPBEAMSEGMENT_H__