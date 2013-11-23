/**
 * ESPAnimatedCurveParticle.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
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