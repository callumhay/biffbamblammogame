/**
 * ESPBeamSegment.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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
    static const double ANIMATION_TIME;

	Point3D defaultPtOnLine;
	Point3D endPt;

    Point3D startPt;
    Point3D animationToPt;

	double timeCounter;

	ESPBeamSegment* parentSegment;
	std::list<ESPBeamSegment*> childSegments;

	// Disallow copy and assign
	ESPBeamSegment(const ESPBeamSegment& b);
	ESPBeamSegment& operator=(const ESPBeamSegment& b);
};

inline void ESPBeamSegment::SetDefaultPointOnLine(const Point3D& pt) {
	this->defaultPtOnLine = pt;
	this->endPt = pt;
    this->startPt = pt;
    this->animationToPt = pt;
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