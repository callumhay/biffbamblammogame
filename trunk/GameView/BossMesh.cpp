/**
 * BossMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "BossMesh.h"
#include "ClassicalBossMesh.h"
#include "GothicRomanticBossMesh.h"
#include "NouveauBossMesh.h"
#include "DecoBossMesh.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Texture2D.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/GameLevel.h"
#include "../GameModel/ClassicalBoss.h"
#include "../GameModel/GothicRomanticBoss.h"
#include "../GameModel/NouveauBoss.h"
#include "../GameModel/DecoBoss.h"
#include "../GameModel/LaserBeamSightsEffectInfo.h"
#include "../GameModel/PowerChargeEffectInfo.h"
#include "../GameModel/ExpandingHaloEffectInfo.h"
#include "../GameModel/SparkBurstEffectInfo.h"
#include "../GameModel/ElectricitySpasmEffectInfo.h"
#include "../GameModel/ElectrifiedEffectInfo.h"

#include "../ResourceManager.h"

BossMesh::BossMesh() : explosionAnimTex(NULL), finalExplosionIsActive(false),
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
    // Initialize the smoke textures...
    if (this->smokeTextures.empty()) {
		this->smokeTextures.reserve(6);
		Texture2D* temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE1, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE2, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE3, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE4, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE5, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE6, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);	
	}

    this->explosionAnimTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_EXPLOSION_ANIMATION, Texture::Trilinear));
    assert(this->explosionAnimTex != NULL);

    this->squareTargetTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_SQUARE_TARGET, Texture::Trilinear));
    assert(this->squareTargetTex != NULL);
    
    this->haloTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_HALO, Texture::Trilinear));
    assert(this->haloTex != NULL);

    this->sparkleTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_SPARKLE, Texture::Trilinear));
    assert(this->sparkleTex != NULL);

    this->lightningAnimTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_LIGHTNING_ANIMATION, Texture::Trilinear));
    assert(this->lightningAnimTex != NULL);

    this->lineAnim.ClearLerp();
    this->flashAnim.ClearLerp();
}

BossMesh::~BossMesh() {
    // Release the smoke textures
    bool success = false;

    for (std::vector<Texture2D*>::iterator iter = this->smokeTextures.begin();
		iter != this->smokeTextures.end(); ++iter) {
		
		success = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
		assert(success);	
	}
	this->smokeTextures.clear();

    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->explosionAnimTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->squareTargetTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->haloTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->sparkleTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->lightningAnimTex);
    assert(success);

    UNUSED_VARIABLE(success);

    // Clean up the effects emitters (if there are any)
    this->ClearActiveEffects();
}


BossMesh* BossMesh::Build(const GameWorld::WorldStyle& style, Boss* boss) {
    BossMesh* result = NULL;
    switch (style) {

        case GameWorld::Classical: {
            assert(dynamic_cast<ClassicalBoss*>(boss) != NULL);
            ClassicalBoss* classicalBoss = static_cast<ClassicalBoss*>(boss);
            result = new ClassicalBossMesh(classicalBoss);
            break;
        }

        case GameWorld::GothicRomantic: {
            assert(dynamic_cast<GothicRomanticBoss*>(boss) != NULL);
            GothicRomanticBoss* gothicBoss = static_cast<GothicRomanticBoss*>(boss);
            result = new GothicRomanticBossMesh(gothicBoss);
            break;
        }

        case GameWorld::Nouveau: {
            assert(dynamic_cast<NouveauBoss*>(boss) != NULL);
            NouveauBoss* nouveauBoss = static_cast<NouveauBoss*>(boss);
            result = new NouveauBossMesh(nouveauBoss);
            break;
        }

        case GameWorld::Deco: {
            assert(dynamic_cast<DecoBoss*>(boss) != NULL);
            DecoBoss* decoBoss = static_cast<DecoBoss*>(boss);
            result = new DecoBossMesh(decoBoss);
            break;
        }

        case GameWorld::Futurism:
            // TODO

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
    double lineAnimTime = (Boss::FADE_TO_BLACK_FINAL_DEAD_BODY_PART_TIME - delayInSecs) / 2.5 + delayInSecs;

    const GameLevel* level = model->GetCurrentLevel();
    assert(level != NULL);
    
    this->lineAnim.SetLerp(delayInSecs, lineAnimTime, 0.0f, 2*level->GetLevelUnitWidth());
    this->flashAnim.SetLerp(lineAnimTime, Boss::TOTAL_DEATH_ANIM_TIME, 0.0f, 2*level->GetLevelUnitHeight());

    this->finalExplosionIsActive = true;

    return lineAnimTime;
}

void BossMesh::ClearActiveEffects() {
    
    for (std::list<ESPEmitter*>::iterator iter = this->fgEffectsEmitters.begin();
        iter != this->fgEffectsEmitters.end(); ++iter) {
            delete *iter;
            *iter = NULL;
    }
    this->fgEffectsEmitters.clear();

    for (std::map<const BossBodyPart*, std::list<ESPEmitter*> >::iterator iter1 = this->fgAttachedEffectsEmitters.begin();
        iter1 != this->fgAttachedEffectsEmitters.end(); ++iter1) {

            std::list<ESPEmitter*>& currEmitters = iter1->second;
            for (std::list<ESPEmitter*>::iterator iter2 = currEmitters.begin(); iter2 != currEmitters.end(); ++iter2) {
                ESPEmitter* currEmitter = *iter2;
                delete currEmitter;
                currEmitter = NULL;
            }
    }
    this->fgAttachedEffectsEmitters.clear();

    for (std::list<ESPEmitter*>::iterator iter = this->bgEffectsEmitters.begin();
        iter != this->bgEffectsEmitters.end(); ++iter) {
            delete *iter;
            *iter = NULL;
    }
    this->bgEffectsEmitters.clear();
}

void BossMesh::AddLaserBeamSightsEffect(const LaserBeamSightsEffectInfo& info) {

    ESPPointEmitter* spinningTarget = new ESPPointEmitter();
    spinningTarget->SetSpawnDelta(ESPInterval(-1));
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
    spinningTarget->SetParticles(1, this->squareTargetTex);

    this->fgEffectsEmitters.push_back(spinningTarget);
}

void BossMesh::AddBossPowerChargeEffect(const PowerChargeEffectInfo& info) {

    const BossBodyPart* bodyPart = info.GetChargingPart();
    const Colour& baseColour = info.GetColour();
    const Vector2D& posOffset = info.GetPositionOffset();

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
    chargeParticles1->SetParticleAlignment(ESP::ScreenAligned);
    chargeParticles1->SetEmitPosition(Point3D(posOffset[0],posOffset[1],0));
    chargeParticles1->SetEmitDirection(Vector3D(0, 1, 0));
    chargeParticles1->SetEmitAngleInDegrees(180);
    chargeParticles1->SetIsReversed(true);
    chargeParticles1->SetParticleColourPalette(colours);
    chargeParticles1->AddEffector(&this->particleFader);
    chargeParticles1->AddEffector(&this->particleMediumShrink);
    chargeParticles1->SetParticles(20, this->sparkleTex);

    static const unsigned int NUM_HALOS = 3;
    ESPPointEmitter* halo = new ESPPointEmitter();
    halo->SetSpawnDelta(ESPInterval(0.25f));
    halo->SetNumParticleLives(1);
    halo->SetParticleLife(ESPInterval(info.GetChargeTimeInSecs()));
    halo->SetParticleSize(ESPInterval(5.0f));
    halo->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    halo->SetParticleAlignment(ESP::ScreenAligned);
    halo->SetEmitPosition(Point3D(posOffset[0],posOffset[1],0));
    halo->SetIsReversed(true);
    halo->SetParticleColour(ESPInterval(baseColour.R()), ESPInterval(baseColour.G()), 
        ESPInterval(baseColour.B()), ESPInterval(1.0f));
    halo->AddEffector(&this->particleShrinkToNothing);
    halo->SetParticles(NUM_HALOS, this->haloTex);

    this->fgAttachedEffectsEmitters[bodyPart].push_back(halo);
    this->fgAttachedEffectsEmitters[bodyPart].push_back(chargeParticles1);
}

void BossMesh::AddBossExpandingHaloEffect(const ExpandingHaloEffectInfo& info) {

    const BossBodyPart* bodyPart = info.GetPart();
    const Colour& colour = info.GetColour();

    Collision::AABB2D aabb = bodyPart->GetLocalBounds().GenerateAABBFromLines();
    float minSize = std::min<float>(aabb.GetHeight(), aabb.GetWidth());

    ESPPointEmitter* halo = new ESPPointEmitter();
    halo->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    halo->SetNumParticleLives(1);
    halo->SetParticleLife(ESPInterval(info.GetTimeInSecs()));
    halo->SetParticleSize(ESPInterval(info.GetSizeMultiplier() * minSize));
    halo->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    halo->SetParticleAlignment(ESP::ScreenAligned);
    halo->SetEmitPosition(Point3D(0,0,0));
    halo->SetParticleColour(ESPInterval(colour.R()), ESPInterval(colour.G()), ESPInterval(colour.B()), ESPInterval(0.8f));
    halo->AddEffector(&this->particleFader);
    halo->AddEffector(&this->particleSuperGrowth);
    halo->SetParticles(1, this->haloTex);

    this->fgAttachedEffectsEmitters[bodyPart].push_back(halo);
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
    sparkParticles1->SetParticles(8, this->sparkleTex);

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
    sparkParticles2->SetParticles(5, this->sparkleTex);

    if (bodyPart != NULL) {
        this->fgAttachedEffectsEmitters[bodyPart].push_back(sparkParticles1);
        this->fgAttachedEffectsEmitters[bodyPart].push_back(sparkParticles2);
    }
    else {
        this->fgEffectsEmitters.push_back(sparkParticles1);
        this->fgEffectsEmitters.push_back(sparkParticles2);
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
    electricSpasm->SetAnimatedParticles(NUM_PARTICLES, this->lightningAnimTex, 64, 64);

    this->fgAttachedEffectsEmitters[bodyPart].push_back(electricSpasm);
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
    electricSpasm->SetAnimatedParticles(20, this->lightningAnimTex, 64, 64);

    this->bgEffectsEmitters.push_back(electricSpasm);
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
                curr->Draw(camera);
                curr->Tick(dT);
                ++iter;
            }
        }
    }
}

void BossMesh::DrawPostBodyEffects(double dT, const Camera& camera) {
    // Go through all the active effects for the boss, draw each one and clean up dead effects
    for (std::list<ESPEmitter*>::iterator iter = this->fgEffectsEmitters.begin(); iter != this->fgEffectsEmitters.end();) {

        ESPEmitter* curr = *iter;
        assert(curr != NULL);

        // Check to see if dead, if so erase it...
        if (curr->IsDead()) {
            iter = this->fgEffectsEmitters.erase(iter);
            delete curr;
            curr = NULL;
        }
        else {
            // Not dead yet so we draw and tick
            curr->Draw(camera);
            curr->Tick(dT);
            ++iter;
        }
    }

    // Go through all the attached effects emitters and do book keeping and drawing
    for (std::map<const BossBodyPart*, std::list<ESPEmitter*> >::iterator iter1 = this->fgAttachedEffectsEmitters.begin();
         iter1 != this->fgAttachedEffectsEmitters.end();) {

        const BossBodyPart* bodyPart = iter1->first;
        Point3D bodyPartPos = bodyPart->GetTranslationPt3D();

        std::list<ESPEmitter*>& currEmitters = iter1->second;

        glPushMatrix();
        glTranslatef(bodyPartPos[0], bodyPartPos[1], bodyPartPos[2]);

        for (std::list<ESPEmitter*>::iterator iter2 = currEmitters.begin(); iter2 != currEmitters.end();) {
            ESPEmitter* currEmitter = *iter2;

            // Check to see if dead, if so erase it...
            if (currEmitter->IsDead()) {
                iter2 = currEmitters.erase(iter2);
                delete currEmitter;
                currEmitter = NULL;
            }
            else {

                // Not dead yet so we draw and tick - transform to the body part to draw
                currEmitter->Tick(dT);
                currEmitter->Draw(camera);
                ++iter2;
            }
        }

        glPopMatrix();

        if (currEmitters.empty()) {
            iter1 = this->fgAttachedEffectsEmitters.erase(iter1);
        }
        else {
            ++iter1;
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


ESPPointEmitter* BossMesh::BuildExplodingEmitter(float width, float height, float sizeScaler) {
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
	
    int numParticles = static_cast<int>(MAX_LIFE / MAX_SPAWN_DELTA);
    assert(numParticles < 30);
    bool success = explosionEmitter->SetAnimatedParticles(numParticles, this->explosionAnimTex, 64, 64);
	assert(success);
    UNUSED_VARIABLE(success);

    return explosionEmitter;
}