/**
 * TutorialInGameRenderPipeline.cpp
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

#include "TutorialInGameRenderPipeline.h"
#include "GameDisplay.h"

TutorialInGameRenderPipeline::TutorialInGameRenderPipeline(GameDisplay* display) :
InGameRenderPipeline(display), pulser(0,0) {

    ScaleEffect pulseSettings(0.2f, 1.15f);
    this->pulser = ESPParticleScaleEffector(pulseSettings);
}

TutorialInGameRenderPipeline::~TutorialInGameRenderPipeline() {
    // Clean up all the tutorial emitters
    for (std::list<ESPPointEmitter*>::iterator iter = this->tutorialHintEmitters.begin();
         iter != this->tutorialHintEmitters.end(); ++iter) {

        ESPPointEmitter* emitter = *iter;
        delete emitter;
        emitter = NULL;
    }
    this->tutorialHintEmitters.clear();

}

/**
 * Embeds a tutorial hint right into the current rendered level and returns a pointer to the emitter
 * that renders that hint. The returned pointer belongs to this object!
 */
ESPPointEmitter* TutorialInGameRenderPipeline::AddEmbededTutorialHint(const Point3D& pos,
                                                                      const TextLabel2D& hintLabel,
                                                                      bool doPulse) {

    ESPPointEmitter* tutorialHintEmitter = new ESPPointEmitter();
    tutorialHintEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    tutorialHintEmitter->SetParticleLife(ESPInterval(ESPParticle::INFINITE_PARTICLE_LIFETIME));
    tutorialHintEmitter->SetEmitPosition(pos);
    tutorialHintEmitter->SetInitialSpd(ESPInterval(0.0f));
    
    const ColourRGBA& labelColour = hintLabel.GetColour();
    tutorialHintEmitter->SetParticleColour(ESPInterval(labelColour.R()), ESPInterval(labelColour.G()),
        ESPInterval(labelColour.B()), ESPInterval(labelColour.A()));

    if (doPulse) {
        tutorialHintEmitter->AddEffector(&this->pulser);
    }

    tutorialHintEmitter->SetParticles(1, hintLabel);

    this->tutorialHintEmitters.push_back(tutorialHintEmitter);
    return tutorialHintEmitter;
}

void TutorialInGameRenderPipeline::DrawSpecialEmbeddedLabels(double dT) {

    Camera& camera = this->display->GetCamera();

    // Draw each of the tutorial emitters
    for (std::list<ESPPointEmitter*>::iterator iter = this->tutorialHintEmitters.begin();
         iter != this->tutorialHintEmitters.end(); ++iter) {

        ESPPointEmitter* emitter = *iter;
        emitter->Tick(dT);
        emitter->Draw(camera);
    }
}

void TutorialInGameRenderPipeline::SetSpecialLabelsAlpha(float alpha) {
    for (std::list<ESPPointEmitter*>::iterator iter = this->tutorialHintEmitters.begin();
         iter != this->tutorialHintEmitters.end(); ++iter) {
        ESPPointEmitter* emitter = *iter;
        emitter->SetParticleAlpha(ESPInterval(alpha));
    }
}