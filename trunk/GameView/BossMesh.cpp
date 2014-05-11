/**
 * BossMesh.cpp
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

#include "BossMesh.h"
#include "ClassicalBossMesh.h"
#include "GothicRomanticBossMesh.h"
#include "NouveauBossMesh.h"
#include "DecoBossMesh.h"
#include "FuturismBossMesh.h"
#include "GameViewConstants.h"
#include "BasicEmitterUpdateStrategy.h"
#include "BossBodyPartEmitterUpdateStrategy.h"
#include "BossPt2PtBeamUpdateStrategy.h"
#include "BossBallBeamUpdateStrategy.h"
#include "PersistentTextureManager.h"
#include "GameESPAssets.h"
#include "GameFontAssetsManager.h"

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
#include "../GameModel/EnumBossEffectInfo.h"

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
    PersistentTextureManager::GetInstance()->PreloadTexture2D(GameViewConstants::GetInstance()->TEXTURE_CIRCLE);
    PersistentTextureManager::GetInstance()->PreloadTexture2D(GameViewConstants::GetInstance()->TEXTURE_HOOP);
    PersistentTextureManager::GetInstance()->PreloadTexture2D(GameViewConstants::GetInstance()->TEXTURE_CLEAN_CIRCLE_GRADIENT);

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

    for (std::list<EffectUpdateStrategy*>::iterator iter = this->bgEffects.begin();
        iter != this->bgEffects.end(); ++iter) {
            delete *iter;
            *iter = NULL;
    }
    this->bgEffects.clear();

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
    spinningTarget->SetParticleAlignment(ESP::ScreenPlaneAligned);
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

    this->bgEffects.push_back(new BasicEmitterUpdateStrategy(electricSpasm));
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
    this->laterPassEffects.push_back(strategy2);
}

void BossMesh::AddTeleportEffect(const BossTeleportEffectInfo& info) {
    switch (info.GetTeleportType()) {

        case BossTeleportEffectInfo::TeleportingOut: {

            float halfSize = info.GetSize() / 2.0f;
            Point3D emitPos(info.GetPosition(),0);
            
            ESPMultiAlphaEffector circleAlpha;
            std::vector<std::pair<float, double> > alphaAndPercentages;
            alphaAndPercentages.reserve(4);
            alphaAndPercentages.push_back(std::make_pair(0.0f, 0.0));
            alphaAndPercentages.push_back(std::make_pair(1.0f, 0.5));
            alphaAndPercentages.push_back(std::make_pair(1.0f, 0.9));
            alphaAndPercentages.push_back(std::make_pair(0.0f, 1.0));
            circleAlpha.SetAlphasWithPercentage(alphaAndPercentages);
            
            ESPPointEmitter* teleportCircle = new ESPPointEmitter();
            teleportCircle->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
            teleportCircle->SetNumParticleLives(1);
            teleportCircle->SetParticleLife(ESPInterval(info.GetTimeInSeconds()));
            teleportCircle->SetParticleSize(ESPInterval(1));
            teleportCircle->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
            teleportCircle->SetParticleAlignment(ESP::ScreenAligned);
            teleportCircle->SetEmitPosition(emitPos);
            teleportCircle->SetParticleColour(Colour(1,1,1));
            teleportCircle->AddCopiedEffector(ESPParticleScaleEffector(0.01, 2*info.GetSize()));
            teleportCircle->AddCopiedEffector(circleAlpha);
            teleportCircle->SetParticles(1, PersistentTextureManager::GetInstance()->GetLoadedTexture(
                GameViewConstants::GetInstance()->TEXTURE_CLEAN_CIRCLE_GRADIENT));

            static const int NUM_MEZZ_PARTICLES = 4;

            float deltaAndLife = info.GetTimeInSeconds() / (NUM_MEZZ_PARTICLES+1);
            ESPPointEmitter* teleportMezz = new ESPPointEmitter();
            teleportMezz->SetSpawnDelta(ESPInterval(deltaAndLife));
            teleportMezz->SetNumParticleLives(1);
            teleportMezz->SetParticleLife(ESPInterval(2*deltaAndLife));
            teleportMezz->SetParticleSize(ESPInterval(1));
            teleportMezz->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
            teleportMezz->SetParticleAlignment(ESP::ScreenAligned);
            teleportMezz->SetEmitPosition(emitPos);
            teleportMezz->SetParticleColour(Colour(0,0,0));
            teleportMezz->AddCopiedEffector(ESPParticleScaleEffector(0.01, info.GetSize()));
            teleportMezz->AddCopiedEffector(circleAlpha);
            teleportMezz->SetParticles(NUM_MEZZ_PARTICLES, PersistentTextureManager::GetInstance()->GetLoadedTexture(
                GameViewConstants::GetInstance()->TEXTURE_HOOP));

            const ESPInterval lifeInterval(info.GetTimeInSeconds() * 0.1f, info.GetTimeInSeconds() * 0.25f);
            
            static const int NUM_PARTICLES = 70;
            ESPInterval deltaSpawnInterval(
                info.GetTimeInSeconds() / static_cast<float>(2.5f*NUM_PARTICLES),
                info.GetTimeInSeconds() / static_cast<float>(2.25f*NUM_PARTICLES));

            std::vector<Texture2D*> suckedParticleTextures(2, NULL);
            suckedParticleTextures[0] = PersistentTextureManager::GetInstance()->GetLoadedTexture(
                GameViewConstants::GetInstance()->TEXTURE_CIRCLE);
            suckedParticleTextures[1] = PersistentTextureManager::GetInstance()->GetLoadedTexture(
                GameViewConstants::GetInstance()->TEXTURE_HOOP);

            ESPPointEmitter* suckedParticles1 = new ESPPointEmitter();
            suckedParticles1->SetSpawnDelta(deltaSpawnInterval);
            suckedParticles1->SetNumParticleLives(1);
            suckedParticles1->SetInitialSpd(ESPInterval(halfSize / lifeInterval.maxValue, halfSize / lifeInterval.minValue));
            suckedParticles1->SetParticleLife(lifeInterval);
            suckedParticles1->SetParticleSize(ESPInterval(info.GetSize() / 13.0f, info.GetSize() / 4.0f));
            suckedParticles1->SetParticleAlignment(ESP::ScreenAligned);
            suckedParticles1->SetEmitDirection(Vector3D(0, 1, 0));
            suckedParticles1->SetEmitPosition(emitPos);
            suckedParticles1->SetEmitAngleInDegrees(180);
            suckedParticles1->SetIsReversed(true);
            suckedParticles1->SetToggleEmitOnPlane(true);
            suckedParticles1->SetParticleColour(Colour(0,0,0));
            suckedParticles1->SetCutoffLifetime(info.GetTimeInSeconds());
            suckedParticles1->AddCopiedEffector(circleAlpha);
            suckedParticles1->AddCopiedEffector(ESPParticleScaleEffector(1.0f, 0.01f));
            suckedParticles1->SetRandomTextureParticles(NUM_PARTICLES, suckedParticleTextures);

            BasicEmitterUpdateStrategy* strategy = new BasicEmitterUpdateStrategy();
            strategy->AddEmitter(teleportCircle);
            strategy->AddEmitter(teleportMezz);
            strategy->AddEmitter(suckedParticles1);

            this->bgEffects.push_back(strategy);

            break;
        }

        case BossTeleportEffectInfo::FastTeleportOut: {

            ESPMultiAlphaEffector circleAlpha;
            std::vector<std::pair<float, double> > alphaAndPercentages;
            alphaAndPercentages.reserve(4);
            alphaAndPercentages.push_back(std::make_pair(0.0f, 0.0));
            alphaAndPercentages.push_back(std::make_pair(0.75f, 0.5));
            alphaAndPercentages.push_back(std::make_pair(0.75f, 0.9));
            alphaAndPercentages.push_back(std::make_pair(0.0f, 1.0));
            circleAlpha.SetAlphasWithPercentage(alphaAndPercentages);

            Point3D emitPos(info.GetPosition(),0);

            ESPPointEmitter* teleportCircle = new ESPPointEmitter();
            teleportCircle->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
            teleportCircle->SetNumParticleLives(1);
            teleportCircle->SetParticleLife(ESPInterval(info.GetTimeInSeconds()));
            teleportCircle->SetParticleSize(ESPInterval(1));
            teleportCircle->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
            teleportCircle->SetParticleAlignment(ESP::ScreenAligned);
            teleportCircle->SetEmitPosition(emitPos);
            teleportCircle->SetParticleColour(Colour(1,1,1));
            teleportCircle->AddCopiedEffector(ESPParticleScaleEffector(0.01, 2*info.GetSize()));
            teleportCircle->AddCopiedEffector(circleAlpha);
            teleportCircle->SetParticles(1, PersistentTextureManager::GetInstance()->GetLoadedTexture(
                GameViewConstants::GetInstance()->TEXTURE_CLEAN_CIRCLE_GRADIENT));

            BasicEmitterUpdateStrategy* strategy = new BasicEmitterUpdateStrategy();
            strategy->AddEmitter(teleportCircle);
            
            this->bgEffects.push_back(strategy);

            break;
        }

        case BossTeleportEffectInfo::TeleportingIn: {
            /*
            static const int NUM_PARTICLES = 20;
            Point3D emitPos(info.GetPosition(),0);

            std::vector<Texture2D*> suckedParticleTextures(2, NULL);
            suckedParticleTextures[0] = PersistentTextureManager::GetInstance()->GetLoadedTexture(
                GameViewConstants::GetInstance()->TEXTURE_CIRCLE);
            suckedParticleTextures[1] = PersistentTextureManager::GetInstance()->GetLoadedTexture(
                GameViewConstants::GetInstance()->TEXTURE_HOOP);

            ESPPointEmitter* spatParticles = new ESPPointEmitter();
            spatParticles->SetSpawnDelta(ESPInterval(0.005f, 0.015f));
            spatParticles->SetNumParticleLives(1);
            spatParticles->SetInitialSpd(ESPInterval(8.0f, 16.0f));
            spatParticles->SetParticleLife(ESPInterval(0.25f*info.GetTimeInSeconds(), 0.75f*info.GetTimeInSeconds()));
            spatParticles->SetParticleSize(ESPInterval(info.GetSize() / 20.0f, info.GetSize() / 16.0f));
            spatParticles->SetParticleAlignment(ESP::ScreenAligned);
            spatParticles->SetEmitDirection(Vector3D(0, 1, 0));
            spatParticles->SetEmitPosition(emitPos);
            spatParticles->SetEmitAngleInDegrees(180);
            spatParticles->SetToggleEmitOnPlane(true);
            spatParticles->SetParticleColour(Colour(0,0,0));
            spatParticles->SetCutoffLifetime(info.GetTimeInSeconds());
            spatParticles->AddEffector(&this->particleFader);
            spatParticles->AddCopiedEffector(ESPParticleScaleEffector(1.0f, 1.5f));
            spatParticles->SetRandomTextureParticles(NUM_PARTICLES, suckedParticleTextures);
            
            BasicEmitterUpdateStrategy* strategy = new BasicEmitterUpdateStrategy();
            strategy->AddEmitter(spatParticles);
            this->bgEffects.push_back(strategy);
            */
            break;
        }

        case BossTeleportEffectInfo::TeleportFailed:
            break;

        default:
            assert(false);
            return;
    }
}

