/**
 * TutorialInGameRenderPipeline.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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