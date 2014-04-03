/**
 * ESPBeam.cpp
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

#include "ESPBeam.h"
#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Collision.h"

ESPBeam::ESPBeam(const ColourRGBA& colour, const Vector3D& beamLineVec, const Vector3D& rotationalVec, 
                 const ESPInterval& amplitudeVariationAmt, const ESPInterval& lineDistVariationAmt) : 
startSegment(new ESPBeamSegment(NULL)), colour(colour),
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

	// Make sure we don't exceed the limits of OpenGL line widths
	float minMaxLineWidth[2];
	glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, minMaxLineWidth);
	thicknessInPixels = std::min<float>(minMaxLineWidth[1], std::max<float>(minMaxLineWidth[0], thicknessInPixels)); 

    glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), this->colour.A());

    glPointSize(thicknessInPixels);
    glBegin(GL_POINTS);
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

    beamSegs.push_back(this->startSegment);
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