void BossMesh::AddEnumEffect(const EnumBossEffectInfo& info) {
    
    switch (info.GetSpecificType()) {
        case EnumBossEffectInfo::FrozenIceClouds: {
            // TODO
            break;
        }

        case EnumBossEffectInfo::FrozenShakeDebris: {
            std::vector<Texture2D*> rockTextures;
            GameESPAssets::GetRockTextures(rockTextures);

            const Colour iceColour(0.75, 0.88f, 0.95f);
            Point3D emitCenter(info.GetBodyPart()->GetTranslationPt2D(), 0.0f);

            // Create the smashy block bits
            ESPPointEmitter* smashBitsEffect = new ESPPointEmitter();
            smashBitsEffect->SetSpawnDelta(ESPInterval(ESPPointEmitter::ONLY_SPAWN_ONCE));
            smashBitsEffect->SetInitialSpd(ESPInterval(5.0f, 8.0f));
            smashBitsEffect->SetParticleLife(ESPInterval(0.75f*info.GetTimeInSecs(), info.GetTimeInSecs()));
            smashBitsEffect->SetEmitDirection(Vector3D(0, 1, 0));
            smashBitsEffect->SetEmitAngleInDegrees(180);
            smashBitsEffect->SetToggleEmitOnPlane(true);
            smashBitsEffect->SetParticleSize(ESPInterval(info.GetSize1D() / 18.0f, info.GetSize1D() / 8.5f));
            smashBitsEffect->SetParticleRotation(ESPInterval(-180.0f, 180.0f));
            smashBitsEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, info.GetSize1D() / 4.0f));
            smashBitsEffect->SetEmitPosition(emitCenter);
            smashBitsEffect->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
            smashBitsEffect->SetParticleColour(
                ESPInterval(iceColour.R(), 1.0f), 
                ESPInterval(iceColour.G(), 1.0f),
                ESPInterval(iceColour.B(), 1.0f), ESPInterval(1.0f));
            smashBitsEffect->AddCopiedEffector(ESPParticleAccelEffector(Vector3D(0,-9.8f,0)));
            smashBitsEffect->AddEffector(&this->particleFader);
            smashBitsEffect->SetRandomTextureParticles(8, rockTextures);

            this->fgEffects.push_back(new BasicEmitterUpdateStrategy(smashBitsEffect));

            break;
        }

        case EnumBossEffectInfo::IceBreak: {

            std::vector<Texture2D*> rockTextures;
            GameESPAssets::GetRockTextures(rockTextures);
            std::vector<Texture2D*> snowflakeTextures;
            GameESPAssets::GetSnowflakeTextures(snowflakeTextures);

            const Colour iceColour(0.75, 0.88f, 0.95f);
            Point3D emitCenter(info.GetBodyPart()->GetTranslationPt2D(), 0.0f);

            // Create the smashy block bits
            ESPPointEmitter* smashBitsEffect = new ESPPointEmitter();
            smashBitsEffect->SetSpawnDelta(ESPInterval(ESPPointEmitter::ONLY_SPAWN_ONCE));
            smashBitsEffect->SetInitialSpd(ESPInterval(9.0f, 14.0f));
            smashBitsEffect->SetParticleLife(ESPInterval(0.75f*info.GetTimeInSecs(), info.GetTimeInSecs()));
            smashBitsEffect->SetEmitDirection(Vector3D(0, 1, 0));
            smashBitsEffect->SetEmitAngleInDegrees(180);
            smashBitsEffect->SetToggleEmitOnPlane(true);
            smashBitsEffect->SetParticleSize(ESPInterval(info.GetSize1D() / 12.0f, info.GetSize1D() / 4.0f));
            smashBitsEffect->SetParticleRotation(ESPInterval(-180.0f, 180.0f));
            smashBitsEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, info.GetSize1D() / 4.0f));
            smashBitsEffect->SetEmitPosition(emitCenter);
            smashBitsEffect->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
            smashBitsEffect->SetParticleColour(
                ESPInterval(iceColour.R(), 1.0f), 
                ESPInterval(iceColour.G(), 1.0f),
                ESPInterval(iceColour.B(), 1.0f), ESPInterval(1.0f));
            smashBitsEffect->AddCopiedEffector(ESPParticleAccelEffector(Vector3D(0,-7.0f,0)));
            smashBitsEffect->AddEffector(&this->particleFader);
            smashBitsEffect->SetRandomTextureParticles(15, rockTextures);

            // Snowflakey bits...
            ESPPointEmitter* snowflakeBitsEffect = new ESPPointEmitter();
            snowflakeBitsEffect->SetSpawnDelta(ESPInterval(ESPPointEmitter::ONLY_SPAWN_ONCE));
            snowflakeBitsEffect->SetInitialSpd(ESPInterval(5.0f, 9.0f));
            snowflakeBitsEffect->SetParticleLife(ESPInterval(1.75f, 2.75f));
            snowflakeBitsEffect->SetEmitDirection(Vector3D(0, 1, 0));
            snowflakeBitsEffect->SetEmitAngleInDegrees(180);
            snowflakeBitsEffect->SetToggleEmitOnPlane(true);
            snowflakeBitsEffect->SetParticleSize(ESPInterval(info.GetSize1D() / 6.0f, info.GetSize1D() / 2.0f));
            snowflakeBitsEffect->SetParticleRotation(ESPInterval(-180.0f, 180.0f));
            snowflakeBitsEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, info.GetSize1D() / 4.0f));
            snowflakeBitsEffect->SetEmitPosition(emitCenter);
            snowflakeBitsEffect->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
            snowflakeBitsEffect->SetParticleColour(
                ESPInterval(iceColour.R(), 1.0f), ESPInterval(iceColour.G(), 1.0f),
                ESPInterval(1.0f), ESPInterval(1.0f));
            snowflakeBitsEffect->AddEffector(&this->particleFader);
            snowflakeBitsEffect->AddCopiedEffector(ESPParticleRotateEffector(160.0f, ESPParticleRotateEffector::RandomDirection()));
            snowflakeBitsEffect->SetRandomTextureParticles(12, snowflakeTextures);

            ESPPointEmitter* otherBits = new ESPPointEmitter();
            otherBits->SetSpawnDelta(ESPInterval(ESPPointEmitter::ONLY_SPAWN_ONCE));
            otherBits->SetInitialSpd(ESPInterval(10.0f, 15.0f));
            otherBits->SetParticleLife(ESPInterval(2.0f, 2.5f));
            otherBits->SetEmitDirection(Vector3D(0, 1, 0));
            otherBits->SetEmitAngleInDegrees(180);
            otherBits->SetToggleEmitOnPlane(true);
            otherBits->SetParticleSize(ESPInterval(info.GetSize1D() / 10.0f, info.GetSize1D() / 5.0f));
            otherBits->SetParticleRotation(ESPInterval(-180.0f, 180.0f));
            otherBits->SetRadiusDeviationFromCenter(ESPInterval(0.0f, info.GetSize1D() / 4.0f));
            otherBits->SetEmitPosition(emitCenter);
            otherBits->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
            otherBits->SetParticleColour(
                ESPInterval(iceColour.R(), 1.0f), ESPInterval(iceColour.G(), 1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
            otherBits->AddEffector(&this->particleFader);
            otherBits->SetParticles(15, 
                PersistentTextureManager::GetInstance()->GetLoadedTexture(
                GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT));

            // Giant snow flake with onomatopoeia
            ESPPointEmitter* snowflakeBackingEffect = new ESPPointEmitter();
            snowflakeBackingEffect->SetSpawnDelta(ESPInterval(ESPPointEmitter::ONLY_SPAWN_ONCE));
            snowflakeBackingEffect->SetInitialSpd(ESPInterval(0.0f));
            snowflakeBackingEffect->SetParticleLife(ESPInterval(0.8f*info.GetTimeInSecs()));
            snowflakeBackingEffect->SetParticleAlignment(ESP::ScreenPlaneAligned);
            snowflakeBackingEffect->SetEmitPosition(emitCenter);
            snowflakeBackingEffect->SetParticleColour(ESPInterval(0.95f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
            snowflakeBackingEffect->SetParticleRotation(ESPInterval(-180, 180));
            snowflakeBackingEffect->SetParticleSize(ESPInterval(1.5*info.GetSize1D()));
            snowflakeBackingEffect->AddEffector(&this->particleFader);
            snowflakeBackingEffect->AddEffector(&this->particleMediumGrowth);
            snowflakeBackingEffect->SetRandomTextureParticles(1, snowflakeTextures);

            // Create an emitter for the sound of onomatopoeia of shattering block
            ESPPointEmitter* iceSmashOnoEffect = new ESPPointEmitter();
            // Set up the emitter...
            iceSmashOnoEffect->SetSpawnDelta(ESPInterval(ESPPointEmitter::ONLY_SPAWN_ONCE));
            iceSmashOnoEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
            iceSmashOnoEffect->SetParticleLife(ESPInterval(info.GetTimeInSecs()));
            iceSmashOnoEffect->SetParticleSize(ESPInterval(1.0f, 1.0f), ESPInterval(1.0f, 1.0f));
            iceSmashOnoEffect->SetParticleRotation(ESPInterval(-20.0f, 20.0f));
            iceSmashOnoEffect->SetParticleAlignment(ESP::ScreenPlaneAligned);
            iceSmashOnoEffect->SetEmitPosition(emitCenter);
            iceSmashOnoEffect->SetParticleColour(GameModelConstants::GetInstance()->ICE_BALL_COLOUR);
            iceSmashOnoEffect->AddEffector(&this->particleFader);
            iceSmashOnoEffect->AddEffector(&this->particleSmallGrowth);

            // Set the onomatopoeia particle
            TextLabel2D smashTextLabel(GameFontAssetsManager::GetInstance()->GetFont(
                GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Big), "");
            smashTextLabel.SetColour(iceColour);
            smashTextLabel.SetDropShadow(Colour(0, 0, 0), 0.15f);
            iceSmashOnoEffect->SetParticles(1, smashTextLabel, Onomatoplex::SHATTER, 
                Onomatoplex::Generator::GetInstance()->GetRandomExtremeness(Onomatoplex::AWESOME, Onomatoplex::UBER));

            BasicEmitterUpdateStrategy* strategy = new BasicEmitterUpdateStrategy();
            strategy->AddEmitter(smashBitsEffect);
            strategy->AddEmitter(otherBits);
            strategy->AddEmitter(snowflakeBitsEffect);
            strategy->AddEmitter(snowflakeBackingEffect);
            this->fgEffects.push_back(strategy);
            this->laterPassEffects.push_back(new BasicEmitterUpdateStrategy(iceSmashOnoEffect));

            break;
        }

        case EnumBossEffectInfo::GothicRomanticBossSummon: {

            Collision::AABB2D bossAABB = info.GetBodyPart()->GenerateWorldAABB();

            ESPPointEmitter* lineEffect = GameESPAssets::CreateContinuousEmphasisLineEffect(
                Point3D(0,0,0), info.GetTimeInSecs(), 0.4f*info.GetSize1D(), 5.0f*info.GetSize1D(), true);
            assert(lineEffect != NULL);
            lineEffect->SetParticleColour(ESPInterval(0.8f,1.0f), ESPInterval(0), ESPInterval(0), ESPInterval(1.0f));
            
            ESPPointEmitter* halo = new ESPPointEmitter();
            halo->SetSpawnDelta(ESPInterval(0.25f));
            halo->SetNumParticleLives(1);
            halo->SetParticleLife(ESPInterval(info.GetTimeInSecs()));
            halo->SetParticleSize(ESPInterval(1.5f*bossAABB.GetHeight()));
            halo->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
            halo->SetParticleAlignment(ESP::ScreenPlaneAligned);
            halo->SetIsReversed(true);
            halo->SetParticleColour(ESPInterval(0.5f), ESPInterval(0.0f), ESPInterval(0.0f), ESPInterval(1.0f));
            halo->AddEffector(&this->particleShrinkToNothing);
            halo->AddEffector(&this->particleFader);
            halo->SetParticles(1, PersistentTextureManager::GetInstance()->GetLoadedTexture(
                GameViewConstants::GetInstance()->TEXTURE_HALO));

            ESPPointEmitter* chargeParticles1 = new ESPPointEmitter();
            chargeParticles1->SetSpawnDelta(ESPInterval(0.025f, 0.05f));
            chargeParticles1->SetNumParticleLives(1);
            chargeParticles1->SetInitialSpd(ESPInterval(2.25f, 6.0f));
            chargeParticles1->SetParticleLife(ESPInterval(info.GetTimeInSecs() * 0.4f, info.GetTimeInSecs()*0.8f));
            chargeParticles1->SetCutoffLifetime(info.GetTimeInSecs());
            chargeParticles1->SetParticleSize(ESPInterval(2.0f, 3.5f));
            chargeParticles1->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
            chargeParticles1->SetParticleAlignment(ESP::ScreenPlaneAligned);
            chargeParticles1->SetEmitDirection(Vector3D(0, 1, 0));
            chargeParticles1->SetEmitAngleInDegrees(180);
            chargeParticles1->SetIsReversed(true);
            chargeParticles1->SetParticleColour(ESPInterval(1), ESPInterval(1), ESPInterval(1), ESPInterval(1));
            chargeParticles1->AddEffector(&this->particleFader);
            chargeParticles1->AddEffector(&this->particleMediumShrink);
            chargeParticles1->AddCopiedEffector(ESPParticleRotateEffector(180.0f, ESPParticleRotateEffector::CLOCKWISE));
            chargeParticles1->SetParticles(20, PersistentTextureManager::GetInstance()->GetLoadedTexture(
                GameViewConstants::GetInstance()->TEXTURE_LENSFLARE));

            BossBodyPartEmitterUpdateStrategy* strategy = new BossBodyPartEmitterUpdateStrategy(info.GetBodyPart());
            strategy->AddEmitter(halo);
            strategy->AddEmitter(lineEffect);
            strategy->AddEmitter(chargeParticles1);
            strategy->SetOffset(info.GetOffset());

            fgEffects.push_back(strategy);

            break;
        }

        case EnumBossEffectInfo::GothicRomanticBossDestroyCharge: {
            


            break;
        }

        case EnumBossEffectInfo::FuturismBossWarningFlare: {

            std::vector<ESPAbstractEmitter*> flareEmitters;
            flareEmitters.reserve(1);
            ESPPointEmitter* briefLensFlare = new ESPPointEmitter();
            briefLensFlare->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
            briefLensFlare->SetNumParticleLives(1);
            briefLensFlare->SetParticleLife(ESPInterval(info.GetTimeInSecs()));
            briefLensFlare->SetParticleSize(ESPInterval(info.GetSize1D()));
            briefLensFlare->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
            briefLensFlare->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
            briefLensFlare->SetEmitPosition(Point3D(0,0,0));
            briefLensFlare->SetParticleColour(Colour(1,1,1));
            briefLensFlare->AddCopiedEffector(ESPParticleScaleEffector(1.0f, 5.0f));
            briefLensFlare->AddCopiedEffector(ESPMultiAlphaEffector(0.0f, 0.0, 1.0f, 0.05, 1.0f, 0.9, 0.0f, 1.0));
            briefLensFlare->SetParticles(1, PersistentTextureManager::GetInstance()->GetLoadedTexture(
                GameViewConstants::GetInstance()->TEXTURE_LENSFLARE));

            ESPPointEmitter* briefSparkle = new ESPPointEmitter();
            briefSparkle->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
            briefSparkle->SetNumParticleLives(1);
            briefSparkle->SetParticleLife(ESPInterval(info.GetTimeInSecs()));
            briefSparkle->SetParticleSize(ESPInterval(info.GetSize1D()));
            briefSparkle->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
            briefSparkle->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
            briefSparkle->SetEmitPosition(Point3D(0,0,0));
            briefSparkle->SetParticleColour(Colour(1,1,1));
            briefSparkle->AddCopiedEffector(ESPParticleScaleEffector(1.0f, 5.0f));
            briefSparkle->AddCopiedEffector(ESPParticleRotateEffector(
                Randomizer::GetInstance()->RandomUnsignedInt() % 360, 1, ESPParticleRotateEffector::CLOCKWISE));
            briefSparkle->AddCopiedEffector(ESPMultiAlphaEffector(1.0f, 0.0, 1.0f, 0.8, 0.0f, 1.0));
            briefSparkle->SetParticles(1, PersistentTextureManager::GetInstance()->GetLoadedTexture(
                GameViewConstants::GetInstance()->TEXTURE_SPARKLE));

            std::list<ESPAbstractEmitter*> emitters; 
            emitters.push_back(briefSparkle);
            emitters.push_back(briefLensFlare);

            BossBodyPartEmitterUpdateStrategy* strategy = 
                new BossBodyPartEmitterUpdateStrategy(info.GetBodyPart(), emitters);
            strategy->SetOffset(info.GetOffset());

            this->laterPassEffects.push_back(strategy);

            break;
        }

        case EnumBossEffectInfo::FuturismBossBeamEnergy: {
            ESPPointEmitter* lineEffect = GameESPAssets::CreateContinuousEmphasisLineEffect(
                Point3D(0,0,0), info.GetTimeInSecs(), 0.5f*info.GetSize1D(), 10.0f*info.GetSize1D(), true);
            assert(lineEffect != NULL);

            BossBodyPartEmitterUpdateStrategy* strategy = 
                new BossBodyPartEmitterUpdateStrategy(info.GetBodyPart(), lineEffect);
            strategy->SetOffset(info.GetOffset());

            this->fgEffects.push_back(strategy);

            break;
        }

        case EnumBossEffectInfo::FuturismBossAttractorBeam: {
            
            ESPMultiAlphaEffector fadeEffector;
            std::vector<std::pair<float, double> > alphaAndPercentages;
            alphaAndPercentages.reserve(4);
            alphaAndPercentages.push_back(std::make_pair(0.0f, 0.0));
            alphaAndPercentages.push_back(std::make_pair(1.0f, 0.4));
            alphaAndPercentages.push_back(std::make_pair(1.0f, 0.9));
            alphaAndPercentages.push_back(std::make_pair(0.0f, 1.0));

            fadeEffector.SetAlphasWithPercentage(alphaAndPercentages);

            ESPPointToPointBeam* bossToBallBigBeam = new ESPPointToPointBeam();
            bossToBallBigBeam->SetColour(ColourRGBA(1.0f, 1.0f, 1.0f, 0.5f));
            bossToBallBigBeam->SetBeamLifetime(ESPInterval(info.GetTimeInSecs() + 3.0));
            bossToBallBigBeam->SetNumBeamShots(1);
            bossToBallBigBeam->SetMainBeamThickness(ESPInterval(1.25f*info.GetSize1D()));
            bossToBallBigBeam->SetNumMainESPBeamSegments(1);
            bossToBallBigBeam->SetEnableDepth(false);
            bossToBallBigBeam->AddCopiedEffector(fadeEffector);

            ESPPointEmitter* lineEffect = GameESPAssets::CreateContinuousEmphasisLineEffect(
                Point3D(0,0,0), info.GetTimeInSecs(), 0.5f*info.GetSize1D(), 5.0f*info.GetSize1D(), true);
            assert(lineEffect != NULL);
            lineEffect->SetParticleColour(ESPInterval(0,0.1), ESPInterval(0), ESPInterval(0), ESPInterval(1.0f));

            static const float NUM_LIVES = 5;

            ESPPointEmitter* sparkleCone = new ESPPointEmitter();
            sparkleCone->SetSpawnDelta(ESPInterval(0.005f, 0.015f), true);
            sparkleCone->SetNumParticleLives(NUM_LIVES);
            sparkleCone->SetInitialSpd(ESPInterval(5.0f, 10.0f));
            sparkleCone->SetParticleLife(ESPInterval(0.8f, 1.2f));
            sparkleCone->SetParticleSize(ESPInterval(info.GetSize1D() * 0.05f, info.GetSize1D() * 0.45f));
            sparkleCone->SetEmitAngleInDegrees(25);
            sparkleCone->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
            sparkleCone->SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(0.8f, 1.0f));
            sparkleCone->SetEmitDirection(Vector3D(info.GetDirection(), 0));
            sparkleCone->SetParticleRotation(ESPInterval(0.0f, 359.99f));
            sparkleCone->SetIsReversed(true);
            sparkleCone->AddCopiedEffector(fadeEffector);
            sparkleCone->AddCopiedEffector(ESPParticleRotateEffector(90.0f, ESPParticleRotateEffector::CLOCKWISE));
            sparkleCone->SetParticles(40, PersistentTextureManager::GetInstance()->GetLoadedTexture(GameViewConstants::GetInstance()->TEXTURE_SPARKLE));

            ESPPointEmitter* chargeParticles1 = new ESPPointEmitter();
            chargeParticles1->SetSpawnDelta(ESPInterval(0.025f, 0.05f), true);
            chargeParticles1->SetNumParticleLives(NUM_LIVES);
            chargeParticles1->SetInitialSpd(ESPInterval(6.0f, 10.0f));
            chargeParticles1->SetParticleLife(ESPInterval(0.75f * info.GetTimeInSecs()/NUM_LIVES, info.GetTimeInSecs()/NUM_LIVES));
            chargeParticles1->SetParticleSize(ESPInterval(info.GetSize1D() * 0.1f, info.GetSize1D() * 0.5f));
            chargeParticles1->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
            chargeParticles1->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
            chargeParticles1->SetEmitDirection(Vector3D(0, 1, 0));
            chargeParticles1->SetEmitAngleInDegrees(180);
            chargeParticles1->SetIsReversed(true);
            chargeParticles1->SetParticleColour(ESPInterval(0,0.1), ESPInterval(0), ESPInterval(0), ESPInterval(1.0f));
            chargeParticles1->AddCopiedEffector(fadeEffector);
            chargeParticles1->AddEffector(&this->particleMediumShrink);
            chargeParticles1->SetParticles(20, PersistentTextureManager::GetInstance()->GetLoadedTexture(
                GameViewConstants::GetInstance()->TEXTURE_CIRCLE));

            static const int NUM_CIRCLES = 10;
            static const float CIRCLE_ALPHA  = 0.1;
            const float CIRCLE_RADIUS = 5*info.GetSize1D();

            ESPMultiAlphaEffector circleFadeEffector;
            alphaAndPercentages[0] = std::make_pair(0.0f, 0.0);
            alphaAndPercentages[1] = std::make_pair(CIRCLE_ALPHA, 0.05);
            alphaAndPercentages[2] = std::make_pair(CIRCLE_ALPHA, 0.8);
            alphaAndPercentages[3] = std::make_pair(0.0f, 1.0);
            circleFadeEffector.SetAlphasWithPercentage(alphaAndPercentages);

            float deltaAndLife = info.GetTimeInSecs() / (NUM_CIRCLES+1);
            ESPPointEmitter* concentricCircleEffect = new ESPPointEmitter();
            concentricCircleEffect->SetSpawnDelta(ESPInterval(deltaAndLife), true);
            concentricCircleEffect->SetNumParticleLives(1);
            concentricCircleEffect->SetParticleLife(ESPInterval((NUM_CIRCLES-1)*deltaAndLife));
            concentricCircleEffect->SetParticleSize(ESPInterval(1));
            concentricCircleEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
            concentricCircleEffect->SetParticleAlignment(ESP::ScreenAligned);
            concentricCircleEffect->SetIsReversed(true);
            concentricCircleEffect->SetCutoffLifetime(info.GetTimeInSecs());
            concentricCircleEffect->SetParticleColour(ESPInterval(0.1), ESPInterval(0.1), ESPInterval(0.1), ESPInterval(0));
            concentricCircleEffect->AddCopiedEffector(ESPParticleScaleEffector(2*CIRCLE_RADIUS, 0.01));
            concentricCircleEffect->AddCopiedEffector(circleFadeEffector);
            concentricCircleEffect->SetParticles(NUM_CIRCLES, PersistentTextureManager::GetInstance()->GetLoadedTexture(
                GameViewConstants::GetInstance()->TEXTURE_HOOP));

            // The flare emitter is at the center of the attractor beam...
            // Set up the pulse effector for the flare emitter
            ScaleEffect flarePulseSettings;
            flarePulseSettings.pulseGrowthScale = 1.25f;
            flarePulseSettings.pulseRate        = 2.5f;
            ESPParticleScaleEffector flarePulse(flarePulseSettings);

            ESPPointEmitter* emitFlare = new ESPPointEmitter();
            emitFlare->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
            emitFlare->SetInitialSpd(ESPInterval(0));
            emitFlare->SetParticleLife(ESPInterval(info.GetTimeInSecs()));
            emitFlare->SetEmitAngleInDegrees(0);
            emitFlare->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
            emitFlare->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
            emitFlare->SetParticleSize(ESPInterval(2.5*info.GetSize1D()));
            emitFlare->SetParticleRotation(ESPInterval(0, 359.99f));
            emitFlare->SetParticleColour(Colour(1,1,1));
            emitFlare->AddCopiedEffector(flarePulse);
            emitFlare->AddCopiedEffector(fadeEffector);
            emitFlare->SetParticles(1, PersistentTextureManager::GetInstance()->GetLoadedTexture(GameViewConstants::GetInstance()->TEXTURE_BRIGHT_FLARE));

            ESPPointEmitter* emitLensFlare = new ESPPointEmitter();
            emitLensFlare->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
            emitLensFlare->SetInitialSpd(ESPInterval(0));
            emitLensFlare->SetParticleLife(ESPInterval(info.GetTimeInSecs()));
            emitLensFlare->SetEmitAngleInDegrees(0);
            emitLensFlare->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
            emitLensFlare->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
            emitLensFlare->SetParticleSize(ESPInterval(3.5*info.GetSize1D()));
            emitLensFlare->SetParticleColour(Colour(1,1,1));
            emitLensFlare->AddCopiedEffector(fadeEffector);
            emitLensFlare->SetParticles(1, PersistentTextureManager::GetInstance()->GetLoadedTexture(GameViewConstants::GetInstance()->TEXTURE_LENSFLARE));

            // Ball aura
            ESPPointEmitter* auraEmitter = new ESPPointEmitter();
            auraEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
            auraEmitter->SetInitialSpd(ESPInterval(0));
            auraEmitter->SetParticleLife(ESPInterval(info.GetTimeInSecs()));
            auraEmitter->SetParticleSize(ESPInterval(3.0f*GameBall::DEFAULT_BALL_RADIUS));
            auraEmitter->SetEmitAngleInDegrees(0);
            auraEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
            auraEmitter->SetParticleAlignment(ESP::ScreenAligned);
            auraEmitter->SetEmitPosition(Point3D(0,0,0));
            auraEmitter->SetParticleColour(Colour(1,1,1));
            auraEmitter->AddCopiedEffector(ESPParticleScaleEffector(ScaleEffect(flarePulseSettings.pulseRate, 2.0)));
            auraEmitter->AddCopiedEffector(fadeEffector);
            auraEmitter->SetParticles(1, PersistentTextureManager::GetInstance()->GetLoadedTexture(GameViewConstants::GetInstance()->TEXTURE_CLEAN_CIRCLE_GRADIENT));

            BossBallBeamUpdateStrategy* strategyFG = new BossBallBeamUpdateStrategy(info.GetBodyPart());
            strategyFG->AddPointEmitter(chargeParticles1);
            strategyFG->AddPointEmitter(sparkleCone);
            strategyFG->AddPointEmitter(lineEffect);
            this->fgEffects.push_back(strategyFG);

            BossBallBeamUpdateStrategy* strategyBG = new BossBallBeamUpdateStrategy(info.GetBodyPart());
            strategyBG->AddPointEmitter(concentricCircleEffect);
            strategyBG->AddBallEmitter(auraEmitter);
            this->bgEffects.push_back(strategyBG);

            BossBallBeamUpdateStrategy* strategyLaterPass = new BossBallBeamUpdateStrategy(info.GetBodyPart());
            strategyLaterPass->AddBeamEmitter(bossToBallBigBeam);
            strategyLaterPass->AddPointEmitter(emitFlare);
            strategyLaterPass->AddPointEmitter(emitLensFlare);
            this->laterPassEffects.push_back(strategyLaterPass);

            break;
        }

        default:
            assert(false);
            return;
    }
}

