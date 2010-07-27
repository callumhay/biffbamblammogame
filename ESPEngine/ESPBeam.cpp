#include "ESPBeam.h"

ESPBeam::ESPBeam() : startSegment(new ESPBeamSegment()), 
lifeTimeInSecs(0.0), currLifeTickCount(0.0) {
}

ESPBeam::~ESPBeam() {
	delete startSegment;
	startSegment = NULL;
}

void ESPBeam::Draw() const {
	std::list<const ESPBeamSegment*> beamSegs;
	beamSegs.push_back(this->startSegment);
	static const Point3D STARTING_PT(0,0,0);

	// For now just draw the main beam...
	glBegin(GL_LINE_STRIP);
	glVertex3fv(STARTING_PT.begin());

	while (!beamSegs.empty()) {
		const ESPBeamSegment* currSeg = beamSegs.front();
		beamSegs.pop_front();
		glVertex3fv(currSeg->GetEndPoint().begin());

		const std::list<ESPBeamSegment*>& childSegs = currSeg->GetChildSegments();
		if (childSegs.size() == 1) {
			beamSegs.push_back(childSegs.front());
		}
	}

	glEnd();

}