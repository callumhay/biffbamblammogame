/**
 * GothicRomanticBossMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GothicRomanticBossMesh.h"
#include "GameViewConstants.h"
#include "GameLightAssets.h"

#include "../ResourceManager.h"

#include "../BlammoEngine/Mesh.h"

#include "../ESPEngine/ESP.h"

#include "../GameModel/GothicRomanticBoss.h"
#include "../GameModel/BossWeakpoint.h"

const double GothicRomanticBossMesh::INTRO_TIME_IN_SECS = 3.0;

GothicRomanticBossMesh::GothicRomanticBossMesh(GothicRomanticBoss* boss) :
BossMesh(), boss(boss), bodyMesh(NULL), topPointMesh(NULL), bottomPointMesh(NULL), legMesh(NULL),
topPointSmokeEmitter(NULL), topPointFireEmitter(NULL), topPointExplodingEmitter(NULL) {
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
    this->topPointExplodingEmitter = this->BuildExplodingEmitter(GothicRomanticBoss::TOP_POINT_WIDTH, GothicRomanticBoss::TOP_POINT_HEIGHT);
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

    UNUSED_VARIABLE(success);

    delete this->topPointSmokeEmitter;
    this->topPointSmokeEmitter = NULL;
    delete this->topPointFireEmitter;
    this->topPointFireEmitter = NULL;
    delete this->topPointExplodingEmitter;
    this->topPointExplodingEmitter = NULL;
}

double GothicRomanticBossMesh::ActivateIntroAnimation() {
    // TODO
    
    return GothicRomanticBossMesh::INTRO_TIME_IN_SECS;
}

void GothicRomanticBossMesh::DrawPreBodyEffects(double dT, const Camera& camera) {
}

void GothicRomanticBossMesh::DrawBody(double dT, const Camera& camera, const BasicPointLight& keyLight,
                                      const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
 
   // Using data from the GameModel's boss object, we draw the various pieces of the boss in their correct
    // worldspace locations...

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

void GothicRomanticBossMesh::DrawPostBodyEffects(double dT, const Camera& camera) {

    const BossBodyPart* topPoint = this->boss->GetTopPoint();
    if (topPoint->GetAlpha() > 0.0f) {

        if (topPoint->GetType() == AbstractBossBodyPart::WeakpointBodyPart) {
            const BossWeakpoint* topPointWeakPt = static_cast<const BossWeakpoint*>(topPoint);
            const Point3D topPointPos = topPoint->GetTranslationPt3D();

            if (!topPointWeakPt->GetIsDestroyed()) {
                if (topPointWeakPt->GetCurrentLifePercentage() < 1.0) {
                    float lifePercentage = topPointWeakPt->GetCurrentLifePercentage();
                    this->topPointFireEmitter->SetSpawnDelta(ESPInterval(lifePercentage*0.4f, lifePercentage*0.9f));
                    this->topPointFireEmitter->Tick(dT);
                    this->topPointSmokeEmitter->SetSpawnDelta(ESPInterval(lifePercentage*0.4f, lifePercentage*0.9f));
                    this->topPointSmokeEmitter->Tick(dT);

                    glPushMatrix();
                    glTranslatef(topPointPos[0], topPointPos[1], topPointPos[2]);
                    this->topPointSmokeEmitter->Draw(camera);
                    this->topPointFireEmitter->Draw(camera);
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
}