void BossMesh::DrawPreBodyEffects(double dT, const Camera& camera, const GameModel& gameModel) {
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
        EffectUpdateStrategy::UpdateStrategyCollection(dT, camera, gameModel, this->bgEffects);
    }
}

ESPPointEmitter* BossMesh::BuildFireEmitter(float width, float height, float sizeScaler, double minSpawnDelta) {
    float smallestDimension = std::min<float>(width, height);

    static const float MAX_LIFE = 1.5f;

	ESPPointEmitter* smokeEmitter = new ESPPointEmitter();
	smokeEmitter->SetSpawnDelta(ESPInterval(minSpawnDelta, minSpawnDelta + 0.05));
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
	
    int numParticles = static_cast<int>(MAX_LIFE / minSpawnDelta);
    assert(numParticles < 30);
    bool success = smokeEmitter->SetRandomTextureParticles(numParticles, this->smokeTextures);
	assert(success);
    UNUSED_VARIABLE(success);

    return smokeEmitter;
}
ESPPointEmitter* BossMesh::BuildSmokeEmitter(float width, float height, float sizeScaler, double minSpawnDelta) {
    float smallestDimension = std::min<float>(width, height);

    static const float MAX_LIFE = 1.75f;

	ESPPointEmitter* smokeEmitter = new ESPPointEmitter();
	smokeEmitter->SetSpawnDelta(ESPInterval(minSpawnDelta, minSpawnDelta + 0.05));
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
	
    int numParticles = static_cast<int>(MAX_LIFE / minSpawnDelta);
    assert(numParticles < 30);
    bool success = smokeEmitter->SetRandomTextureParticles(numParticles, this->smokeTextures);
	assert(success);
    UNUSED_VARIABLE(success);

    return smokeEmitter;
}


ESPPointEmitter* BossMesh::BuildExplodingEmitter(float volumeAmt, const AbstractBossBodyPart* bossBodyPart, float width, float height, float sizeScaler) {
    float largestDimension = std::max<float>(width, height);

    ESPInterval spawnDeltaInterval(0.2f);
    
    static const double FPS = 60.0;
    static const float MAX_LIFE = 4*16.0 / FPS;

	ESPPointEmitter* explosionEmitter = new ESPPointEmitter();
	explosionEmitter->SetSpawnDelta(spawnDeltaInterval);
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
	
    int numParticles = static_cast<int>(MAX_LIFE / spawnDeltaInterval.maxValue);
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
    emitFlare->SetParticleAlignment(ESP::ScreenAligned);
    emitFlare->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    emitFlare->SetParticleSize(ESPInterval(3.25*size));
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
    emitLensFlare->SetParticleSize(ESPInterval(4.5*size));
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
