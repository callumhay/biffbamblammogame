/**
 * ESP.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

// The all including header file for the ESP (Emitter Sprite Particle) Engine API

// Utility classes and functions for the ESP library
#include "ESPUtil.h"

// Effectors
#include "ESPParticleEffector.h"
#include "ESPParticleAccelEffector.h"
#include "ESPParticleColourEffector.h"
#include "ESPParticleScaleEffector.h"
#include "ESPParticleRotateEffector.h"
#include "ESPParticleMagneticEffector.h"

// Emitters and particles
#include "ESPEmitter.h"
#include "ESPPointEmitter.h"
#include "ESPVolumeEmitter.h"
#include "ESPPointToPointBeam.h"
#include "ESPBeam.h"
#include "ESPParticle.h"
#include "ESPOnomataParticle.h"
#include "ESPShaderParticle.h"
