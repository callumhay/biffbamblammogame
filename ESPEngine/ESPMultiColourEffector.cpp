/**
 * ESPMultiColourEffector.cpp
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

#include "ESPMultiColourEffector.h"
#include "ESPParticle.h"

#include "../BlammoEngine/Animation.h"

ESPMultiColourEffector::ESPMultiColourEffector() : ESPEffector() {
}

void ESPMultiColourEffector::AffectParticleOnTick(double dT, ESPParticle* particle) {
	UNUSED_PARAMETER(dT);

	// If there are colours to interpolate across then we setup an animation for them and
	// tick them to the current point in the particle's life
	if (this->colours.size() == 0) {
		return;
	}
	else if (this->colours.size() == 1) {
		particle->SetColour(this->colours[0]);
		return;
	}

	double particleLifeElapsed = particle->GetCurrentLifeElapsed();
	double totalParticleLife   = particle->GetLifespanLength();
    double percentLifeElapsed  = std::min<float>(1.0f, particleLifeElapsed / totalParticleLife);

	int lookupIndexBase = static_cast<int>(percentLifeElapsed * (static_cast<double>(this->colours.size())-1));
	int lookupIndexNext = (lookupIndexBase + 1);
	
	if (lookupIndexNext >= static_cast<int>(this->colours.size())) {
		lookupIndexNext = static_cast<int>(this->colours.size()) - 1;
        
        if (lookupIndexNext == lookupIndexBase) {
            particle->SetColour(this->colours.back());
            return;
        }
	}

	double startPercentage = this->percentages[lookupIndexBase];
	double endPercentage   = this->percentages[lookupIndexNext];
	
    Vector4D startColour = this->colours[lookupIndexBase].GetAsVector4D();
	Vector4D endColour   = this->colours[lookupIndexNext].GetAsVector4D();
    Vector4D finalColour = startColour + (percentLifeElapsed - startPercentage) * (endColour - startColour) / (endPercentage - startPercentage);

	particle->SetColour(ColourRGBA(finalColour));
}

void ESPMultiColourEffector::AffectBeamOnTick(double, ESPBeam*) {
    assert(false);
    // NOT IMPLEMENTED YET
}

void ESPMultiColourEffector::SetColours(const std::vector<ColourRGBA>& colours) {
	this->colours = colours;
	this->percentages.clear();

	if (colours.empty()) {
		return;
	}
    else if (colours.size() == 1) {
        this->percentages.reserve(1);
        this->percentages.push_back(1.0);
        return;
    }

	this->percentages.reserve(colours.size());

	double currPercentage = 0.0;
    double percentageIncrement = 1.0 / static_cast<double>(colours.size()-1);
	for (int i = 0; i < static_cast<int>(colours.size()); i++) {
		this->percentages.push_back(currPercentage);
		currPercentage += percentageIncrement;
	}
    this->percentages.back() = 1.0;
}

void ESPMultiColourEffector::SetColoursWithPercentage(const std::vector<std::pair<ColourRGBA, double> >& colours) {
    this->colours.clear();
    this->percentages.clear();

    int addAmt = 0;
    if (colours.front().second != 0.0) {
        addAmt++;
    }
    if (colours.back().second != 1.0) {
        addAmt++;
    }

    this->colours.reserve(colours.size() + addAmt);
    this->percentages.reserve(colours.size() + addAmt);

    if (colours.front().second != 0.0) {
        this->percentages.push_back(0.0);
        this->colours.push_back(colours.front().first);
    }

    for (size_t i = 0; i < colours.size(); i++) {
        this->colours.push_back(colours[i].first);
        this->percentages.push_back(colours[i].second);
    }

    if (this->percentages.back() != 1.0) {
        this->colours.push_back(this->colours.back());
        this->percentages.push_back(1.0);
    }
}