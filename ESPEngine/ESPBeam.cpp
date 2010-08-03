#include "ESPBeam.h"

ESPBeam::ESPBeam(const Vector3D& beamLineVec, const Vector3D& rotationalVec, 
								 const ESPInterval& amplitudeVariationAmt, const ESPInterval& lineDistVariationAmt) : 
startSegment(new ESPBeamSegment(NULL)), 
lifeTimeInSecs(0.0), currLifeTickCount(0.0),
rotationalVec(rotationalVec), beamLineVec(beamLineVec), 
amplitudeVariationAmt(amplitudeVariationAmt), lineDistVariationAmt(lineDistVariationAmt) {
}

ESPBeam::~ESPBeam() {
	delete startSegment;
	startSegment = NULL;
}

void ESPBeam::Draw() const {
	std::list<const ESPBeamSegment*> beamSegs;
	beamSegs.push_back(this->startSegment);

	// For now just draw the main beam...
	glBegin(GL_LINE_STRIP);
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