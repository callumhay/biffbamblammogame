/**
 * ESPBeam.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#include "ESPBeam.h"
#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Collision.h"

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

void ESPBeam::Draw(const Point3D& startPt, const Point3D& endPt, const Camera& camera) const {
	std::list<const ESPBeamSegment*> beamSegs;
	beamSegs.push_back(this->startSegment);

	// Figure out the thickness (in pixels) of the line based on the camera distance and the
	// thickness of the beam in world units...

	// Calculate the distance from the camera along the look vector that the beam is...
	//Vector3D cameraViewVec   = camera.GetNormalizedViewVector();
	Vector3D distVec = Point3D::GetMidPoint(startPt, endPt) - camera.GetCurrentCameraPosition();
	float distanceFromCamera = distVec.length();

	// Calculate the height of the screen in world space
	float screenHeightInWorldUnits = 2.0f * tan(Trig::degreesToRadians(camera.GetFOVAngleInDegrees() / 2.0f)) * distanceFromCamera;
	// Now we simply use a ratio between the world units and the pixels based on the height of the screen
	float thicknessInPixels = static_cast<float>(this->thickness * camera.GetWindowHeight()) / screenHeightInWorldUnits;

	// Make sure we dont' exceed the limits of OpenGL line widths
	float minMaxLineWidth[2];
	glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, minMaxLineWidth);
	thicknessInPixels = std::min<float>(minMaxLineWidth[1], std::max<float>(minMaxLineWidth[0], thicknessInPixels)); 

	// For now just draw the main beam...
	glLineWidth(thicknessInPixels);
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