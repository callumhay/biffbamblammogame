/**
 * ESPAnimatedCurveParticle.cpp
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

#include "ESPAnimatedCurveParticle.h"

ESPAnimatedCurveParticle::ESPAnimatedCurveParticle(std::vector<Bezier*> bezierCurves, 
                                                   const ESPInterval& lineThickness,
                                                   const ESPInterval& timeToAnimateInterval) : 

ESPParticle(), lineThickness(lineThickness), possibleCurves(bezierCurves), timeToAnimateInterval(timeToAnimateInterval) {

    this->SelectRandomCurve();
}

ESPAnimatedCurveParticle::~ESPAnimatedCurveParticle() {
}

void ESPAnimatedCurveParticle::Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan) {
    ESPParticle::Revive(pos, vel, size, rot, totalLifespan);

    // Reset the curve's animation
    this->SelectRandomCurve();
}

void ESPAnimatedCurveParticle::Tick(const double dT) {

    this->currTimeCounter -= dT;
    if (this->currTimeCounter < 0.0) {
        this->currTimeCounter = 0.0;
    }

    ESPParticle::Tick(dT);
}

void ESPAnimatedCurveParticle::Draw(const Camera&, const ESP::ESPAlignment&) {

    // Don't draw if dead...
	if (this->IsDead()) {
		return;
	}

	glPushMatrix();
	glTranslatef(this->position[0], this->position[1], this->position[2]);

	glRotatef(this->rotation, 0, 0, -1);
	glScalef(this->size[0], this->size[1], 1.0f);
	glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), this->alpha);
	
    glLineWidth(this->currLineThickness);
	this->possibleCurves[this->currSelectedCurveIdx]->DrawGL(this->GetCurveT());
	
    glPopMatrix();
}