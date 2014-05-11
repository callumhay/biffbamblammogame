/**
 * RandomToItemAnimation.cpp
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

#include "RandomToItemAnimation.h"
#include "GameViewConstants.h"
#include "GameESPAssets.h"

#include "../Blammopedia.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/PlayerPaddle.h"
#include "../GameModel/LevelPiece.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/GeometryMaker.h"
#include "../ResourceManager.h"

RandomToItemAnimation::RandomToItemAnimation() : currItemFromRandomTexture(NULL),
isAnimating(false), itemNameEffect(NULL) {

	this->fadeAnim.SetInterpolantValue(0.0f);
	this->fadeAnim.SetRepeat(false);
	this->itemMoveAnim.SetInterpolantValue(Point2D(0.0f, 0.0f));
	this->itemMoveAnim.SetRepeat(false);
    this->scaleAnim.SetInterpolantValue(0.0f);
    this->scaleAnim.SetRepeat(false);
}

RandomToItemAnimation::~RandomToItemAnimation() {
	//bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->randomItemTexture);
	//assert(success);
    if (this->itemNameEffect != NULL) {
        delete this->itemNameEffect;
        this->itemNameEffect = NULL;
    }
}

void RandomToItemAnimation::Start(const GameItem& actualItem, const Texture* gameItemTexture,
                                  const GameModel& gameModel, GameESPAssets* espAssets) {

	assert(gameItemTexture != NULL);
    assert(espAssets != NULL);

	const PlayerPaddle& paddle = *gameModel.GetPlayerPaddle();

	// If the paddle camera is active then we just don't do the animation, tough luck!
	if (paddle.GetIsPaddleCameraOn()) {
		return;
	}
	this->currItemFromRandomTexture = gameItemTexture;
	
	// Setup all of the animations
	static const double TOTAL_ANIMATION_TIME = 2.25;

    float startYPos = 2*paddle.GetHalfHeight() + GameItem::ITEM_HEIGHT;
    float endYPos   = 2*paddle.GetHalfHeight() + 4*GameItem::ITEM_HEIGHT;
    float distance  = fabs(endYPos - startYPos);
    float speed     = static_cast<float>(distance / TOTAL_ANIMATION_TIME);

	// Move it over the course of the entire animation
    Point2D startLocation = Point2D(paddle.GetCenterPosition()[0], startYPos);
    Point2D endLocation   = Point2D(paddle.GetCenterPosition()[0], endYPos);
	this->itemMoveAnim.SetLerp(0.0, TOTAL_ANIMATION_TIME, startLocation, endLocation);
	this->itemMoveAnim.SetInterpolantValue(startLocation);

	// Fade out while continuing to move
	this->fadeAnim.SetLerp(0.0, TOTAL_ANIMATION_TIME, 1.0f, 0.0f);
	this->fadeAnim.SetInterpolantValue(1.0f);

    // Scale while moving
    this->scaleAnim.SetLerp(0.0f, TOTAL_ANIMATION_TIME, 1.0f, 1.5f);
    this->scaleAnim.SetInterpolantValue(1.0f);

    assert(actualItem.GetItemType() != GameItem::LifeUpItem);
    this->itemNameEffect = espAssets->CreateItemNameEffect(paddle, actualItem);
    assert(this->itemNameEffect != NULL);

    Point2D emitPos = paddle.GetCenterPosition() + (2*paddle.GetHalfHeight() + 2.5*GameItem::ITEM_HEIGHT) * paddle.GetUpVector();
    this->itemNameEffect->SetInitialSpd(ESPInterval(speed));
    this->itemNameEffect->SetEmitPosition(Point3D(emitPos, 0.0f));
    this->itemNameEffect->SetParticleLife(ESPInterval(TOTAL_ANIMATION_TIME));

	this->isAnimating = true;
}

void RandomToItemAnimation::Stop() {
	this->isAnimating = false;
    if (this->itemNameEffect != NULL) {
        delete this->itemNameEffect;
        this->itemNameEffect = NULL;
    }
}

void RandomToItemAnimation::Draw(const Camera& camera, double dT) {
	if (!this->isAnimating) {
		return;
	}

	// Generate a screen align matrix
	Matrix4x4 screenAlignMatrix = camera.GenerateScreenAlignMatrix();

	// Now draw the actual graphics of the animation...
	glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_POLYGON_BIT | GL_LINE_BIT);
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const Point2D& movePt = this->itemMoveAnim.GetInterpolantValue();
	glPushMatrix();
	glTranslatef(movePt[0], movePt[1], 0.0f);
    
    float actualItemAlpha = this->fadeAnim.GetInterpolantValue();
    float itemScale = this->scaleAnim.GetInterpolantValue();

	glMultMatrixf(screenAlignMatrix.begin());
    glScalef(itemScale*2, itemScale, 1.0f);

	if (actualItemAlpha > 0.0f) {
		// Item
        glColor4f(1.0f, 1.0f, 1.0f, actualItemAlpha);
		this->currItemFromRandomTexture->BindTexture();
		GeometryMaker::GetInstance()->DrawQuad();

        // Outline
		glLineWidth(2.0f);
		glColor4f(0.0f, 0.0f, 0.0f, actualItemAlpha);
		glPolygonMode(GL_FRONT, GL_LINE);
		GeometryMaker::GetInstance()->DrawQuad();
	}

	glPopMatrix();
	glPopAttrib();

    this->itemNameEffect->Draw(camera);
    this->itemNameEffect->Tick(dT);

	// Tick all the animations
	this->fadeAnim.Tick(dT);
    this->scaleAnim.Tick(dT);
	this->isAnimating = !this->fadeAnim.Tick(dT);
	this->itemMoveAnim.Tick(dT);

    if (!this->isAnimating) {
        assert(this->itemNameEffect != NULL);
        delete this->itemNameEffect;
        this->itemNameEffect = NULL;
    }
}