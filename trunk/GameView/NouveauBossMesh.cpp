/**
 * NouveauBossMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "NouveauBossMesh.h"
#include "GameViewConstants.h"
#include "CgFxPrism.h"
#include "InGameBossLevelDisplayState.h"
#include "GameAssets.h"

#include "../BlammoEngine/Mesh.h"
#include "../GameModel/NouveauBoss.h"
#include "../GameModel/BossWeakpoint.h"
#include "../ResourceManager.h"

const double NouveauBossMesh::INTRO_TIME_IN_SECS = 4.0;

NouveauBossMesh::NouveauBossMesh(NouveauBoss* boss) : BossMesh(), boss(boss),
bodyMesh(NULL), bottomCurlMesh(NULL), bottomHexSphereMesh(NULL),
sideCurlsMesh(NULL), sideSphereMesh(NULL), sideSphereFrillsMesh(NULL),
sideSphereHolderCurlMesh(NULL), topDomeMesh(NULL), topGazeboMesh(NULL),
topSphereMesh(NULL), circleGlowTex(NULL), 
leftArmExplodingEmitter(NULL), rightArmExplodingEmitter(NULL) {

    assert(boss != NULL);

    this->bodyMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_BODY_MESH);
    assert(this->bodyMesh != NULL);
    this->bottomCurlMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_BOTTOM_CURL_MESH);
    assert(this->bottomCurlMesh != NULL);
    this->bottomHexSphereMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_BOTTOM_HEX_SPHERE_MESH);
    assert(this->bottomHexSphereMesh != NULL);
    this->sideCurlsMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_SIDE_CURLS_MESH);
    assert(this->sideCurlsMesh != NULL);
    this->sideSphereMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_SIDE_SPHERE_MESH);
    assert(this->sideSphereMesh != NULL);
    this->sideSphereFrillsMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_SIDE_SPHERE_FRILLS_MESH);
    assert(this->sideSphereFrillsMesh != NULL);
    this->sideSphereHolderCurlMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_SIDE_SPHERE_HOLDER_CURL_MESH);
    assert(this->sideSphereHolderCurlMesh != NULL);
    this->topDomeMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_TOP_ENCLOSURE_DOME_MESH);
    assert(this->topDomeMesh != NULL);
    this->topGazeboMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_TOP_ENCLOSURE_GAZEBO_MESH);
    assert(this->topGazeboMesh != NULL);
    this->topSphereMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BOSS_TOP_SPHERE_MESH);
    assert(this->topSphereMesh != NULL);

    this->circleGlowTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CLEAN_CIRCLE_GRADIENT, Texture::Trilinear));
    assert(this->circleGlowTex != NULL);

    this->topGazeboSmokeEmitter     = this->BuildSmokeEmitter(NouveauBoss::TOP_ENCLOSURE_GAZEBO_WIDTH, NouveauBoss::TOP_ENCLOSURE_GAZEBO_HEIGHT);
    this->topGazeboFireEmitter      = this->BuildFireEmitter(NouveauBoss::TOP_ENCLOSURE_GAZEBO_WIDTH, NouveauBoss::TOP_ENCLOSURE_GAZEBO_HEIGHT);
    this->topGazeboExplodingEmitter = this->BuildExplodingEmitter(NouveauBoss::TOP_ENCLOSURE_GAZEBO_WIDTH, NouveauBoss::TOP_ENCLOSURE_GAZEBO_HEIGHT);
    this->topDomeExplodingEmitter   = this->BuildExplodingEmitter(NouveauBoss::TOP_ENCLOSURE_DOME_BOTTOM_WIDTH, NouveauBoss::TOP_ENCLOSURE_DOME_HEIGHT);

    this->bodySmokeEmitter     = this->BuildSmokeEmitter(NouveauBoss::BODY_CORE_BOTTOM_WIDTH, NouveauBoss::BODY_CORE_HEIGHT, 0.66f);
    this->bodyFireEmitter      = this->BuildFireEmitter(NouveauBoss::BODY_CORE_BOTTOM_WIDTH,  NouveauBoss::BODY_CORE_HEIGHT, 0.66f);
    this->bodyExplodingEmitter = this->BuildExplodingEmitter(NouveauBoss::BODY_CORE_BOTTOM_WIDTH, NouveauBoss::BODY_CORE_HEIGHT, 0.85f);

    this->leftArmExplodingEmitter  = this->BuildExplodingEmitter(NouveauBoss::ARM_SPHERE_HOLDER_CURL_WIDTH, NouveauBoss::ARM_SPHERE_HOLDER_CURL_HEIGHT);
    this->rightArmExplodingEmitter = this->BuildExplodingEmitter(NouveauBoss::ARM_SPHERE_HOLDER_CURL_WIDTH, NouveauBoss::ARM_SPHERE_HOLDER_CURL_HEIGHT);

#define INIT_ANIM(anim) anim.ClearLerp(); anim.SetInterpolantValue(0.0f); anim.SetRepeat(false);
    INIT_ANIM(this->leftSideTopGlowAnim);
    INIT_ANIM(this->leftSideBottomGlowAnim);
    INIT_ANIM(this->rightSideTopGlowAnim);
    INIT_ANIM(this->rightSideBottomGlowAnim);
    INIT_ANIM(this->bottomGlowAnim);
#undef INIT_ANIM

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
    {
        std::vector<double> timeVals;
        timeVals.reserve(3);
        timeVals.push_back(0.0);
        timeVals.push_back(0.9);
        timeVals.push_back(1.8);
        std::vector<Colour> colourVals;
        colourVals.reserve(timeVals.size());
        colourVals.push_back(Colour(1,1,1));
        colourVals.push_back(Colour(0.33f, 0.33f, 0.33f));
        colourVals.push_back(Colour(1,1,1));

        this->frillColourAnim.SetLerp(timeVals, colourVals);
        this->frillColourAnim.SetRepeat(true);
    }
}

NouveauBossMesh::~NouveauBossMesh() {
    this->boss = NULL;

    bool success = false;

    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->bodyMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->bottomCurlMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->bottomHexSphereMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->sideCurlsMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->sideSphereMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->sideSphereFrillsMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->sideSphereHolderCurlMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->topDomeMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->topGazeboMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->topSphereMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->circleGlowTex);
    assert(success);

    UNUSED_VARIABLE(success);

    delete this->topGazeboFireEmitter;
    this->topGazeboFireEmitter = NULL;
    delete this->topGazeboSmokeEmitter;
    this->topGazeboSmokeEmitter = NULL;
    delete this->topGazeboExplodingEmitter;
    this->topGazeboExplodingEmitter = NULL;
    delete this->topDomeExplodingEmitter;
    this->topDomeExplodingEmitter = NULL;
    delete this->bodySmokeEmitter;
    this->bodySmokeEmitter = NULL;
    delete this->bodyExplodingEmitter;
    this->bodyExplodingEmitter = NULL;
    delete this->bodyFireEmitter;
    this->bodyFireEmitter = NULL;
    delete this->leftArmExplodingEmitter;
    this->leftArmExplodingEmitter = NULL;
    delete this->rightArmExplodingEmitter;
    this->rightArmExplodingEmitter = NULL;
}

double NouveauBossMesh::ActivateIntroAnimation() {
    static const float MAX_GLOW_ALPHA = 0.75f;
    static const double BOTTOM_GLOW_TIME = INTRO_TIME_IN_SECS/3.0;

    double nextStartTime = 0.0;

    const double TIME_PER_SIDE_GLOW = 
        (INTRO_TIME_IN_SECS - nextStartTime - BOTTOM_GLOW_TIME - InGameBossLevelDisplayState::TIME_OF_UNPAUSE_BEFORE_INTRO_END) / 4.0;

    this->leftSideTopGlowAnim.SetLerp(nextStartTime, nextStartTime + TIME_PER_SIDE_GLOW, 0.0f, MAX_GLOW_ALPHA);
    nextStartTime += TIME_PER_SIDE_GLOW;
    this->rightSideBottomGlowAnim.SetLerp(nextStartTime, nextStartTime + TIME_PER_SIDE_GLOW, 0.0f, MAX_GLOW_ALPHA);
    nextStartTime += TIME_PER_SIDE_GLOW;
    this->leftSideBottomGlowAnim.SetLerp(nextStartTime, nextStartTime + TIME_PER_SIDE_GLOW, 0.0f, MAX_GLOW_ALPHA);
    nextStartTime += TIME_PER_SIDE_GLOW;
    this->rightSideTopGlowAnim.SetLerp(nextStartTime, nextStartTime + TIME_PER_SIDE_GLOW, 0.0f, MAX_GLOW_ALPHA);
    nextStartTime += TIME_PER_SIDE_GLOW;

    this->bottomGlowAnim.SetLerp(nextStartTime, nextStartTime + BOTTOM_GLOW_TIME, 0.0f, MAX_GLOW_ALPHA);
    
    this->introTimeCountdown = INTRO_TIME_IN_SECS;

    return NouveauBossMesh::INTRO_TIME_IN_SECS;
}

void NouveauBossMesh::DrawBody(double dT, const Camera& camera, const BasicPointLight& keyLight,
                               const BasicPointLight& fillLight, const BasicPointLight& ballLight,
                               const GameAssets* assets) {

    UNUSED_PARAMETER(dT);

    // Using data from the GameModel's boss object, we draw the various pieces of the boss in their correct
    // worldspace locations...

    ColourRGBA currColour;

#define DRAW_BODY_PART(bodyPart, bodyPartMesh) \
    assert(bodyPart != NULL); \
    currColour = bodyPart->GetColour(); \
    if (currColour.A() > 0.0f) { \
        glPushMatrix(); \
        glMultMatrixf(bodyPart->GetWorldTransform().begin()); \
        glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A()); \
        bodyPartMesh->Draw(camera, keyLight, fillLight, ballLight); \
        glPopMatrix(); \
    }

    // Core Body
    const BossBodyPart* body = this->boss->GetBody();  
    DRAW_BODY_PART(body, this->bodyMesh);

    // Side curls
    const BossBodyPart* leftSideCurl = this->boss->GetLeftSideCurls();
    DRAW_BODY_PART(leftSideCurl, this->sideCurlsMesh);
    const BossBodyPart* rightSideCurl = this->boss->GetRightSideCurls();
    DRAW_BODY_PART(rightSideCurl, this->sideCurlsMesh);

    // Side arms (curl holder, sphere, frills)
    const BossBodyPart* leftHolderCurl       = this->boss->GetLeftSideHolderCurl();
    const BossBodyPart* leftSideSphere       = this->boss->GetLeftSideSphere();
    
    DRAW_BODY_PART(leftHolderCurl, this->sideSphereHolderCurlMesh);
    DRAW_BODY_PART(leftSideSphere, this->sideSphereMesh);

    const BossBodyPart* rightHolderCurl       = this->boss->GetRightSideHolderCurl();
    const BossBodyPart* rightSideSphere       = this->boss->GetRightSideSphere();
    DRAW_BODY_PART(rightHolderCurl, this->sideSphereHolderCurlMesh);
    DRAW_BODY_PART(rightSideSphere, this->sideSphereMesh);

    // Make the frills glow special colours to indicate that they must be destroyed (when they haven't yet been destroyed)
    const BossBodyPart* leftSideSphereFrills = this->boss->GetLeftSideSphereFrills();
    const BossBodyPart* rightSideSphereFrills = this->boss->GetRightSideSphereFrills();
    
    this->frillColourAnim.Tick(dT);
    Colour& frillColour = this->frillColourAnim.GetInterpolantValue();

    currColour = leftSideSphereFrills->GetColour();
    if (currColour.A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(leftSideSphereFrills->GetWorldTransform().begin());
        glColor4f(frillColour.R(), frillColour.G(), frillColour.B(), currColour.A());
        this->sideSphereFrillsMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    currColour = rightSideSphereFrills->GetColour();
    if (currColour.A() > 0.0f) {
        glPushMatrix();
        glMultMatrixf(rightSideSphereFrills->GetWorldTransform().begin());
        glColor4f(frillColour.R(), frillColour.G(), frillColour.B(), currColour.A());
        this->sideSphereFrillsMesh->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();
    }

    // Bottom hex-sphere
    const BossBodyPart* bottomHexSphere = this->boss->GetBottomHexSphere();
    DRAW_BODY_PART(bottomHexSphere, this->bottomHexSphereMesh);

    // Bottom curls
    for (int i = 0; i < NouveauBoss::NUM_CURLS; i++) {
        const BossBodyPart* bottomCurl = this->boss->GetBottomCurl(i);
        DRAW_BODY_PART(bottomCurl, this->bottomCurlMesh);
    }

    // Top sphere
    const BossBodyPart* topSphere = this->boss->GetTopSphere();
    DRAW_BODY_PART(topSphere, this->topSphereMesh);

    // Top Gazebo
    const BossBodyPart* topGazebo = this->boss->GetTopGazebo();
    DRAW_BODY_PART(topGazebo, this->topGazeboMesh);

    // Top Dome
    MaterialGroup* prismMatGrp = this->topDomeMesh->GetMaterialGroups().begin()->second;
    CgFxPrism* prismMaterial = static_cast<CgFxPrism*>(prismMatGrp->GetMaterial());
    prismMaterial->SetSceneTexture(assets->GetFBOAssets()->GetFullSceneFBO()->GetFBOTexture());

    const BossBodyPart* topDome = this->boss->GetTopDome();
    DRAW_BODY_PART(topDome, this->topDomeMesh);

#undef DRAW_BODY_PART
}

void NouveauBossMesh::DrawPostBodyEffects(double dT, const Camera& camera) {
    BossMesh::DrawPostBodyEffects(dT, camera);

    this->glowCirclePulseAnim.Tick(dT);
    float pulseScaler = this->glowCirclePulseAnim.GetInterpolantValue();

    // Check to see if we're drawing intro effects
    if (this->introTimeCountdown > 0.0) {
        this->leftSideTopGlowAnim.Tick(dT);
        this->leftSideBottomGlowAnim.Tick(dT);
        this->rightSideTopGlowAnim.Tick(dT);
        this->rightSideBottomGlowAnim.Tick(dT);
        this->bottomGlowAnim.Tick(dT);
        this->introTimeCountdown -= dT;
    }

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    // Left curl sphere holder (arm) effects
    const BossBodyPart* leftArmSphere  = this->boss->GetLeftSideSphere();
    if (leftArmSphere->GetAlpha() > 0.0f) {
        if (leftArmSphere->GetType() == AbstractBossBodyPart::WeakpointBodyPart) {
            const BossWeakpoint* leftArmSphereWeakpt = static_cast<const BossWeakpoint*>(leftArmSphere);
            
            if (leftArmSphereWeakpt->GetIsDestroyed()) {

                Point2D position = this->boss->GetLeftSideCurlHolderExplosionCenter();
                glPushMatrix();
                glTranslatef(position[0], position[1], 0);
                this->leftArmExplodingEmitter->Tick(dT);
                this->leftArmExplodingEmitter->Draw(camera);
                glPopMatrix();
            }
        }
    }

    // Right curl sphere holder (arm) effects
    const BossBodyPart* rightArmSphere = this->boss->GetRightSideSphere();
    if (rightArmSphere->GetAlpha() > 0.0f) {
        if (rightArmSphere->GetType() == AbstractBossBodyPart::WeakpointBodyPart) {
            const BossWeakpoint* rightArmSphereWeakpt = static_cast<const BossWeakpoint*>(rightArmSphere);
            
            if (rightArmSphereWeakpt->GetIsDestroyed()) {
                
                Point2D position = this->boss->GetRightSideCurlHolderExplosionCenter();
                glPushMatrix();
                glTranslatef(position[0], position[1], 0);
                this->rightArmExplodingEmitter->Tick(dT);
                this->rightArmExplodingEmitter->Draw(camera);
                glPopMatrix();
            }
        }
    }

    // Draw effects for the top sphere, gazebo and dome
    const BossBodyPart* topDome   = this->boss->GetTopDome();
    const BossBodyPart* gazebo    = this->boss->GetTopGazebo();
    const BossBodyPart* topSphere = this->boss->GetTopSphere();
    if (gazebo->GetAlpha() > 0.0f) {
        if (topSphere->GetType() == AbstractBossBodyPart::WeakpointBodyPart) {
            Point3D position = gazebo->GetTranslationPt3D();
            if (gazebo->GetIsDestroyed()) {
                glPushMatrix();
                glTranslatef(position[0], position[1], position[2]);
                this->topGazeboExplodingEmitter->Tick(dT);
                this->topGazeboExplodingEmitter->Draw(camera);
                glPopMatrix();
            }
            else {
                const BossWeakpoint* topSphereWeakpt = static_cast<const BossWeakpoint*>(topSphere);
                float lifePercentage = topSphereWeakpt->GetCurrentLifePercentage();
                if (lifePercentage < 1.0f) {
                    glPushMatrix();
                    glTranslatef(position[0], position[1], position[2]);

                    this->topGazeboSmokeEmitter->SetSpawnDelta(ESPInterval(lifePercentage*0.4f, lifePercentage*0.9f));
                    this->topGazeboSmokeEmitter->Tick(dT);
                    this->topGazeboSmokeEmitter->Draw(camera);

                    if (lifePercentage <= 0.5f) {
                        this->topGazeboFireEmitter->SetSpawnDelta(ESPInterval(lifePercentage*0.4f, lifePercentage*0.9f));
                        this->topGazeboFireEmitter->Tick(dT);
                        this->topGazeboFireEmitter->Draw(camera);
                    }
                    glPopMatrix();
                }
            }
        }
    }
    else {
        assert(gazebo->GetIsDestroyed());
        assert(topSphere->GetType() == AbstractBossBodyPart::WeakpointBodyPart);

        if (!topSphere->GetIsDestroyed()) {
            const BossWeakpoint* topSphereWeakpt = static_cast<const BossWeakpoint*>(topSphere);
            float lifePercentage = topSphereWeakpt->GetCurrentLifePercentage();
            
            if (lifePercentage < 1.0f) {
                Point3D position = this->boss->GetBody()->GetTranslationPt3D();
                
                glPushMatrix();
                glTranslatef(position[0], position[1], position[2]);

                this->bodySmokeEmitter->SetSpawnDelta(ESPInterval(lifePercentage*0.4f, lifePercentage*0.9f));
                this->bodySmokeEmitter->Tick(dT);
                this->bodySmokeEmitter->Draw(camera);

                if (lifePercentage <= 0.5f) {
                    this->bodyFireEmitter->SetSpawnDelta(ESPInterval(lifePercentage*0.4f, lifePercentage*0.9f));
                    this->bodyFireEmitter->Tick(dT);
                    this->bodyFireEmitter->Draw(camera);
                }
                glPopMatrix();
            }
        }
        else {
            // Final boss body explosion
            const BossBodyPart* body = this->boss->GetBody();
            if (body->GetAlpha() > 0.0f) {
                Point3D position = body->GetTranslationPt3D();
                glPushMatrix();
                glTranslatef(position[0], position[1], position[2]);
                this->bodyExplodingEmitter->SetParticleAlpha(body->GetAlpha());
                this->bodyExplodingEmitter->Tick(dT);
                this->bodyExplodingEmitter->Draw(camera);
                glPopMatrix();
            }
        }
    }

    if (topDome->GetAlpha() > 0.0f) {
        if (topDome->GetIsDestroyed()) {
            Point2D position = this->boss->GetTopDomeExplosionCenter();
            glPushMatrix();
            glTranslatef(position[0], position[1], 0);
            this->topDomeExplodingEmitter->Tick(dT);
            this->topDomeExplodingEmitter->Draw(camera);
            glPopMatrix();
        }
    }

    // Draw effects for the left side curls
    this->circleGlowTex->BindTexture();
    const BossBodyPart* leftSideCurls = this->boss->GetLeftSideCurls();
    if (leftSideCurls->GetAlpha() > 0.0f) {
        float scale = 0.75f * pulseScaler;
        Point3D currPos;
        
        currPos = this->boss->GetLeftSideCurlShootTopPt();
        glPushMatrix();
        glTranslatef(currPos[0], currPos[1], currPos[2]);
        glScalef(scale, scale, 1.0f);
        glColor4f(1.0f, 0.1f, 0.1f, this->leftSideTopGlowAnim.GetInterpolantValue());
        GeometryMaker::GetInstance()->DrawQuad();
        glPopMatrix();

        currPos = this->boss->GetLeftSideCurlShootBottomPt();
        glPushMatrix();
        glTranslatef(currPos[0], currPos[1], currPos[2]);
        glScalef(scale, scale, 1.0f);
        glColor4f(1.0f, 0.1f, 0.1f, this->leftSideBottomGlowAnim.GetInterpolantValue());
        GeometryMaker::GetInstance()->DrawQuad();
        glPopMatrix();
    }

    // Draw effects for the right side curls
    const BossBodyPart* rightSideCurls = this->boss->GetRightSideCurls();
    if (rightSideCurls->GetAlpha() > 0.0f) {
        float scale = 0.75f * pulseScaler;
        Point3D currPos;

        currPos = this->boss->GetRightSideCurlShootTopPt();
        glPushMatrix();
        glTranslatef(currPos[0], currPos[1], currPos[2]);
        glScalef(scale, scale, 1.0f);
        glColor4f(1.0f, 0.1f, 0.1f, rightSideCurls->GetAlpha() * this->rightSideTopGlowAnim.GetInterpolantValue());
        GeometryMaker::GetInstance()->DrawQuad();
        glPopMatrix();

        currPos = this->boss->GetRightSideCurlShootBottomPt();
        glPushMatrix();
        glTranslatef(currPos[0], currPos[1], currPos[2]);
        glScalef(scale, scale, 1.0f);
        glColor4f(1.0f, 0.1f, 0.1f, rightSideCurls->GetAlpha() * this->rightSideBottomGlowAnim.GetInterpolantValue());
        GeometryMaker::GetInstance()->DrawQuad();
        glPopMatrix();
    }

    // Draw effects for the bottom sphere
    const BossBodyPart* bottomSphere = this->boss->GetBottomHexSphere();
    if (bottomSphere->GetAlpha() > 0.0f) {
        float scale = 1.2f * pulseScaler;
        Point3D currPos = this->boss->GetBottomSphereShootPt();
        glPushMatrix();
        glTranslatef(currPos[0], currPos[1], currPos[2]);
        glScalef(scale, scale, 1.0f);
        glColor4f(1.0f, 0.1f, 0.1f, bottomSphere->GetAlpha() * this->bottomGlowAnim.GetInterpolantValue());
        GeometryMaker::GetInstance()->DrawQuad();
        glPopMatrix();
    }
    this->circleGlowTex->UnbindTexture();

    glPopAttrib();
}

Point3D NouveauBossMesh::GetBossFinalExplodingEpicenter() const {
    Point3D center(this->boss->GetBody()->GetTranslationPt2D(), NouveauBoss::BODY_CORE_MID_HALF_DEPTH);
    return center;
}