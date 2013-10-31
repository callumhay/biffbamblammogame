/**
 * GothicRomanticBossMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "GothicRomanticBossMesh.h"
#include "GameViewConstants.h"
#include "GameAssets.h"
#include "GameLightAssets.h"
#include "InGameBossLevelDisplayState.h"

#include "../ResourceManager.h"
#include "../BlammoEngine/Mesh.h"
#include "../ESPEngine/ESP.h"
#include "../GameModel/GothicRomanticBoss.h"
#include "../GameModel/BossWeakpoint.h"

const double GothicRomanticBossMesh::INTRO_TIME_IN_SECS = 4.0;

GothicRomanticBossMesh::GothicRomanticBossMesh(GothicRomanticBoss* boss, GameSound* sound) :
BossMesh(sound), boss(boss), bodyMesh(NULL), topPointMesh(NULL), bottomPointMesh(NULL), legMesh(NULL),
topPointSmokeEmitter(NULL), topPointFireEmitter(NULL), topPointExplodingEmitter(NULL), 
circleGlowTex(NULL), bottomGlowSoundID(INVALID_SOUND_ID) {
    assert(boss != NULL);

    // Load the mesh assets...
    this->bodyMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->GOTHIC_ROMANTIC_BOSS_BODY_MESH);
    assert(this->bodyMesh != NULL);
    this->topPointMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->GOTHIC_ROMANTIC_BOSS_TOP_POINT_MESH);
    assert(this->topPointMesh != NULL);
    this->bottomPointMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->GOTHIC_ROMANTIC_BOSS_BOTTOM_POINT_MESH);
    assert(this->bottomPointMesh != NULL);
    this->legMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->GOTHIC_ROMANTIC_BOSS_LEG_MESH);
    assert(this->legMesh != NULL);

    this->topPointSmokeEmitter  = this->BuildSmokeEmitter(GothicRomanticBoss::TOP_POINT_WIDTH, GothicRomanticBoss::TOP_POINT_HEIGHT);
    this->topPointFireEmitter   = this->BuildFireEmitter(GothicRomanticBoss::TOP_POINT_WIDTH, GothicRomanticBoss::TOP_POINT_HEIGHT);
    this->topPointExplodingEmitter = this->BuildExplodingEmitter(0.5f, this->boss->GetTopPoint(), 
        GothicRomanticBoss::TOP_POINT_WIDTH, GothicRomanticBoss::TOP_POINT_HEIGHT, 1.2f);

    this->bottomPointSmokeEmitter = this->BuildSmokeEmitter(GothicRomanticBoss::BOTTOM_POINT_WIDTH, GothicRomanticBoss::BOTTOM_POINT_HEIGHT);
    this->bottomPointFireEmitter  = this->BuildFireEmitter(GothicRomanticBoss::BOTTOM_POINT_WIDTH, GothicRomanticBoss::BOTTOM_POINT_HEIGHT);
    this->bottomPointExplodingEmitter = this->BuildExplodingEmitter(0.75f, this->boss->GetBottomPoint(), 
        GothicRomanticBoss::BOTTOM_POINT_WIDTH, GothicRomanticBoss::BOTTOM_POINT_HEIGHT, 1.3f);

    this->bodySmokeEmitter = this->BuildSmokeEmitter(GothicRomanticBoss::BODY_WIDTH/1.5f, GothicRomanticBoss::BODY_HEIGHT/2.0f);
    this->bodyFireEmitter  = this->BuildFireEmitter(GothicRomanticBoss::BODY_WIDTH/1.5f, GothicRomanticBoss::BODY_HEIGHT/2.0f);
    this->bodyExplodingEmitter = this->BuildExplodingEmitter(1.0f, this->boss->GetBody(), GothicRomanticBoss::BODY_WIDTH, GothicRomanticBoss::BODY_HEIGHT);

    this->circleGlowTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CLEAN_CIRCLE_GRADIENT, Texture::Trilinear));
    assert(this->circleGlowTex != NULL);

    {
        std::vector<double> timeVals;
        timeVals.reserve(3);
        timeVals.push_back(0.0);
        timeVals.push_back(1.0);
        timeVals.push_back(2.0);

        std::vector<float> pulseVals;
        pulseVals.reserve(timeVals.size());
        pulseVals.push_back(1.0f);
        pulseVals.push_back(1.33f);
        pulseVals.push_back(1.0f);

        this->glowCirclePulseAnim.SetLerp(timeVals, pulseVals);
        this->glowCirclePulseAnim.SetRepeat(true);
    }

    this->bottomPtGlowAlphaAnim.ClearLerp();
    this->bottomPtGlowAlphaAnim.SetInterpolantValue(0.0f);
    this->bottomPtGlowAlphaAnim.SetRepeat(false);

    this->legGlowAlphaAnims.resize(GothicRomanticBoss::NUM_LEGS);
    this->legGlowSoundIDs.resize(GothicRomanticBoss::NUM_LEGS, INVALID_SOUND_ID);
    for (int i = 0; i < static_cast<int>(this->legGlowAlphaAnims.size()); i++) {
        
        this->legGlowAlphaAnims[i].ClearLerp();
        this->legGlowAlphaAnims[i].SetInterpolantValue(0.0f);
        this->legGlowAlphaAnims[i].SetRepeat(false);
    }
}

GothicRomanticBossMesh::~GothicRomanticBossMesh() {
    // The boss pointer doesn't belong to this, just referenced
    this->boss = NULL;
    
    bool success = false;
   
    // Release the mesh assets...
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->bodyMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->topPointMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->bottomPointMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->legMesh);
    assert(success);

    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->circleGlowTex);
    assert(success);

    UNUSED_VARIABLE(success);

    delete this->topPointSmokeEmitter;
    this->topPointSmokeEmitter = NULL;
    delete this->topPointFireEmitter;
    this->topPointFireEmitter = NULL;
    delete this->topPointExplodingEmitter;
    this->topPointExplodingEmitter = NULL;
    delete this->bottomPointExplodingEmitter;
    this->bottomPointExplodingEmitter = NULL;
    delete this->bottomPointFireEmitter;
    this->bottomPointFireEmitter = NULL;
    delete this->bottomPointSmokeEmitter;
    this->bottomPointSmokeEmitter = NULL;
    delete this->bodySmokeEmitter;
    this->bodySmokeEmitter = NULL;
    delete this->bodyFireEmitter;
    this->bodyFireEmitter = NULL;
    delete this->bodyExplodingEmitter;
    this->bodyExplodingEmitter = NULL;
}

double GothicRomanticBossMesh::ActivateIntroAnimation() {
    static const float MAX_GLOW_ALPHA = 0.75f;

    double nextStartTime = INTRO_TIME_IN_SECS/3.0;
    this->bottomPtGlowAlphaAnim.SetLerp(0.0, nextStartTime, 0.0f, MAX_GLOW_ALPHA);
    this->bottomPtGlowAlphaAnim.SetInterpolantValue(0.0f);
    this->bottomPtGlowAlphaAnim.SetRepeat(false);
    nextStartTime += 0.5;

    const double TIME_PER_LEG_GLOW = 
        (INTRO_TIME_IN_SECS - nextStartTime - InGameBossLevelDisplayState::TIME_OF_UNPAUSE_BEFORE_INTRO_END) / 
        static_cast<double>(GothicRomanticBoss::NUM_LEGS);

    for (int i = 0; i < static_cast<int>(this->legGlowAlphaAnims.size()); i++) {
        this->legGlowAlphaAnims[i].SetLerp(nextStartTime, nextStartTime + TIME_PER_LEG_GLOW, 0.0f, MAX_GLOW_ALPHA);
        this->legGlowAlphaAnims[i].SetInterpolantValue(0.0f);
        this->legGlowAlphaAnims[i].SetRepeat(false);
        nextStartTime += TIME_PER_LEG_GLOW;
    }

    this->introTimeCountdown = INTRO_TIME_IN_SECS;
    return GothicRomanticBossMesh::INTRO_TIME_IN_SECS;
}

void GothicRomanticBossMesh::DrawBody(double dT, const Camera& camera, const BasicPointLight& keyLight,
                                      const BasicPointLight& fillLight, const BasicPointLight& ballLight,
                                      const GameAssets* assets) {
 
    UNUSED_PARAMETER(dT);
    UNUSED_PARAMETER(assets);

    // Using data from the GameModel's boss object, we draw the various pieces of the boss in their correct
    // world space locations...

    ColourRGBA currColour;

    // Body
    const BossBodyPart* body = this->boss->GetBody();
    assert(body != NULL);

    currColour = body->GetColour();
    if (currColour.A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(body->GetWorldTransform().begin());
        glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
        this->bodyMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    // Top Point
    const BossBodyPart* topPoint = this->boss->GetTopPoint();
    assert(topPoint != NULL);

    currColour = topPoint->GetColour();
    if (currColour.A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(topPoint->GetWorldTransform().begin());
        glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
        this->topPointMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    // Bottom Point
    const BossBodyPart* bottomPoint = this->boss->GetBottomPoint();
    assert(bottomPoint != NULL);

    currColour = bottomPoint->GetColour();
    if (currColour.A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(bottomPoint->GetWorldTransform().begin());
        glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
        this->bottomPointMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    // Legs
    for (int i = 0; i < GothicRomanticBoss::NUM_LEGS; i++) {
        const BossBodyPart* leg = this->boss->GetLeg(i);
        assert(leg != NULL);

        currColour = leg->GetColour();
        if (currColour.A() > 0.0f) {
            glPushMatrix();
            glMultMatrixf(leg->GetWorldTransform().begin());
            glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
            this->legMesh->Draw(camera, keyLight, fillLight, ballLight);
            glPopMatrix();
        }
    }

}

void GothicRomanticBossMesh::DrawPostBodyEffects(double dT, const Camera& camera, const GameAssets* assets) {
    BossMesh::DrawPostBodyEffects(dT, camera, assets);

    this->glowCirclePulseAnim.Tick(dT);
    float pulseScaler = this->glowCirclePulseAnim.GetInterpolantValue();

    // Check to see if we're drawing intro effects
    if (this->introTimeCountdown > 0.0) {

        if (this->bottomGlowSoundID == INVALID_SOUND_ID && this->bottomPtGlowAlphaAnim.GetInterpolantValue() > 0.0f) {
            this->bottomGlowSoundID = assets->GetSound()->PlaySound(GameSound::BossGlowEvent, false, false);
        }

        this->bottomPtGlowAlphaAnim.Tick(dT);
        for (int i = 0; i < static_cast<int>(this->legGlowAlphaAnims.size()); i++) {
            this->legGlowAlphaAnims[i].Tick(dT);
            if (this->legGlowSoundIDs[i] == INVALID_SOUND_ID && this->legGlowAlphaAnims[i].GetInterpolantValue() > 0.0f) {
                this->legGlowSoundIDs[i] = assets->GetSound()->PlaySound(GameSound::BossGlowEvent, false, false);
            }
        }

        this->introTimeCountdown -= dT;
    }

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    const BossBodyPart* topPoint = this->boss->GetTopPoint();
    if (topPoint->GetAlpha() > 0.0f) {

        if (topPoint->GetType() == AbstractBossBodyPart::WeakpointBodyPart) {
            const BossWeakpoint* topPointWeakPt = static_cast<const BossWeakpoint*>(topPoint);
            const Point3D topPointPos = topPoint->GetTranslationPt3D();
            
            if (!topPointWeakPt->GetIsDestroyed()) {
                float lifePercentage = topPointWeakPt->GetCurrentLifePercentage();
                
                if (lifePercentage < 1.0f) {
                    glPushMatrix();
                    glTranslatef(topPointPos[0], topPointPos[1], topPointPos[2]);

                    this->topPointSmokeEmitter->SetSpawnDelta(ESPInterval(lifePercentage*0.4f, lifePercentage*0.9f));
                    this->topPointSmokeEmitter->Tick(dT);
                    this->topPointSmokeEmitter->Draw(camera);

                    if (lifePercentage <= 0.5f) {
                        this->topPointFireEmitter->SetSpawnDelta(ESPInterval(lifePercentage*0.4f, lifePercentage*0.9f));
                        this->topPointFireEmitter->Tick(dT);
                        this->topPointFireEmitter->Draw(camera);
                    }

                    glPopMatrix();
                }
            }
            else {
                glPushMatrix();
                glTranslatef(topPointPos[0], topPointPos[1], topPointPos[2]);
                this->topPointExplodingEmitter->Tick(dT);
                this->topPointExplodingEmitter->Draw(camera);
                glPopMatrix();
            }
        }
    }

    // Bottom point effects
    // NOTE: Be sure to do this before the leg effects, so that the leg effects include the effects
    // of the bottom point in their alpha blending
    const BossBodyPart* bottomPoint = this->boss->GetBottomPoint();
    if (bottomPoint->GetAlpha() > 0.0f) {

        if (bottomPoint->GetType() == AbstractBossBodyPart::WeakpointBodyPart) {
            const BossWeakpoint* bottomPointWeakPt = static_cast<const BossWeakpoint*>(bottomPoint);
            const Point3D bottomPointPos = bottomPoint->GetTranslationPt3D();

            if (!bottomPointWeakPt->GetIsDestroyed()) {
                float lifePercentage = bottomPointWeakPt->GetCurrentLifePercentage();
                if (lifePercentage < 1.0) {
                    glPushMatrix();
                    glTranslatef(bottomPointPos[0], bottomPointPos[1], bottomPointPos[2]);
                    
                    this->bottomPointSmokeEmitter->SetSpawnDelta(ESPInterval(lifePercentage*0.4f, lifePercentage*0.9f));
                    this->bottomPointSmokeEmitter->Tick(dT);
                    this->bottomPointSmokeEmitter->Draw(camera);

                    if (lifePercentage <= 0.25) {
                        this->bottomPointFireEmitter->SetSpawnDelta(ESPInterval(0.1f, 0.23f));
                        this->bottomPointFireEmitter->Tick(dT);
                        this->bottomPointFireEmitter->Draw(camera);
                    }

                    glPopMatrix();
                }
            }
            else {
                glPushMatrix();
                glTranslatef(bottomPointPos[0], bottomPointPos[1], bottomPointPos[2]);
                this->bottomPointExplodingEmitter->Tick(dT);
                this->bottomPointExplodingEmitter->Draw(camera);
                glPopMatrix();
            }
        }

        glColor4f(1.0f, 0.1f, 0.1f, this->bottomPtGlowAlphaAnim.GetInterpolantValue());
        this->circleGlowTex->BindTexture();
        
        Point3D pointPos = this->boss->GetBottomPointTipPos();
        glPushMatrix();
        glTranslatef(pointPos[0], pointPos[1], pointPos[2]);
        glScalef(pulseScaler, pulseScaler, 1.0f);
        GeometryMaker::GetInstance()->DrawQuad();
        glPopMatrix();
        this->circleGlowTex->UnbindTexture();
    }

    // Body effects
    const BossBodyPart* body = this->boss->GetBody();
    if (body->GetAlpha() > 0.0f) {
        
        if (body->GetType() == AbstractBossBodyPart::WeakpointBodyPart) {
            Point3D bodyPos = body->GetTranslationPt3D();

            if (!body->GetIsDestroyed()) {
                const BossWeakpoint* bodyWeakpt = static_cast<const BossWeakpoint*>(body);
                float lifePercentage = bodyWeakpt->GetCurrentLifePercentage();
                
                if (lifePercentage < 1.0) {
                    
                    glPushMatrix();
                    glTranslatef(bodyPos[0], bodyPos[1], bodyPos[2]);

                    // Draw smoke
                    this->bodySmokeEmitter->Tick(dT);
                    this->bodySmokeEmitter->Draw(camera);

                    if (lifePercentage <= 0.5) {
                        // Draw fire as well
                        this->bodyFireEmitter->Tick(dT);
                        this->bodyFireEmitter->Draw(camera);
                    }

                    glPopMatrix();
                }
            }
            else {
                glPushMatrix();
                glTranslatef(bodyPos[0], bodyPos[1], bodyPos[2]);
                this->bodyExplodingEmitter->SetParticleAlpha(body->GetAlpha());
                this->bodyExplodingEmitter->Tick(dT);
                this->bodyExplodingEmitter->Draw(camera);
                glPopMatrix();
            }
        }
    }


    // Leg effects
    const BossBodyPart* leg = this->boss->GetLeg(0);
    if (leg->GetAlpha() > 0.0f) {

        if (!this->boss->IsBodyPartDead(leg)) {
            this->circleGlowTex->BindTexture();
            for (int i = 0; i < GothicRomanticBoss::NUM_LEGS; i++) {

                glColor4f(1.0f, 0.1f, 0.1f, leg->GetAlpha()*this->legGlowAlphaAnims[i].GetInterpolantValue());

                Point3D legPos = this->boss->GetLegPointPos(i);

                glPushMatrix();
                glTranslatef(legPos[0], legPos[1], legPos[2]);
                glScalef(pulseScaler*0.75f, pulseScaler*0.75f, 1.0f);
                GeometryMaker::GetInstance()->DrawQuad();
                glPopMatrix();
            }
            this->circleGlowTex->UnbindTexture();
        }
    }

    glPopAttrib();
}

Point3D GothicRomanticBossMesh::GetBossFinalExplodingEpicenter() const {
    Point3D center(this->boss->GetBody()->GetTranslationPt2D(), GothicRomanticBoss::HALF_BODY_WIDTH);
    return center;
}