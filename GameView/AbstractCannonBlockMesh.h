/**
 * AbstractCannonBlockMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2014
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ABSTRACTCANNONBLOCKMESH_H__
#define __ABSTRACTCANNONBLOCKMESH_H__

#include "../BlammoEngine/Texture2D.h"

#include "../ESPEngine/ESPParticleScaleEffector.h"
#include "../ESPEngine/ESPParticleColourEffector.h"

class ESPPointEmitter;

class AbstractCannonBlockMesh {
public:
    AbstractCannonBlockMesh();
    virtual ~AbstractCannonBlockMesh();

protected:
   
    // Effects for drawing attention to active cannon blocks
    ESPParticleScaleEffector haloExpandPulse;
    ESPParticleScaleEffector attentionExpandPulse;
    ESPParticleColourEffector haloFader;
    ESPParticleColourEffector attentionFader;
    Texture2D* haloTexture;

    ESPPointEmitter* activeCannonEffectEmitter;
    ESPPointEmitter* ballCamAttentionEffectEmitter;

    DISALLOW_COPY_AND_ASSIGN(AbstractCannonBlockMesh);
};

#endif // __ABSTRACTCANNONBLOCKMESH_H__