/**
 * SwitchBlockMesh.cpp
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

#include "SwitchBlockMesh.h"
#include "GameViewConstants.h"

#include "../ESPEngine/ESPPointEmitter.h"
#include "../GameModel/SwitchBlock.h"
#include "../ResourceManager.h"

SwitchBlockMesh::SwitchBlockMesh() : switchBlockGeometry(NULL), 
switchOnMaterialGrp(NULL), switchOffMaterialGrp(NULL), switchCurrentMaterialGrp(NULL),
greenOnSwitchTexture(NULL), redOnSwitchTexture(NULL), offSwitchTexture(NULL), haloTexture(NULL),
idlePulseEmitter(NULL), pulseGlowTexture(NULL), haloExpandPulse(1.0f, 3.0f), haloFader(1.0f, 0.15f), pulser(0,0) {

    this->LoadMesh();

    this->greenOnSwitchTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_GREEN_ON_SWITCH, Texture::Trilinear));
    this->redOnSwitchTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_RED_ON_SWITCH, Texture::Trilinear));
    this->offSwitchTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_OFF_SWITCH, Texture::Trilinear));
    assert(this->greenOnSwitchTexture != NULL);
    assert(this->redOnSwitchTexture != NULL);
    assert(this->offSwitchTexture != NULL);

	assert(this->haloTexture == NULL);
	this->haloTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_HALO, Texture::Trilinear));
	assert(this->haloTexture != NULL);

    this->pulseGlowTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CLEAN_CIRCLE_GRADIENT, Texture::Trilinear));
    assert(this->pulseGlowTexture != NULL);

    this->InitEmitters();
}

SwitchBlockMesh::~SwitchBlockMesh() {
    delete this->onEmitter;
    this->onEmitter = NULL;
    delete this->idlePulseEmitter;
    this->idlePulseEmitter = NULL;

    // Restore the mesh to its initial state (off) before cleaning up
    MaterialProperties* switchOnMatProperties      = this->switchOnMaterialGrp->GetMaterial()->GetProperties();
    MaterialProperties* switchOffMatProperties     = this->switchOffMaterialGrp->GetMaterial()->GetProperties();
    MaterialProperties* switchCurrentMatProperties = this->switchCurrentMaterialGrp->GetMaterial()->GetProperties();
    switchOffMatProperties->diffuseTexture     = this->redOnSwitchTexture;
    switchOnMatProperties->diffuseTexture      = this->offSwitchTexture;
    switchCurrentMatProperties->diffuseTexture = this->redOnSwitchTexture;

    bool success = ResourceManager::GetInstance()->ReleaseMeshResource(this->switchBlockGeometry);
	assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->greenOnSwitchTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->redOnSwitchTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->offSwitchTexture);
    assert(success);

    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->haloTexture);
	assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->pulseGlowTexture);
    assert(success);

    UNUSED_VARIABLE(success);

    this->Flush();
}

void SwitchBlockMesh::Flush() {
    this->switchBlocks.clear();
    for (std::list<SwitchBlockMesh::SwitchConnection*>::iterator iter = this->activeConnections.begin();
         iter != this->activeConnections.end(); ++iter) {

        SwitchBlockMesh::SwitchConnection* currConn = *iter;
        delete currConn;
        currConn = NULL;
    }
    this->activeConnections.clear();
}

void SwitchBlockMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
                           const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
    

    MaterialProperties* switchOnMatProperties       = this->switchOnMaterialGrp->GetMaterial()->GetProperties();
    MaterialProperties* switchOffMatProperties      = this->switchOffMaterialGrp->GetMaterial()->GetProperties();
    MaterialProperties* switchCurrentMatProperties  = this->switchCurrentMaterialGrp->GetMaterial()->GetProperties();
    
    glPushAttrib(GL_TEXTURE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);

    // Go through each switch block and draw it...
    for (std::set<const SwitchBlock*>::const_iterator iter = this->switchBlocks.begin(); iter != this->switchBlocks.end(); ++iter) {
        const SwitchBlock* switchBlock = *iter;

        const Point2D& blockCenter = switchBlock->GetCenter();
        
		glPushMatrix();
		glTranslatef(blockCenter[0], blockCenter[1], 0.0f);

        // Set the texture based on whether the block is currently switched on or not
        if (switchBlock->GetIsSwitchOn()) {
            switchOnMatProperties->diffuseTexture      = this->greenOnSwitchTexture;
            switchOffMatProperties->diffuseTexture     = this->offSwitchTexture;
            switchCurrentMatProperties->diffuseTexture = this->greenOnSwitchTexture;
            this->onEmitter->Draw(camera);
            BasicPointLight newKeyLight(Point3D(blockCenter[0], blockCenter[1], 10), Colour(1, 1, 1), 0.0f);
            this->switchOnMaterialGrp->Draw(camera, newKeyLight, fillLight, ballLight);
            this->switchOffMaterialGrp->Draw(camera, newKeyLight, fillLight, ballLight);
            this->switchCurrentMaterialGrp->Draw(camera, newKeyLight, fillLight, ballLight);
        }
        else {
            switchOffMatProperties->diffuseTexture     = this->redOnSwitchTexture;
            switchOnMatProperties->diffuseTexture      = this->offSwitchTexture;
            switchCurrentMatProperties->diffuseTexture = this->redOnSwitchTexture;

            this->idlePulseEmitter->Draw(camera);

            this->switchOnMaterialGrp->Draw(camera, keyLight, fillLight, ballLight);
            this->switchOffMaterialGrp->Draw(camera, keyLight, fillLight, ballLight);
            this->switchCurrentMaterialGrp->Draw(camera, keyLight, fillLight, ballLight);
        }

        glPopMatrix();
    }

    // Draw any active connections...
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    for (std::list<SwitchBlockMesh::SwitchConnection*>::iterator iter = this->activeConnections.begin();
         iter != this->activeConnections.end();) {

        SwitchBlockMesh::SwitchConnection* currConn = *iter;
        if (currConn->Draw(dT, camera)) {
            iter = this->activeConnections.erase(iter);
            delete currConn;
            currConn = NULL;
        }
        else {
            ++iter;
        }
    }

    glPopAttrib();
    this->onEmitter->Tick(dT);
    this->idlePulseEmitter->Tick(dT);
}

void SwitchBlockMesh::SetAlphaMultiplier(float alpha) {
	const std::map<std::string, MaterialGroup*>&  matGrps = this->switchBlockGeometry->GetMaterialGroups();
	for (std::map<std::string, MaterialGroup*>::const_iterator iter = matGrps.begin(); iter != matGrps.end(); ++iter) {
		MaterialGroup* matGrp = iter->second;
		matGrp->GetMaterial()->GetProperties()->alphaMultiplier = alpha;
	}

    this->onEmitter->SetParticleAlpha(alpha);
    this->idlePulseEmitter->SetParticleAlpha(alpha);
}

void SwitchBlockMesh::LoadMesh() {
    assert(this->switchBlockGeometry == NULL);
    assert(this->switchOnMaterialGrp == NULL);
    assert(this->switchOffMaterialGrp == NULL);
    assert(this->switchCurrentMaterialGrp == NULL);
    assert(this->baseMaterialGrp.empty());

    this->switchBlockGeometry = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->SWITCH_BLOCK_MESH);
    assert(this->switchBlockGeometry != NULL);

	// Set the base material group map (which never changes its texture) and also set the on and off material groups; 
	// which changes its texture on a per-block basis depending on whether a particular switch is on/off
	this->baseMaterialGrp = this->switchBlockGeometry->GetMaterialGroups();

    // On switch material group...
    std::map<std::string, MaterialGroup*>::iterator findIter = this->baseMaterialGrp.find(GameViewConstants::GetInstance()->SWITCH_ON_MATGRP);
    assert(findIter != this->baseMaterialGrp.end());
    this->switchOnMaterialGrp = findIter->second;
    assert(this->switchOnMaterialGrp != NULL);
	this->baseMaterialGrp.erase(findIter);

    // Off switch material group...
    findIter = this->baseMaterialGrp.find(GameViewConstants::GetInstance()->SWITCH_OFF_MATGRP);
    assert(findIter != this->baseMaterialGrp.end());
    this->switchOffMaterialGrp = findIter->second;
    assert(this->switchOffMaterialGrp != NULL);
	this->baseMaterialGrp.erase(findIter);

    // Current switch mode material group...
    findIter = this->baseMaterialGrp.find(GameViewConstants::GetInstance()->SWITCH_CURRENT_MATGRP);
    assert(findIter != this->baseMaterialGrp.end());
    this->switchCurrentMaterialGrp = findIter->second;
    assert(this->switchCurrentMaterialGrp != NULL);
	this->baseMaterialGrp.erase(findIter);

    assert(this->baseMaterialGrp.size() == 1);
}

void SwitchBlockMesh::InitEmitters() {

	// Halo effect that pulses outwards when a switch block is turned on
	const float HALO_LIFETIME = 1.5f;
	const int NUM_HALOS = 3;
	this->onEmitter = new ESPPointEmitter();
	this->onEmitter->SetSpawnDelta(ESPInterval(HALO_LIFETIME / static_cast<float>(NUM_HALOS)));
	this->onEmitter->SetInitialSpd(ESPInterval(0));
	this->onEmitter->SetParticleLife(ESPInterval(HALO_LIFETIME));
	this->onEmitter->SetParticleSize(ESPInterval(1.5 * LevelPiece::HALF_PIECE_WIDTH), ESPInterval(1.5 * LevelPiece::HALF_PIECE_HEIGHT));
	this->onEmitter->SetEmitAngleInDegrees(0);
	this->onEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    this->onEmitter->SetParticleAlignment(ESP::GlobalAxisAlignedX);
	this->onEmitter->SetEmitPosition(Point3D(0,0,0));
	this->onEmitter->SetParticleColour(ESPInterval(0), ESPInterval(1), ESPInterval(0), ESPInterval(1.0f));
	this->onEmitter->AddEffector(&this->haloExpandPulse);
	this->onEmitter->AddEffector(&this->haloFader);
	this->onEmitter->SetParticles(NUM_HALOS, this->haloTexture);

    ScaleEffect pulseSettings;
    pulseSettings.pulseGrowthScale = 1.45f;
    pulseSettings.pulseRate = 0.5f;
    this->pulser = ESPParticleScaleEffector(pulseSettings);

    this->idlePulseEmitter = new ESPPointEmitter();
    this->idlePulseEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    this->idlePulseEmitter->SetInitialSpd(ESPInterval(0));
    this->idlePulseEmitter->SetParticleLife(ESPInterval(ESPParticle::INFINITE_PARTICLE_LIFETIME));
    this->idlePulseEmitter->SetParticleSize(ESPInterval(1.2f*LevelPiece::PIECE_WIDTH), ESPInterval(1.2f*LevelPiece::PIECE_HEIGHT));
    this->idlePulseEmitter->SetEmitAngleInDegrees(0);
    this->idlePulseEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    this->idlePulseEmitter->SetParticleAlignment(ESP::GlobalAxisAlignedX);
    this->idlePulseEmitter->SetEmitPosition(Point3D(0, 0, 0));
    this->idlePulseEmitter->SetParticleColour(ESPInterval(1), ESPInterval(0), ESPInterval(0), ESPInterval(0.75f));
    this->idlePulseEmitter->AddEffector(&this->pulser);
    this->idlePulseEmitter->SetParticles(1, this->pulseGlowTexture);
}

SwitchBlockMesh::SwitchConnection::SwitchConnection(const SwitchBlock* switchBlock,
                                                    const LevelPiece* triggeredPiece) : 
alphaAnim(1), glowBitTexture(NULL), sparkleTexture(NULL), glowEmitter1(NULL), glowEmitter2(NULL),
particleFader(1, 0.1f), particleGrower(1.0f, 1.5f) {

    assert(switchBlock != NULL);
    assert(triggeredPiece != NULL);
    
    this->glowBitTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, Texture::Trilinear, GL_TEXTURE_2D));
    this->sparkleTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SPARKLE, Texture::Trilinear, GL_TEXTURE_2D));
    this->glowEmitter1 = this->BuildGlowEmitter(triggeredPiece->GetCenter(), this->glowBitTexture);
    this->glowEmitter2 = this->BuildGlowEmitter(triggeredPiece->GetCenter(), this->sparkleTexture);
    
    // Build the connection line from the switch block to the location
    // of the triggered block
    size_t switchXIdx = switchBlock->GetWidthIndex();
    size_t switchYIdx = switchBlock->GetHeightIndex();
    size_t trigXIdx   = triggeredPiece->GetWidthIndex();
    size_t trigYIdx   = triggeredPiece->GetHeightIndex();

    const Point2D& switchCenterPt = switchBlock->GetCenter();
    const Point2D& trigCenterPt   = triggeredPiece->GetCenter();
    
    // Figure out where the triggered piece is in relation to the switch
    if (trigXIdx < switchXIdx) {
        
        Point2D startPt(switchCenterPt[0] - LevelPiece::HALF_PIECE_WIDTH, switchCenterPt[1]);
        this->points.push_back(startPt);

        if (trigYIdx < switchYIdx) {
            // Triggered piece is to the bottom-left of the switch
            Point2D midPt(trigCenterPt[0], startPt[1]);
            Point2D endPt(trigCenterPt[0], trigCenterPt[1] + LevelPiece::HALF_PIECE_HEIGHT);
            this->points.push_back(midPt);
            this->points.push_back(endPt);
        }
        else if (trigYIdx > switchYIdx) {
            // Triggered piece is to the top-left of the switch
            Point2D midPt(trigCenterPt[0], startPt[1]);
            Point2D endPt(trigCenterPt[0], trigCenterPt[1] - LevelPiece::HALF_PIECE_HEIGHT);
            this->points.push_back(midPt);
            this->points.push_back(endPt);
        }
        else {
            // Triggered piece is to the left of the switch
            Point2D endPt(trigCenterPt[0] + LevelPiece::HALF_PIECE_WIDTH, trigCenterPt[1]);
            this->points.push_back(endPt);
        }
    }
    else if (trigXIdx > switchXIdx) {
        Point2D startPt(switchCenterPt[0] + LevelPiece::HALF_PIECE_WIDTH, switchCenterPt[1]);
        this->points.push_back(startPt);

        if (trigYIdx < switchYIdx) {
            // Triggered piece is to the bottom-right of the switch
            Point2D midPt(trigCenterPt[0], startPt[1]);
            Point2D endPt(trigCenterPt[0], trigCenterPt[1] + LevelPiece::HALF_PIECE_HEIGHT);
            this->points.push_back(midPt);
            this->points.push_back(endPt);
        }
        else if (trigYIdx > switchYIdx) {
            // Triggered piece is to the top-right of the switch
            Point2D midPt(trigCenterPt[0], startPt[1]);
            Point2D endPt(trigCenterPt[0], trigCenterPt[1] - LevelPiece::HALF_PIECE_HEIGHT);
            this->points.push_back(midPt);
            this->points.push_back(endPt);
        }
        else {
            // Triggered piece is to the right of the switch
            Point2D endPt(trigCenterPt[0] - LevelPiece::HALF_PIECE_WIDTH, trigCenterPt[1]);
            this->points.push_back(endPt);
        }
    }
    else {
        // Same location on x axis
        if (trigYIdx < switchYIdx) {
            // Triggered piece is to the bottom of the switch
            Point2D startPt(switchCenterPt[0], switchCenterPt[1] - LevelPiece::HALF_PIECE_HEIGHT);
            Point2D endPt(trigCenterPt[0], trigCenterPt[1] + LevelPiece::HALF_PIECE_HEIGHT);
            this->points.push_back(startPt);
            this->points.push_back(endPt);
        }
        else if (trigYIdx > switchYIdx) {
            // Triggered piece is to the top of the switch
            Point2D startPt(switchCenterPt[0], switchCenterPt[1] + LevelPiece::HALF_PIECE_HEIGHT);
            Point2D endPt(trigCenterPt[0], trigCenterPt[1] - LevelPiece::HALF_PIECE_HEIGHT);
            this->points.push_back(startPt);
            this->points.push_back(endPt);
        }
        else {
            assert(false);
        }
    }

    double animationTime = static_cast<double>(SwitchBlock::RESET_TIME_IN_MS) / 1000.0;
    this->alphaAnim.SetLerp(animationTime, 0.0f);
    this->alphaAnim.SetInterpolantValue(1.0f);
    this->alphaAnim.SetRepeat(false);
}

SwitchBlockMesh::SwitchConnection::~SwitchConnection() {
    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->glowBitTexture);
	assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->sparkleTexture);
    assert(success);

    delete this->glowEmitter1;
    this->glowEmitter1 = NULL;
    delete this->glowEmitter2;
    this->glowEmitter2 = NULL;
}

// Draws the connection and ticks it.
// Returns: true if the connection is done animating/drawing, false otherwise
bool SwitchBlockMesh::SwitchConnection::Draw(double dT, const Camera& camera) {
    float currAlpha = this->alphaAnim.GetInterpolantValue();
    //float size = this->lineAnim.GetInterpolantValue();

    // Draw a connection line using the points
    glColor4f(0, 0.78f, 0.35f, currAlpha);
    glLineWidth(5.0f);
    glBegin(GL_LINE_STRIP);
    for (size_t i = 0; i < this->points.size(); i++) {
        const Point2D& currPt = this->points[i];
        glVertex2f(currPt[0], currPt[1]);
    }
    glEnd();
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    for (size_t i = 0; i < this->points.size(); i++) {
        const Point2D& currPt = this->points[i];
        glVertex2f(currPt[0], currPt[1]);
    }
    glEnd();

    glColor4f(1, 1, 1, currAlpha);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    for (size_t i = 0; i < this->points.size(); i++) {
        const Point2D& currPt = this->points[i];
        glVertex2f(currPt[0], currPt[1]);
    }
    glEnd();
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (size_t i = 0; i < this->points.size(); i++) {
        const Point2D& currPt = this->points[i];
        glVertex2f(currPt[0], currPt[1]);
    }
    glEnd();

    this->glowEmitter1->Tick(dT);
    this->glowEmitter2->Tick(dT);
    this->glowEmitter1->Draw(camera);
    this->glowEmitter2->Draw(camera);

    return this->alphaAnim.Tick(dT);
}

ESPPointEmitter* SwitchBlockMesh::SwitchConnection::BuildGlowEmitter(const Point2D& position, Texture2D* texture) {
    static const int NUM_PARTICLES = 15;
    ESPPointEmitter* glowEmitter = new ESPPointEmitter();
    glowEmitter->SetNumParticleLives(1);
	glowEmitter->SetSpawnDelta(ESPInterval(0.005f, 0.01f));
	glowEmitter->SetInitialSpd(ESPInterval(2.0f, 5.0f));
	glowEmitter->SetParticleLife(ESPInterval(1.0f, 3.0f));
    glowEmitter->SetParticleSize(ESPInterval(0.15f * LevelPiece::HALF_PIECE_WIDTH, 0.4f * LevelPiece::HALF_PIECE_WIDTH));
	glowEmitter->SetParticleColour(ESPInterval(0.7f), ESPInterval(0.9f, 1.0f), ESPInterval(0.7f, 1.0f), ESPInterval(1.0f));
	glowEmitter->SetEmitAngleInDegrees(180);
	glowEmitter->SetRadiusDeviationFromCenter(0);
	glowEmitter->SetEmitPosition(Point3D(position,0));
	glowEmitter->AddEffector(&this->particleFader);
    glowEmitter->AddEffector(&this->particleGrower);
	glowEmitter->SetParticles(NUM_PARTICLES, texture);
    return glowEmitter;
}
