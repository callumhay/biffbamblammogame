/**
 * BossMesh.cpp
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

#include "BossMesh.h"
#include "ClassicalBossMesh.h"
#include "GothicRomanticBossMesh.h"
#include "NouveauBossMesh.h"
#include "DecoBossMesh.h"
#include "FuturismBossMesh.h"
#include "GameViewConstants.h"
#include "CgFxBossWeakpoint.h"
#include "BasicEmitterUpdateStrategy.h"
#include "BossBodyPartEmitterUpdateStrategy.h"
#include "BossPt2PtBeamUpdateStrategy.h"
#include "PersistentTextureManager.h"

#include "../BlammoEngine/Texture2D.h"

#include "../GameSound/GameSound.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/GameLevel.h"
#include "../GameModel/ClassicalBoss.h"
#include "../GameModel/GothicRomanticBoss.h"
#include "../GameModel/NouveauBoss.h"
#include "../GameModel/DecoBoss.h"
#include "../GameModel/FuturismBoss.h"
#include "../GameModel/LaserBeamSightsEffectInfo.h"
#include "../GameModel/PowerChargeEffectInfo.h"
#include "../GameModel/ExpandingHaloEffectInfo.h"
#include "../GameModel/SparkBurstEffectInfo.h"
#include "../GameModel/ElectricitySpasmEffectInfo.h"
#include "../GameModel/ElectrifiedEffectInfo.h"
#include "../GameModel/SummonPortalsEffectInfo.h"
#include "../GameModel/BossTeleportEffectInfo.h"

#include "../ResourceManager.h"

BossMesh::BossMesh(GameSound* sound) : 
sound(sound), 
weakpointMaterial(NULL),
finalExplosionIsActive(false),
finalExplosionSoundID(INVALID_SOUND_ID),
particleSmallGrowth(1.0f, 1.3f), 
particleMediumGrowth(1.0f, 1.6f), 
particleLargeGrowth(1.0f, 2.2f),
particleSuperGrowth(1.0f, 5.0f),
particleMediumShrink(1.0f, 0.25f),
particleFader(1, 0),
smokeColourFader(ColourRGBA(0.7f, 0.7f, 0.7f, 1.0f), ColourRGBA(0.1f, 0.1f, 0.1f, 0.1f)),
particleFireColourFader(ColourRGBA(0.75f, 0.75f, 0.1f, 1.0f), ColourRGBA(0.75f, 0.2f, 0.2f, 0.0f)),
rotateEffectorCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::CLOCKWISE),
rotateEffectorCCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::COUNTER_CLOCKWISE),
laserSightBigToSmallSize(1.5f, 0.5f),
laserSightTargetColourChanger(ColourRGBA(0.0f, 1.0f, 0.0f, 0.8f), ColourRGBA(1.0f, 0.0f, 0.0f, 1.0f)),
laserSightTargetRotateEffector(100.0f, ESPParticleRotateEffector::COUNTER_CLOCKWISE),
particleShrinkToNothing(1, 0)
{
    assert(sound != NULL);

    // Initialize the smoke textures...
	this->smokeTextures.reserve(6);
    this->smokeTextures.push_back(PersistentTextureManager::GetInstance()->PreloadTexture2D(GameViewConstants::GetInstance()->TEXTURE_SMOKE1));
    this->smokeTextures.push_back(PersistentTextureManager::GetInstance()->PreloadTexture2D(GameViewConstants::GetInstance()->TEXTURE_SMOKE2));
    this->smokeTextures.push_back(PersistentTextureManager::GetInstance()->PreloadTexture2D(GameViewConstants::GetInstance()->TEXTURE_SMOKE3));
    this->smokeTextures.push_back(PersistentTextureManager::GetInstance()->PreloadTexture2D(GameViewConstants::GetInstance()->TEXTURE_SMOKE4));
    this->smokeTextures.push_back(PersistentTextureManager::GetInstance()->PreloadTexture2D(GameViewConstants::GetInstance()->TEXTURE_SMOKE5));
    this->smokeTextures.push_back(PersistentTextureManager::GetInstance()->PreloadTexture2D(GameViewConstants::GetInstance()->TEXTURE_SMOKE6));

    PersistentTextureManager::GetInstance()->PreloadTexture2D(GameViewConstants::GetInstance()->TEXTURE_EXPLOSION_ANIMATION);
    PersistentTextureManager::GetInstance()->PreloadTexture2D(GameViewConstants::GetInstance()->TEXTURE_SQUARE_TARGET);
    PersistentTextureManager::GetInstance()->PreloadTexture2D(GameViewConstants::GetInstance()->TEXTURE_HALO);
    PersistentTextureManager::GetInstance()->PreloadTexture2D(GameViewConstants::GetInstance()->TEXTURE_SPARKLE);
    PersistentTextureManager::GetInstance()->PreloadTexture2D(GameViewConstants::GetInstance()->TEXTURE_LIGHTNING_ANIMATION);
    PersistentTextureManager::GetInstance()->PreloadTexture2D(GameViewConstants::GetInstance()->TEXTURE_BRIGHT_FLARE);
    PersistentTextureManager::GetInstance()->PreloadTexture2D(GameViewConstants::GetInstance()->TEXTURE_LENSFLARE);

    this->lineAnim.ClearLerp();
    this->flashAnim.ClearLerp();

    this->weakpointMaterial = new CgFxBossWeakpoint();
}

BossMesh::~BossMesh() {
	this->smokeTextures.clear();

    // Clean up the effects emitters (if there are any)
    this->ClearActiveEffects();

    // Clean up all the handlers
    for (std::list<ExplodingEmitterHandler*>::iterator iter = this->explodingEmitterHandlers.begin();
         iter != this->explodingEmitterHandlers.end(); ++iter) {
        ExplodingEmitterHandler* handler = *iter;
        delete handler;
        handler = NULL;
    }
    this->explodingEmitterHandlers.clear();

    delete this->weakpointMaterial;
    this->weakpointMaterial = NULL;
}

BossMesh* BossMesh::Build(const GameWorld::WorldStyle& style, Boss* boss, GameSound* sound) {
    BossMesh* result = NULL;
    switch (style) {

        case GameWorld::Classical: {
            assert(dynamic_cast<ClassicalBoss*>(boss) != NULL);
            ClassicalBoss* classicalBoss = static_cast<ClassicalBoss*>(boss);
            result = new ClassicalBossMesh(classicalBoss, sound);
            break;
        }

        case GameWorld::GothicRomantic: {
            assert(dynamic_cast<GothicRomanticBoss*>(boss) != NULL);
            GothicRomanticBoss* gothicBoss = static_cast<GothicRomanticBoss*>(boss);
            result = new GothicRomanticBossMesh(gothicBoss, sound);
            break;
        }

        case GameWorld::Nouveau: {
            assert(dynamic_cast<NouveauBoss*>(boss) != NULL);
            NouveauBoss* nouveauBoss = static_cast<NouveauBoss*>(boss);
            result = new NouveauBossMesh(nouveauBoss, sound);
            break;
        }

        case GameWorld::Deco: {
            assert(dynamic_cast<DecoBoss*>(boss) != NULL);
            DecoBoss* decoBoss = static_cast<DecoBoss*>(boss);
            result = new DecoBossMesh(decoBoss, sound);
            break;
        }

        case GameWorld::Futurism: {
            assert(dynamic_cast<FuturismBoss*>(boss) != NULL);
            FuturismBoss* futurismBoss = static_cast<FuturismBoss*>(boss);
            result = new FuturismBossMesh(futurismBoss, sound);
            break;
        }

        //case GameWorld::SurrealismDada:
            // TODO

        //case GameWorld::Postmodernism:
            // TODO

        default:
            assert(false);
            break;

    }

    return result;
}

double BossMesh::ActivateBossExplodingFlashEffects(double delayInSecs, const GameModel* model,
                                                   const Camera& camera) {
    UNUSED_PARAMETER(camera);

    assert(model != NULL);
    assert(!this->finalExplosionIsActive);

    assert(delayInSecs < Boss::FADE_TO_BLACK_FINAL_DEAD_BODY_PART_TIME);
    double lineAnimTime = ((Boss::FADE_TO_BLACK_FINAL_DEAD_BODY_PART_TIME - delayInSecs) / 2.5) + delayInSecs; // 2.58

    const GameLevel* level = model->GetCurrentLevel();
    assert(level != NULL);
    
    this->lineAnim.SetLerp(delayInSecs, lineAnimTime, 0.0f, 2*level->GetLevelUnitWidth());
    this->flashAnim.SetLerp(lineAnimTime, Boss::TOTAL_DEATH_ANIM_TIME, 0.0f, 2*level->GetLevelUnitHeight());
    
    // The play back of the line+flash is as follows (in second intervals):
    // 1.5  --> 2.58 (total of 1.08 seconds): line expansion
    // 2.58 --> 5.25 (total of 2.67 seconds): flash expansion

    this->finalExplosionIsActive = true;
    this->finalExplosionSoundID  = INVALID_SOUND_ID;

    return lineAnimTime;
}

void BossMesh::ClearActiveEffects() {
    
    for (std::list<EffectUpdateStrategy*>::iterator iter = this->fgEffects.begin();
        iter != this->fgEffects.end(); ++iter) {
            delete *iter;
            *iter = NULL;
    }
    this->fgEffects.clear();

    for (std::list<ESPEmitter*>::iterator iter = this->bgEffectsEmitters.begin();
        iter != this->bgEffectsEmitters.end(); ++iter) {
            delete *iter;
            *iter = NULL;
    }
    this->bgEffectsEmitters.clear();

    for (std::list<EffectUpdateStrategy*>::iterator iter = this->laterPassEffects.begin();
        iter != this->laterPassEffects.end(); ++iter) {
            delete *iter;
            *iter = NULL;
    }
    this->laterPassEffects.clear();
}

void BossMesh::AddLaserBeamSightsEffect(const LaserBeamSightsEffectInfo& info) {

    ESPPointEmitter* spinningTarget = new ESPPointEmitter();
    spinningTarget->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    spinningTarget->SetInitialSpd(ESPInterval(0));
    spinningTarget->SetParticleLife(ESPInterval(info.GetDurationInSecs()));
    spinningTarget->SetParticleSize(ESPInterval(PlayerPaddle::PADDLE_WIDTH_TOTAL));
    spinningTarget->SetEmitAngleInDegrees(0);
    spinningTarget->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    spinningTarget->SetParticleAlignment(ESP::ScreenAligned);
    spinningTarget->SetEmitPosition(Point3D(info.GetTargetPoint(), 0));
    spinningTarget->SetParticleColour(ESPInterval(1), ESPInterval(1), ESPInterval(1), ESPInterval(0.5f));
    spinningTarget->AddEffector(&this->laserSightTargetRotateEffector);
    spinningTarget->AddEffector(&this->laserSightTargetColourChanger);
    spinningTarget->AddEffector(&this->laserSightBigToSmallSize);
    spinningTarget->SetParticles(1, PersistentTextureManager::GetInstance()->GetLoadedTexture(GameViewConstants::GetInstance()->TEXTURE_SQUARE_TARGET));

    this->fgEffects.push_back(new BasicEmitterUpdateStrategy(spinningTarget));
}

void BossMesh::AddBossPowerChargeEffect(const PowerChargeEffectInfo& info) {

    const BossBodyPart* bodyPart = info.GetChargingPart();
    const Colour& baseColour = info.GetColour();
    const Vector3D& posOffset = info.GetPositionOffset();

    std::vector<Colour> colours;
    colours.reserve(5);
    colours.push_back(baseColour);
    colours.push_back(1.5f * baseColour);
    colours.push_back(2.0f * baseColour);
    colours.push_back(2.5f * baseColour);
    colours.push_back(Colour(1,1,1));

    ESPPointEmitter* chargeParticles1 = new ESPPointEmitter();
    chargeParticles1->SetSpawnDelta(ESPInterval(0.025f, 0.05f));
    chargeParticles1->SetNumParticleLives(1);
    chargeParticles1->SetInitialSpd(ESPInterval(2.6f, 4.5f));
    chargeParticles1->SetParticleLife(ESPInterval(info.GetChargeTimeInSecs() * 0.75f, info.GetChargeTimeInSecs()));
    chargeParticles1->SetParticleSize(ESPInterval(info.GetSizeMultiplier() * 1.5f, info.GetSizeMultiplier() * 3.5f));
    chargeParticles1->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    chargeParticles1->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
    chargeParticles1->SetEmitDirection(Vector3D(0, 1, 0));
    chargeParticles1->SetEmitAngleInDegrees(180);
    chargeParticles1->SetIsReversed(true);
    chargeParticles1->SetParticleColourPalette(colours);
    chargeParticles1->AddEffector(&this->particleFader);
    chargeParticles1->AddEffector(&this->particleMediumShrink);
    chargeParticles1->SetParticles(20, PersistentTextureManager::GetInstance()->GetLoadedTexture(
        GameViewConstants::GetInstance()->TEXTURE_SPARKLE));

    static const unsigned int NUM_HALOS = 3;
    ESPPointEmitter* halo = new ESPPointEmitter();
    halo->SetSpawnDelta(ESPInterval(0.25f));
    halo->SetNumParticleLives(1);
    halo->SetParticleLife(ESPInterval(info.GetChargeTimeInSecs()));
    halo->SetParticleSize(ESPInterval(5.0f));
    halo->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    halo->SetParticleAlignment(ESP::ScreenAligned);
    halo->SetIsReversed(true);
    halo->SetParticleColour(ESPInterval(baseColour.R()), ESPInterval(baseColour.G()), 
        ESPInterval(baseColour.B()), ESPInterval(1.0f));
    halo->AddEffector(&this->particleShrinkToNothing);
    halo->SetParticles(NUM_HALOS, PersistentTextureManager::GetInstance()->GetLoadedTexture(
        GameViewConstants::GetInstance()->TEXTURE_HALO));

    BossBodyPartEmitterUpdateStrategy* strategy = new BossBodyPartEmitterUpdateStrategy(bodyPart);
    strategy->AddEmitter(halo);
    strategy->AddEmitter(chargeParticles1);
    strategy->SetOffset(posOffset);

    this->fgEffects.push_back(strategy);
}

void BossMesh::AddBossExpandingHaloEffect(const ExpandingHaloEffectInfo& info) {

    const BossBodyPart* bodyPart = info.GetPart();
    const Colour& colour = info.GetColour();
    const Vector3D& offsetVec = info.GetOffset();
    Point3D offsetPt(offsetVec[0], offsetVec[1], offsetVec[2]);

    Collision::AABB2D aabb = bodyPart->GetLocalBounds().GenerateAABBFromLines();
    float minSize = std::max<float>(LevelPiece::PIECE_HEIGHT, std::min<float>(aabb.GetHeight(), aabb.GetWidth()));

    ESPPointEmitter* halo = new ESPPointEmitter();
    halo->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    halo->SetNumParticleLives(1);
    halo->SetParticleLife(ESPInterval(info.GetTimeInSecs()));
    halo->SetParticleSize(ESPInterval(info.GetSizeMultiplier() * minSize));
    halo->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    halo->SetParticleAlignment(ESP::ScreenAligned);
    halo->SetEmitPosition(offsetPt);
    halo->SetParticleColour(ESPInterval(colour.R()), ESPInterval(colour.G()), ESPInterval(colour.B()), ESPInterval(0.8f));
    halo->AddEffector(&this->particleFader);
    halo->AddEffector(&this->particleSuperGrowth);
    halo->SetParticles(1, PersistentTextureManager::GetInstance()->GetLoadedTexture(GameViewConstants::GetInstance()->TEXTURE_HALO));

    BossBodyPartEmitterUpdateStrategy* strategy = new BossBodyPartEmitterUpdateStrategy(bodyPart);
    strategy->AddEmitter(halo);

    this->fgEffects.push_back(strategy);
}

void BossMesh::AddBossSparkBurstEffect(const SparkBurstEffectInfo& info) {

    const BossBodyPart* bodyPart = info.GetPart();
    const Colour& colour = info.GetColour();
    const Vector3D& offsetVec = info.GetOffset();
    Point3D offsetPt(offsetVec[0], offsetVec[1], offsetVec[2]);

    ESPPointEmitter* sparkParticles1 = new ESPPointEmitter();
    sparkParticles1->SetSpawnDelta(ESPEmitter::ONLY_SPAWN_ONCE);
    sparkParticles1->SetNumParticleLives(1);
    sparkParticles1->SetInitialSpd(ESPInterval(5.0f, 9.0f));
    sparkParticles1->SetParticleLife(ESPInterval(info.GetTimeInSecs()));
    sparkParticles1->SetParticleSize(ESPInterval(0.75f, 1.9f));
    sparkParticles1->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    sparkParticles1->SetParticleAlignment(ESP::ScreenAligned);
    sparkParticles1->SetEmitPosition(offsetPt);
    sparkParticles1->SetEmitDirection(Vector3D(0, 1, 0));
    sparkParticles1->SetEmitAngleInDegrees(180);
    sparkParticles1->SetParticleColour(
        ESPInterval(colour.R() * 0.75f, colour.R()), 
        ESPInterval(colour.G() * 0.75f, colour.G()),
        ESPInterval(colour.B() * 0.75f, colour.B()), ESPInterval(1.0f));
    sparkParticles1->AddEffector(&this->particleFader);
    sparkParticles1->AddEffector(&this->particleSmallGrowth);
    sparkParticles1->SetParticles(8, PersistentTextureManager::GetInstance()->GetLoadedTexture(GameViewConstants::GetInstance()->TEXTURE_SPARKLE));

    ESPPointEmitter* sparkParticles2 = new ESPPointEmitter();
    sparkParticles2->SetSpawnDelta(ESPEmitter::ONLY_SPAWN_ONCE);
    sparkParticles2->SetNumParticleLives(1);
    sparkParticles2->SetInitialSpd(ESPInterval(4.0f, 8.0f));
    sparkParticles2->SetParticleLife(ESPInterval(info.GetTimeInSecs()));
    sparkParticles2->SetParticleSize(ESPInterval(0.5f, 1.5f));
    sparkParticles2->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    sparkParticles2->SetParticleAlignment(ESP::ScreenAligned);
    sparkParticles2->SetEmitPosition(offsetPt);
    sparkParticles2->SetEmitDirection(Vector3D(0, 1, 0));
    sparkParticles2->SetEmitAngleInDegrees(180);
    sparkParticles2->SetParticleColour(ESPInterval(1), ESPInterval(1), ESPInterval(1), ESPInterval(0.8f));
    sparkParticles2->AddEffector(&this->particleFader);
    sparkParticles2->AddEffector(&this->particleSmallGrowth);
    sparkParticles2->SetParticles(5, PersistentTextureManager::GetInstance()->GetLoadedTexture(GameViewConstants::GetInstance()->TEXTURE_SPARKLE));

    if (bodyPart != NULL) {
        BossBodyPartEmitterUpdateStrategy* strategy = new BossBodyPartEmitterUpdateStrategy(bodyPart);
        strategy->AddEmitter(sparkParticles1);
        strategy->AddEmitter(sparkParticles2);
        this->fgEffects.push_back(strategy);
    }
    else {
        this->fgEffects.push_back(new BasicEmitterUpdateStrategy(sparkParticles1));
        this->fgEffects.push_back(new BasicEmitterUpdateStrategy(sparkParticles2));
    }
}

void BossMesh::AddElectricitySpasmEffect(const ElectricitySpasmEffectInfo& info) {

    static const double FPS = 24.0;
    static const int NUM_PARTICLES = 15;

    const BossBodyPart* bodyPart = info.GetPart();
    const Colour& colour = info.GetColour();
    Collision::AABB2D bodyPartAABB = bodyPart->GenerateWorldAABB();

    float maxSize = std::max<float>(bodyPartAABB.GetHeight(), bodyPartAABB.GetWidth());
    float minSize = std::min<float>(bodyPartAABB.GetHeight(), bodyPartAABB.GetWidth());

    ESPPointEmitter* electricSpasm = new ESPPointEmitter();
    electricSpasm->SetSpawnDelta(ESPInterval(0.02f, 0.1f));
    electricSpasm->SetNumParticleLives(1);
    electricSpasm->SetParticleLife(ESPInterval(0.5f * info.GetTimeInSecs(), 0.8f * info.GetTimeInSecs()));
    electricSpasm->SetParticleSize(ESPInterval(0.75f * minSize, 0.75f * maxSize));
    electricSpasm->SetRadiusDeviationFromCenter(ESPInterval(0.0f, bodyPartAABB.GetWidth()/2.25f),
        ESPInterval(0.0f, bodyPartAABB.GetHeight()/2.5f), ESPInterval(0.0f));
    electricSpasm->SetParticleAlignment(ESP::ScreenAligned);
    electricSpasm->SetEmitPosition(Point3D(0,0,0));
    electricSpasm->SetEmitDirection(Vector3D(0, 1, 0));
    electricSpasm->SetInitialSpd(ESPInterval(0.0f));
    electricSpasm->SetEmitAngleInDegrees(180);
    electricSpasm->SetParticleRotation(ESPInterval(0.0f, 359.9999f));
    electricSpasm->SetParticleColour(ESPInterval(colour.R()), ESPInterval(colour.G()),ESPInterval(colour.B()), ESPInterval(1.0f));
    electricSpasm->SetAnimatedParticles(NUM_PARTICLES, 
        PersistentTextureManager::GetInstance()->GetLoadedTexture(GameViewConstants::GetInstance()->TEXTURE_LIGHTNING_ANIMATION), 64, 64);

    BossBodyPartEmitterUpdateStrategy* strategy = new BossBodyPartEmitterUpdateStrategy(bodyPart);
    strategy->AddEmitter(electricSpasm);

    this->fgEffects.push_back(strategy);
}

void BossMesh::AddElectrifiedEffect(const ElectrifiedEffectInfo& info) {

    const Colour& colour = info.GetColour();
    
    ESPPointEmitter* electricSpasm = new ESPPointEmitter();
    electricSpasm->SetSpawnDelta(ESPEmitter::ONLY_SPAWN_ONCE);
    electricSpasm->SetNumParticleLives(1);
    electricSpasm->SetParticleLife(ESPInterval(info.GetTimeInSecs()));
    electricSpasm->SetParticleSize(ESPInterval(0.4f * info.GetSize(), 0.8f * info.GetSize()));
    electricSpasm->SetRadiusDeviationFromCenter(ESPInterval(0.0f, info.GetSize() * 0.5f),
        ESPInterval(0.0f, info.GetSize() * 0.5f), ESPInterval(0.0f));
    electricSpasm->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
    electricSpasm->SetEmitPosition(info.GetPosition());
    electricSpasm->SetEmitDirection(Vector3D(0, 1, 0));
    electricSpasm->SetInitialSpd(ESPInterval(0.0f));
    electricSpasm->SetEmitAngleInDegrees(180);
    electricSpasm->SetParticleRotation(ESPInterval(0.0f, 359.9999f));
    electricSpasm->SetParticleColour(ESPInterval(colour.R()), ESPInterval(colour.G()),ESPInterval(colour.B()), ESPInterval(1.0f));
    electricSpasm->SetAnimatedParticles(20, 
        PersistentTextureManager::GetInstance()->GetLoadedTexture(GameViewConstants::GetInstance()->TEXTURE_LIGHTNING_ANIMATION), 64, 64);

    this->bgEffectsEmitters.push_back(electricSpasm);
}

void BossMesh::AddSummonPortalEffect(const SummonPortalsEffectInfo& info) {
    Point3D bossPos = info.GetBodyPart()->GetTranslationPt3D() + info.GetOffset();
    Point3D portal1Pos(info.GetPortal1Pos(), 0);
    //Point3D portal2Pos(info.GetPortal2Pos(), 0);
    
    std::list<ESPAbstractEmitter*> emitters; 
    std::list<ESPPointToPointBeam*> beams;
    this->BuildSummonPortalEffects(bossPos, portal1Pos, info.GetTimeInSecs(), 
        info.GetSize(), info.GetPortalColour(), emitters, beams);

    BossPt2PtBeamUpdateStrategy* strategy1 = new BossPt2PtBeamUpdateStrategy(info.GetBodyPart(), beams);
    strategy1->SetOffset(info.GetOffset());
    BossBodyPartEmitterUpdateStrategy* strategy2 = new BossBodyPartEmitterUpdateStrategy(info.GetBodyPart(), emitters);
    strategy2->SetOffset(info.GetOffset());

    // NOTE: Beams should be drawn first!
    this->laterPassEffects.push_back(strategy1);
    this->fgEffects.push_back(strategy2);
}

void BossMesh::AddTeleportEffect(const BossTeleportEffectInfo& info) {
    // A portal opens at both the original and final locations of the teleportation
    // TODO

    // Energy particles are being sucked into the original location and spit out of the teleport location
    // TODO
}

void BossMesh::DrawPreBodyEffects(double dT, const Camera& camera) {
    UNUSED_PARAMETER(camera);

    if (this->finalExplosionIsActive) {
        this->lineAnim.Tick(dT);
        this->flashAnim.Tick(dT);

        Point3D explosionCenter = this->GetBossFinalExplodingEpicenter();

        glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_LINE_BIT);
	    glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    
        glColor4f(1,1,1,1);

        float lineWidth = this->lineAnim.GetInterpolantValue();
        if (lineWidth > 0.0f) {

            if (this->finalExplosionSoundID == INVALID_SOUND_ID) {
                this->finalExplosionSoundID = this->sound->PlaySound(GameSound::BossDeathFlashToFullscreenWhiteoutEvent, false, false);
            }

            glBegin(GL_LINES);
            glVertex3f(explosionCenter[0] - lineWidth, explosionCenter[1], explosionCenter[2]);
            glVertex3f(explosionCenter[0] + lineWidth, explosionCenter[1], explosionCenter[2]);
            glEnd();

        }
        float flashHeight = this->flashAnim.GetInterpolantValue();
        if (flashHeight > 0.0f) {
            glBegin(GL_QUADS);
            glVertex3f(explosionCenter[0] - lineWidth, explosionCenter[1] - flashHeight, explosionCenter[2]);
            glVertex3f(explosionCenter[0] + lineWidth, explosionCenter[1] - flashHeight, explosionCenter[2]);
            glVertex3f(explosionCenter[0] + lineWidth, explosionCenter[1] + flashHeight, explosionCenter[2]);
            glVertex3f(explosionCenter[0] - lineWidth, explosionCenter[1] + flashHeight, explosionCenter[2]);
            glEnd();
        }

        glPopAttrib();
    }
    else {
        // Draw currently active background effects
        for (std::list<ESPEmitter*>::iterator iter = this->bgEffectsEmitters.begin(); iter != this->bgEffectsEmitters.end();) {
            ESPEmitter* curr = *iter;
            assert(curr != NULL);

            // Check to see if dead, if so erase it...
            if (curr->IsDead()) {
                iter = this->bgEffectsEmitters.erase(iter);
                delete curr;
                curr = NULL;
            }
            else {
                // Not dead yet so we draw and tick
                curr->Tick(dT);
                curr->Draw(camera);
                ++iter;
            }
        }
    }
}

ESPPointEmitter* BossMesh::BuildFireEmitter(float width, float height, float sizeScaler) {
    float smallestDimension = std::min<float>(width, height);

    static const float MAX_SPAWN_DELTA = 0.16f;
    static const float MAX_LIFE = 1.5f;

	ESPPointEmitter* smokeEmitter = new ESPPointEmitter();
	smokeEmitter->SetSpawnDelta(ESPInterval(MAX_SPAWN_DELTA, MAX_SPAWN_DELTA));
	smokeEmitter->SetInitialSpd(ESPInterval(1.0f, 4.0f));
	smokeEmitter->SetParticleLife(ESPInterval(MAX_LIFE - 1.0f, MAX_LIFE));
    smokeEmitter->SetParticleSize(ESPInterval(0.25f * sizeScaler* smallestDimension, 0.75f * sizeScaler* smallestDimension));
	smokeEmitter->SetEmitAngleInDegrees(0);
	smokeEmitter->SetEmitDirection(Vector3D(0, 1, 0));
	smokeEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.25f * width), ESPInterval(0.0f, 0.5f * height), ESPInterval(0.0f));
	smokeEmitter->SetParticleAlignment(ESP::ScreenAligned);
	smokeEmitter->SetEmitPosition(Point3D(0.0f, 0.0f, 0.0f));
	smokeEmitter->AddEffector(&this->particleFireColourFader);
	smokeEmitter->AddEffector(&this->particleMediumGrowth);
	smokeEmitter->AddEffector(&this->rotateEffectorCW);
	
    int numParticles = static_cast<int>(MAX_LIFE / MAX_SPAWN_DELTA);
    assert(numParticles < 30);
    bool success = smokeEmitter->SetRandomTextureParticles(numParticles, this->smokeTextures);
	assert(success);
    UNUSED_VARIABLE(success);

    return smokeEmitter;
}
ESPPointEmitter* BossMesh::BuildSmokeEmitter(float width, float height, float sizeScaler) {
    float smallestDimension = std::min<float>(width, height);

    static const float MAX_SPAWN_DELTA = 0.16f;
    static const float MAX_LIFE = 1.75f;

	ESPPointEmitter* smokeEmitter = new ESPPointEmitter();
	smokeEmitter->SetSpawnDelta(ESPInterval(MAX_SPAWN_DELTA, MAX_SPAWN_DELTA));
	smokeEmitter->SetInitialSpd(ESPInterval(1.0f, 4.0f));
	smokeEmitter->SetParticleLife(ESPInterval(MAX_LIFE - 1.0f, MAX_LIFE));
    smokeEmitter->SetParticleSize(ESPInterval(0.3f * sizeScaler* smallestDimension, 0.8f * sizeScaler* smallestDimension));
	smokeEmitter->SetEmitAngleInDegrees(0);
	smokeEmitter->SetEmitDirection(Vector3D(0, 1, 0));
	smokeEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.25f * width), ESPInterval(0.0f, 0.5f * height), ESPInterval(0.0f));
	smokeEmitter->SetParticleAlignment(ESP::ScreenAligned);
	smokeEmitter->SetEmitPosition(Point3D(0.0f, 0.0f, 0.0f));
	smokeEmitter->AddEffector(&this->smokeColourFader);
	smokeEmitter->AddEffector(&this->particleLargeGrowth);
	smokeEmitter->AddEffector(&this->rotateEffectorCCW);
	
    int numParticles = static_cast<int>(MAX_LIFE / MAX_SPAWN_DELTA);
    assert(numParticles < 30);
    bool success = smokeEmitter->SetRandomTextureParticles(numParticles, this->smokeTextures);
	assert(success);
    UNUSED_VARIABLE(success);

    return smokeEmitter;
}


ESPPointEmitter* BossMesh::BuildExplodingEmitter(float volumeAmt, const AbstractBossBodyPart* bossBodyPart, float width, float height, float sizeScaler) {
    float largestDimension = std::max<float>(width, height);

    static const float MAX_SPAWN_DELTA = 0.2f;
    
    static const double FPS = 60.0;
    static const float MAX_LIFE = 4*16.0 / FPS;

	ESPPointEmitter* explosionEmitter = new ESPPointEmitter();
	explosionEmitter->SetSpawnDelta(ESPInterval(0.2f, MAX_SPAWN_DELTA));
	explosionEmitter->SetInitialSpd(ESPInterval(1.0f));
	explosionEmitter->SetParticleLife(ESPInterval(MAX_LIFE));
    explosionEmitter->SetParticleSize(ESPInterval(0.25f * sizeScaler * largestDimension, 0.5f * sizeScaler * largestDimension));
	explosionEmitter->SetEmitAngleInDegrees(0);
	explosionEmitter->SetEmitDirection(Vector3D(0, 1, 0));
	explosionEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.25f * width), ESPInterval(0.0f, 0.5f * height), ESPInterval(0.0f));
	explosionEmitter->SetParticleAlignment(ESP::ScreenAligned);
	explosionEmitter->SetEmitPosition(Point3D(0.0f, 0.0f, 0.0f));
	explosionEmitter->AddEffector(&this->particleLargeGrowth);

    this->explodingEmitterHandlers.push_back(new ExplodingEmitterHandler(this->sound, bossBodyPart, volumeAmt));
    explosionEmitter->AddEventHandler(this->explodingEmitterHandlers.back());
	
    int numParticles = static_cast<int>(MAX_LIFE / MAX_SPAWN_DELTA);
    assert(numParticles < 30);
    bool success = explosionEmitter->SetAnimatedParticles(numParticles, 
        PersistentTextureManager::GetInstance()->GetLoadedTexture(GameViewConstants::GetInstance()->TEXTURE_EXPLOSION_ANIMATION), 256, 256);
	assert(success);
    UNUSED_VARIABLE(success);

    return explosionEmitter;
}

void BossMesh::BuildSummonPortalEffects(const Point3D& bossPos, const Point3D& portalPos, double effectTime,
                                        float size, const Colour& portalColour, std::list<ESPAbstractEmitter*>& emitters, 
                                        std::list<ESPPointToPointBeam*>& beams) const {

    ESPMultiAlphaEffector fadeEffector;
    std::vector<std::pair<float, double> > alphaAndPercentages;
    alphaAndPercentages.reserve(4);
    alphaAndPercentages.push_back(std::make_pair(0.0f, 0.0));
    alphaAndPercentages.push_back(std::make_pair(1.0f, 0.1));
    alphaAndPercentages.push_back(std::make_pair(1.0f, 0.7));
    alphaAndPercentages.push_back(std::make_pair(0.0f, 1.0));

    fadeEffector.SetAlphasWithPercentage(alphaAndPercentages);

    // Beams...
    static const float NUM_SEGS_PER_UNIT = 0.5f;

    Vector3D beamVec = (portalPos-bossPos);
    assert(!beamVec.IsZero());
    float beamLength = beamVec.length();
    Colour brightPortalColour = Colour(0.66f,0.66f,0.66f) + 1.5f*portalColour;

    ESPPointToPointBeam* mainBeam = new ESPPointToPointBeam();
    mainBeam->SetStartAndEndPoints(bossPos, portalPos);
    mainBeam->SetColour(ColourRGBA(Colour(0.33f,0.33f,0.33f) + 1.5f*portalColour, 1.0f));
    mainBeam->SetBeamLifetime(ESPInterval(0.85*effectTime));
    mainBeam->SetNumBeamShots(1);
    mainBeam->SetMainBeamThickness(ESPInterval(size));
    mainBeam->SetNumMainESPBeamSegments(1);
    mainBeam->SetEnableDepth(false);
    mainBeam->AddCopiedEffector(fadeEffector);

    ESPPointToPointBeam* innerBeam = new ESPPointToPointBeam();
    innerBeam->SetStartAndEndPoints(bossPos, portalPos);
    innerBeam->SetColour(ColourRGBA(Colour(0.55f,0.55f,0.55f) + 1.5f*portalColour, 1.0f));
    innerBeam->SetBeamLifetime(ESPInterval(0.85*effectTime));
    innerBeam->SetNumBeamShots(1);
    innerBeam->SetMainBeamThickness(ESPInterval(0.5f*size));
    innerBeam->SetNumMainESPBeamSegments(1);
    innerBeam->SetEnableDepth(false);
    innerBeam->AddCopiedEffector(fadeEffector);

    static const int NUM_SMALLER_BEAMS = 3;
    ESPPointToPointBeam* smallerBeam = new ESPPointToPointBeam();
    smallerBeam->SetStartAndEndPoints(bossPos, portalPos);
    smallerBeam->SetColour(ColourRGBA(brightPortalColour, 1.0f));
    smallerBeam->SetBeamLifetime(ESPInterval(0.85*effectTime));
    //smallerBeam->SetTimeBetweenBeamShots(ESPInterval(TIME_BETWEEN_SHOTS));
    smallerBeam->SetNumBeamShots(NUM_SMALLER_BEAMS);
    smallerBeam->SetMainBeamAmplitude(ESPInterval(0.1f * size, 0.5f*size));
    smallerBeam->SetMainBeamThickness(ESPInterval(0.1f * size));
    smallerBeam->SetNumMainESPBeamSegments(static_cast<int>(beamLength * NUM_SEGS_PER_UNIT));
    smallerBeam->SetEnableDepth(false);
    smallerBeam->AddCopiedEffector(fadeEffector);

    beams.push_back(mainBeam);
    beams.push_back(innerBeam);
    beams.push_back(smallerBeam);

    // Emitters...
    // Set up the pulse effector for the flare emitter
    ScaleEffect flarePulseSettings;
    flarePulseSettings.pulseGrowthScale = 1.25f;
    flarePulseSettings.pulseRate        = 4.0f;
    ESPParticleScaleEffector flarePulse(flarePulseSettings);

    // NOTE: We don't position the emitters at the boss position since their draw strategy is different
    // (they will be automatically translated to the position of the body part associated with the effects)
    // Setup the flare emitter
    ESPPointEmitter* emitFlare = new ESPPointEmitter();
    emitFlare->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    emitFlare->SetInitialSpd(ESPInterval(0));
    emitFlare->SetParticleLife(ESPInterval(effectTime));
    emitFlare->SetEmitAngleInDegrees(0);
    emitFlare->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
    emitFlare->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    emitFlare->SetParticleSize(ESPInterval(4*size));
    emitFlare->SetParticleRotation(ESPInterval(0, 359.99f));
    emitFlare->SetParticleColour(ESPInterval(brightPortalColour.R()), 
        ESPInterval(brightPortalColour.G()), ESPInterval(brightPortalColour.B()), ESPInterval(1.0f));
    emitFlare->AddCopiedEffector(flarePulse);
    emitFlare->AddCopiedEffector(fadeEffector);
    emitFlare->SetParticles(1, PersistentTextureManager::GetInstance()->GetLoadedTexture(GameViewConstants::GetInstance()->TEXTURE_BRIGHT_FLARE));
    
    ESPPointEmitter* emitLensFlare = new ESPPointEmitter();
    emitLensFlare->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    emitLensFlare->SetInitialSpd(ESPInterval(0));
    emitLensFlare->SetParticleLife(ESPInterval(effectTime));
    emitLensFlare->SetEmitAngleInDegrees(0);
    emitLensFlare->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
    emitLensFlare->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    emitLensFlare->SetParticleSize(ESPInterval(5*size));
    emitLensFlare->SetParticleColour(ESPInterval(brightPortalColour.R()), 
        ESPInterval(brightPortalColour.G()), ESPInterval(brightPortalColour.B()), ESPInterval(1.0f));
    emitLensFlare->AddCopiedEffector(fadeEffector);
    emitLensFlare->SetParticles(1, PersistentTextureManager::GetInstance()->GetLoadedTexture(GameViewConstants::GetInstance()->TEXTURE_LENSFLARE));

    emitters.push_back(emitFlare);
    emitters.push_back(emitLensFlare);
}

void BossMesh::BuildShieldingColourAnimation(AnimationMultiLerp<Colour>& anim) {
    std::vector<double> timeVals;
    timeVals.reserve(3);
    timeVals.push_back(0.0);
    timeVals.push_back(0.7);
    timeVals.push_back(1.4);
    std::vector<Colour> colourVals;
    colourVals.reserve(timeVals.size());
    colourVals.push_back(Colour(1,1,1));
    colourVals.push_back(Colour(0.33f, 0.33f, 0.33f));
    colourVals.push_back(Colour(1,1,1));

    anim.SetLerp(timeVals, colourVals);
    anim.SetRepeat(true);
}

BossMesh::ExplodingEmitterHandler::ExplodingEmitterHandler(GameSound* sound, const AbstractBossBodyPart* bossBodyPart, float volumeAmt) : 
sound(sound), bossBodyPart(bossBodyPart), volumeLevel(volumeAmt) { 
    assert(sound != NULL); 
    assert(bossBodyPart != NULL);
};

void BossMesh::ExplodingEmitterHandler::ParticleSpawnedEvent(const ESPParticle* particle) {
    UNUSED_PARAMETER(particle);
    this->sound->PlaySound(GameSound::BossBlowingUpLoop, false, true, this->volumeLevel);
}
