/**
 * BallBoostHUD.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#include "BallBoostHUD.h"
#include "GameViewConstants.h"

#include "../ResourceManager.h"
#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/GeometryMaker.h"
#include "../ESPEngine/ESPPointEmitter.h"


const float BallBoostHUD::BALL_FILL_START_PERCENT = 0.156f;
const float BallBoostHUD::BALL_FILL_END_PERCENT   = 0.828f;
const float BallBoostHUD::BALL_FILL_DIFF_PERCENT  = BALL_FILL_END_PERCENT - BALL_FILL_START_PERCENT;

BallBoostHUD::BallBoostHUD(int displayHeight) : ballHUDOutlineTex(NULL), ballFillTex(NULL), currNumTrailFills(0),
haloTexture(NULL), boostGainedHaloEmitter(NULL), haloExpander(1.0f, 2.75f), haloFader(1.0f, 0.10f), alpha(0.0f) {

    // Initialize the textures for the ball boost HUD
    this->ballHUDOutlineTex = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BOOST_HUD_OUTLINE,  Texture::Trilinear, GL_TEXTURE_2D);
    this->ballFillTex       = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BOOST_HUD_BALLFILL, Texture::Trilinear, GL_TEXTURE_2D);
    this->haloTexture       = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_HALO, Texture::Trilinear));
    
    assert(this->ballHUDOutlineTex != NULL);
    assert(this->ballFillTex       != NULL);
    assert(this->haloTexture       != NULL);

    // Initialize the trail fill components
    this->trailFills.reserve(3);
    this->trailFills.push_back(new BallBoostHUD::TrailFill(GameViewConstants::GetInstance()->TEXTURE_BOOST_HUD_TRAILFILL_1, ColourRGBA(1.0f, 1.0f, 0.0f, this->alpha)));
    this->trailFills.push_back(new BallBoostHUD::TrailFill(GameViewConstants::GetInstance()->TEXTURE_BOOST_HUD_TRAILFILL_2, ColourRGBA(1.0f, 0.7f, 0.0f, this->alpha)));
    this->trailFills.push_back(new BallBoostHUD::TrailFill(GameViewConstants::GetInstance()->TEXTURE_BOOST_HUD_TRAILFILL_3, ColourRGBA(1.0f, 0.3f, 0.0f, this->alpha)));

    // Initialize the emitter/effect for when boosts are gained
    static const float TOTAL_HALO_LIFE  = 0.8f;
    static const int NUM_HALO_PARTICLES = 3;
    this->boostGainedHaloEmitter = new ESPPointEmitter();
    this->boostGainedHaloEmitter->SetSpawnDelta(ESPInterval(TOTAL_HALO_LIFE / static_cast<float>(NUM_HALO_PARTICLES)));
	this->boostGainedHaloEmitter->SetInitialSpd(ESPInterval(0));
	this->boostGainedHaloEmitter->SetParticleLife(ESPInterval(TOTAL_HALO_LIFE));
    this->boostGainedHaloEmitter->SetNumParticleLives(1);
	this->boostGainedHaloEmitter->SetParticleSize(ESPInterval(BALL_BOOST_HUD_HEIGHT));
	this->boostGainedHaloEmitter->SetEmitAngleInDegrees(0);
	this->boostGainedHaloEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->boostGainedHaloEmitter->SetParticleAlignment(ESP::NoAlignment);

	this->boostGainedHaloEmitter->SetEmitPosition(
        Point3D(H_BORDER_SPACING + 0.887f * BALL_BOOST_HUD_WIDTH, 
                displayHeight - V_BORDER_SPACING - BALL_BOOST_HUD_HALF_HEIGHT,0));

	this->boostGainedHaloEmitter->AddEffector(&this->haloExpander);
	this->boostGainedHaloEmitter->AddEffector(&this->haloFader);
	bool result = this->boostGainedHaloEmitter->SetParticles(NUM_HALO_PARTICLES, this->haloTexture);
	assert(result);
}

BallBoostHUD::~BallBoostHUD() {
    // Clean up any effects
    delete this->boostGainedHaloEmitter;
    this->boostGainedHaloEmitter = NULL;

    // Clean up the textures
    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->ballHUDOutlineTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->ballFillTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->haloTexture);

    for (std::vector<BallBoostHUD::TrailFill*>::iterator iter = this->trailFills.begin();
         iter != this->trailFills.end(); ++iter) {
        BallBoostHUD::TrailFill* temp = *iter;
        delete temp;
        temp = NULL;
    }
    this->trailFills.clear();
}

void BallBoostHUD::Draw(const Camera& camera, const BallBoostModel* model,
                        int displayHeight, double dT) {
    if (this->alpha == 0.0f) {
        return;
    }

	// Prepare OGL for drawing the timer
	glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);			

	// Make world coordinates equal window coordinates
	Camera::PushWindowCoords();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

    // Draw any of the active effects for this HUD
    for (std::list<ESPPointEmitter*>::iterator iter = this->activeEffects.begin(); iter != this->activeEffects.end();) {
        ESPPointEmitter* currEmitter = *iter;
        if (currEmitter->IsDead()) {
            iter = this->activeEffects.erase(iter);
            continue;
        }
        currEmitter->Draw(camera);
        currEmitter->Tick(dT);
        ++iter;
    }

    glTranslatef(H_BORDER_SPACING + BALL_BOOST_HUD_HALF_WIDTH, 
                 displayHeight - V_BORDER_SPACING - BALL_BOOST_HUD_HALF_HEIGHT, 0);
    
    // Boost trail fills...
    for (std::vector<BallBoostHUD::TrailFill*>::iterator iter = this->trailFills.begin();
         iter != this->trailFills.end(); ++iter) {
        BallBoostHUD::TrailFill* trailFill = *iter;
        trailFill->Draw(dT);
    }

    // Draw the fills that are currently present in the HUD - these will only be present
    // if there's a boost model present
    if (model != NULL) {
        float percentBallFill = model->GetBoostChargePercentage();
        this->ballFillTex->BindTexture();

        // Ball fill: If this isn't the first boost then we draw the previous boost fill as well as
        // the current one that's filling up
        if (model->GetNumAvailableBoosts() != 0 && percentBallFill < 1.0) {
            const Colour& prevColour = this->trailFills[model->GetNumAvailableBoosts()-1]->GetColour();
            glColor4f(prevColour.R(), prevColour.G(), prevColour.B(), this->alpha);
	        glBegin(GL_QUADS);
	        glTexCoord2d(0, 0); glVertex2d(-BALL_BOOST_HUD_HALF_WIDTH, -BALL_BOOST_HUD_HALF_HEIGHT);
	        glTexCoord2d(1, 0); glVertex2d( BALL_BOOST_HUD_HALF_WIDTH, -BALL_BOOST_HUD_HALF_HEIGHT);
	        glTexCoord2d(1, 1); glVertex2d( BALL_BOOST_HUD_HALF_WIDTH,  BALL_BOOST_HUD_HALF_HEIGHT);
	        glTexCoord2d(0, 1); glVertex2d(-BALL_BOOST_HUD_HALF_WIDTH,  BALL_BOOST_HUD_HALF_HEIGHT);
	        glEnd();
        }

        float currBallHeight = BALL_BOOST_HUD_HEIGHT * BALL_FILL_START_PERCENT + BALL_BOOST_HUD_HEIGHT * BALL_FILL_DIFF_PERCENT * percentBallFill;
        float ballFillHeight = -BALL_BOOST_HUD_HALF_HEIGHT + currBallHeight;
        float texCoord = currBallHeight / BALL_BOOST_HUD_HEIGHT;

        const Colour& currColour = this->GetCurrentTrailFill()->GetColour();
        glColor4f(currColour.R(), currColour.G(), currColour.B(), this->alpha);
	    glBegin(GL_QUADS);
	    glTexCoord2d(0, 0);        glVertex2d(-BALL_BOOST_HUD_HALF_WIDTH, -BALL_BOOST_HUD_HALF_HEIGHT);
	    glTexCoord2d(1, 0);        glVertex2d( BALL_BOOST_HUD_HALF_WIDTH, -BALL_BOOST_HUD_HALF_HEIGHT);
	    glTexCoord2d(1, texCoord); glVertex2d( BALL_BOOST_HUD_HALF_WIDTH, ballFillHeight);
	    glTexCoord2d(0, texCoord); glVertex2d(-BALL_BOOST_HUD_HALF_WIDTH, ballFillHeight);
	    glEnd();

    }

    // Draw the HUD outline over the fill stuffs...
    this->ballHUDOutlineTex->BindTexture();
    glScalef(BALL_BOOST_HUD_WIDTH, BALL_BOOST_HUD_HEIGHT, 0.0f);
    glColor4f(1,1,1, this->alpha);
    GeometryMaker::GetInstance()->DrawQuad();

	// Pop modelview matrix
	glPopMatrix();
	// Pop the projection matrix
	Camera::PopWindowCoords();
	glPopAttrib();
}

void BallBoostHUD::BoostGained() {
    BallBoostHUD::TrailFill* currTrailFill = this->trailFills[this->currNumTrailFills];
    currTrailFill->Gained();
    const Colour& currFillColour = currTrailFill->GetColour();
    
    // Draw a pulse/halo effect to signify that a new boost has been gained
    this->boostGainedHaloEmitter->SetParticleColour(ESPInterval(currFillColour.R()), ESPInterval(currFillColour.G()), 
                                                    ESPInterval(currFillColour.B()), ESPInterval(1.0f));
    this->boostGainedHaloEmitter->Reset();
    this->activeEffects.push_back(this->boostGainedHaloEmitter);

    this->currNumTrailFills = std::min<int>(BallBoostModel::TOTAL_NUM_BOOSTS, this->currNumTrailFills + 1);
}

void BallBoostHUD::BoostLost() {
    if (this->currNumTrailFills == 0) {
        return;
    }
    this->activeEffects.clear();
    this->currNumTrailFills = std::max<int>(0, this->currNumTrailFills - 1);
    this->trailFills[this->currNumTrailFills]->Lost();
}

void BallBoostHUD::Reinitialize() {
    this->currNumTrailFills = 0;
    for (std::vector<BallBoostHUD::TrailFill*>::iterator iter = this->trailFills.begin();
         iter != this->trailFills.end(); ++iter) {
        BallBoostHUD::TrailFill* trailFill = *iter;
        trailFill->Clear();
    }
}

void BallBoostHUD::SetAlpha(float alpha) {
    this->alpha = alpha;
    for (std::vector<BallBoostHUD::TrailFill*>::iterator iter = this->trailFills.begin();
         iter != this->trailFills.end(); ++iter) {
        BallBoostHUD::TrailFill* trailFill = *iter;
        trailFill->SetAlpha(alpha);
    }
}

const double BallBoostHUD::TrailFill::FILL_TIME_IN_SECONDS    = 0.75;
const double BallBoostHUD::TrailFill::UNFILL_TIME_IN_SECONDS  = 0.55;

BallBoostHUD::TrailFill::TrailFill(const char* trailTexFilepath, const ColourRGBA& colour) : widthAnim(0.0f), colour(colour) {
    this->trailTex = ResourceManager::GetInstance()->GetImgTextureResource(trailTexFilepath, Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->trailTex != NULL);

    this->widthAnim.SetInterpolantValue(0.0f);
    this->widthAnim.SetRepeat(false);
}

BallBoostHUD::TrailFill::~TrailFill() {
    // Clean up the texture
    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->trailTex);
    assert(success);
}

void BallBoostHUD::TrailFill::Draw(double dT) {
    this->widthAnim.Tick(dT);
    float temp = this->widthAnim.GetInterpolantValue();
    if (temp == 0.0f) {
        return;
    }

    float texCoord = 1.0f - (temp / BALL_BOOST_HUD_WIDTH);
    temp = BALL_BOOST_HUD_HALF_WIDTH - temp;

    this->trailTex->BindTexture();
    glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2d(texCoord, 0); glVertex2d(temp, -BALL_BOOST_HUD_HALF_HEIGHT);
	glTexCoord2d(1, 0);        glVertex2d(BALL_BOOST_HUD_HALF_WIDTH, -BALL_BOOST_HUD_HALF_HEIGHT);
	glTexCoord2d(1, 1);        glVertex2d(BALL_BOOST_HUD_HALF_WIDTH, BALL_BOOST_HUD_HALF_HEIGHT);
	glTexCoord2d(texCoord, 1); glVertex2d(temp, BALL_BOOST_HUD_HALF_HEIGHT);
	glEnd();
}

void BallBoostHUD::TrailFill::Gained() {
    this->widthAnim.SetLerp(FILL_TIME_IN_SECONDS, BALL_BOOST_HUD_WIDTH);
}

void BallBoostHUD::TrailFill::Lost() {
    this->widthAnim.SetLerp(UNFILL_TIME_IN_SECONDS, 0.0f);
}

void BallBoostHUD::TrailFill::Clear() {
    this->widthAnim.ClearLerp();
    this->widthAnim.SetInterpolantValue(0.0f);
}