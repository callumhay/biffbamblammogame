/**
 * ESPMultiAlphaEffector.cpp
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

#include "ESPMultiAlphaEffector.h"
#include "ESPParticle.h"
#include "ESPBeam.h"

#include "../BlammoEngine/Animation.h"

ESPMultiAlphaEffector::ESPMultiAlphaEffector() : ESPEffector() {
}

void ESPMultiAlphaEffector::AffectParticleOnTick(double dT, ESPParticle* particle) {
	UNUSED_PARAMETER(dT);

	// If there are alphas to interpolate across then we setup an animation for them and
	// tick them to the current point in the particle's life
	if (this->alphas.size() == 0) {
		return;
	}
	else if (this->alphas.size() == 1) {
		particle->SetAlpha(this->alphas[0]);
		return;
	}

	particle->SetAlpha(this->CalculateAlpha(particle->GetCurrentLifeElapsed(), particle->GetLifespanLength()));
}

void ESPMultiAlphaEffector::AffectBeamOnTick(double dT, ESPBeam* beam) {
    UNUSED_PARAMETER(dT);

    // If there are alphas to interpolate across then we setup an animation for them and
    // tick them to the current point in the particle's life
    if (this->alphas.size() == 0) {
        return;
    }
    else if (this->alphas.size() == 1) {
        beam->SetAlpha(this->alphas[0]);
        return;
    }

    beam->SetAlpha(this->CalculateAlpha(beam->GetCurrentLifeElapsed(), beam->GetLifespanLength()));
}

void ESPMultiAlphaEffector::SetAlphas(const std::vector<float>& alphas) {
	this->alphas = alphas;
	this->percentages.clear();

	if (alphas.size() <= 1) {
		return;
	}

	this->percentages.reserve(alphas.size());

	double currPercentage = 0.0;
	double percentageIncrement = 1.0 / static_cast<float>(alphas.size()-1);
	for (size_t i = 0; i < alphas.size(); i++) {
		this->percentages.push_back(currPercentage);
		currPercentage += percentageIncrement;
	}
    this->percentages.back() = 1.0;

}

// Pairing is <alpha, percentage>, where the percentage is a decimal number [0,1].
// NOTE: The last percentage must always equal 1.
void ESPMultiAlphaEffector::SetAlphasWithPercentage(const std::vector<std::pair<float, double> >& alphas) {

    this->alphas.clear();
    this->percentages.clear();

    int addAmt = 0;
    if (alphas.front().second != 0.0) {
        addAmt++;
    }
    if (alphas.back().second != 1.0) {
        addAmt++;
    }

    this->alphas.reserve(alphas.size() + addAmt);
    this->percentages.reserve(alphas.size() + addAmt);

    if (alphas.front().second != 0.0) {
        this->percentages.push_back(0.0);
        this->alphas.push_back(alphas.front().first);
    }

    for (size_t i = 0; i < alphas.size(); i++) {
        this->alphas.push_back(alphas[i].first);
        this->percentages.push_back(alphas[i].second);
    }
    
    if (this->percentages.back() != 1.0) {
        this->alphas.push_back(this->alphas.back());
        this->percentages.push_back(1.0);
    }
}

float ESPMultiAlphaEffector::CalculateAlpha(double lifeElapsed, double totalLifespan) const {

    double percentLifeElapsed = std::min<float>(1.0f, lifeElapsed / totalLifespan);

    int lookupIndexBase = static_cast<int>(percentLifeElapsed * static_cast<double>(this->alphas.size()-1));
    int lookupIndexNext = (lookupIndexBase + 1);

    if (lookupIndexNext >= static_cast<int>(this->alphas.size())) {
        lookupIndexNext = this->alphas.size() - 1;
    }

    double startPercentage = this->percentages[lookupIndexBase];
    double endPercentage   = this->percentages[lookupIndexNext];
    float startAlpha = this->alphas[lookupIndexBase];
    float endAlpha   = this->alphas[lookupIndexNext];

    return static_cast<float>(startAlpha + (percentLifeElapsed - startPercentage) * (endAlpha - startAlpha) / (endPercentage - startPercentage));
}