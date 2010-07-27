#include "ESPBeamSegment.h"

ESPBeamSegment::ESPBeamSegment() {
}

ESPBeamSegment::~ESPBeamSegment() {
	for (std::list<ESPBeamSegment*>::iterator iter = this->childSegments.begin(); iter != this->childSegments.end(); ++iter) {
		ESPBeamSegment* seg = *iter;
		delete seg;
		seg = NULL;
	}
	this->childSegments.clear();
}