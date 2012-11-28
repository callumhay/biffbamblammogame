/**
 * TutorialInGameRenderPipeline.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __TUTORIALINGAMERENDERPIPELINE_H__
#define __TUTORIALINGAMERENDERPIPELINE_H__

#include "InGameRenderPipeline.h"

#include "../ESPEngine/ESP.h"

class TutorialInGameRenderPipeline : public InGameRenderPipeline {
public:
	TutorialInGameRenderPipeline(GameDisplay* display);
	~TutorialInGameRenderPipeline();

    ESPPointEmitter* AddEmbededTutorialHint(const Point3D& pos, const TextLabel2D& hintLabel, bool doPulse);

private:
    ESPParticleScaleEffector pulser;
    std::list<ESPPointEmitter*> tutorialHintEmitters;

    void DrawSpecialEmbeddedLabels(double dT);
    void SetSpecialLabelsAlpha(float alpha);
};

#endif // __TUTORIALINGAMERENDERPIPELINE_H__