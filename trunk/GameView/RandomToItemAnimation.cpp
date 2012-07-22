/**
 * RandomToItemAnimation.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "RandomToItemAnimation.h"
#include "GameViewConstants.h"
#include "../Blammopedia.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/PlayerPaddle.h"
#include "../GameModel/LevelPiece.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/GeometryMaker.h"
#include "../ResourceManager.h"

RandomToItemAnimation::RandomToItemAnimation() :
randomItemTexture(NULL), currItemFromRandomTexture(NULL), isAnimating(false) {
	Blammopedia* blammopedia = ResourceManager::GetInstance()->GetBlammopedia();
	assert(blammopedia != NULL);
	const Blammopedia::ItemEntry* randomItemEntry = blammopedia->GetItemEntry(GameItem::RandomItem);
	assert(randomItemEntry != NULL);
	this->randomItemTexture = randomItemEntry->GetItemTexture();
	assert(randomItemTexture != NULL);

	this->initialFadeInAnim.SetInterpolantValue(0.0f);
	this->initialFadeInAnim.SetRepeat(false);
	this->randomFadeOutItemFadeInAnim.SetInterpolantValue(0.0f);
	this->randomFadeOutItemFadeInAnim.SetRepeat(false);
	this->finalFadeOutAnim.SetInterpolantValue(0.0f);
	this->finalFadeOutAnim.SetRepeat(false);
	this->itemMoveAnim.SetInterpolantValue(Point2D(0.0f, 0.0f));
	this->itemMoveAnim.SetRepeat(false);
}

RandomToItemAnimation::~RandomToItemAnimation() {
	//bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->randomItemTexture);
	//assert(success);
}

void RandomToItemAnimation::Start(const Texture* gameItemTexture, const GameModel& gameModel) {
	assert(gameItemTexture != NULL);

	const PlayerPaddle& paddle = *gameModel.GetPlayerPaddle();

	// If the paddle camera is active then we just don't do the animation, tough luck!
	if (paddle.GetIsPaddleCameraOn()) {
		return;
	}
	this->currItemFromRandomTexture = gameItemTexture;
	
	// Setup all of the animations:
	
	static const double TOTAL_FADE_OUT_TO_FADE_IN_TIME = 1.25;
	static const double TOTAL_INITIAL_FADE_IN_TIME     = 0.33;
	static const double TOTAL_FINAL_FADE_OUT_TIME      = 0.33;
	static const double TOTAL_ANIMATION_TIME = TOTAL_FADE_OUT_TO_FADE_IN_TIME + TOTAL_INITIAL_FADE_IN_TIME +
		                                         TOTAL_FINAL_FADE_OUT_TIME;

	// Start with a fade in of the random item...
	this->initialFadeInAnim.SetLerp(0.0, TOTAL_INITIAL_FADE_IN_TIME, 0.0f, 1.0f);
	this->initialFadeInAnim.SetInterpolantValue(0.0f);

	Vector2D halfLevelDim = 0.5f * gameModel.GetLevelUnitDimensions();

	// Move it over the course of the entire animation
	Point2D startLocation = Point2D(-1.5f * paddle.GetHalfWidthTotal() + halfLevelDim[0], -(paddle.GetHalfHeight() + 0.5f * GameItem::ITEM_HEIGHT));
	Point2D endLocation   = Point2D( 1.5f * paddle.GetHalfWidthTotal() + halfLevelDim[0], -(paddle.GetHalfHeight() + 0.5f * GameItem::ITEM_HEIGHT));
	this->itemMoveAnim.SetLerp(TOTAL_INITIAL_FADE_IN_TIME, TOTAL_FADE_OUT_TO_FADE_IN_TIME + TOTAL_INITIAL_FADE_IN_TIME, startLocation, endLocation);
	this->itemMoveAnim.SetInterpolantValue(startLocation);

	// ...and fade it out as we fade the actual item in
	double endOfFadeOutFadeIn = TOTAL_INITIAL_FADE_IN_TIME + TOTAL_FADE_OUT_TO_FADE_IN_TIME;
	this->randomFadeOutItemFadeInAnim.SetLerp(TOTAL_INITIAL_FADE_IN_TIME, endOfFadeOutFadeIn, 1.0f, 0.0f);
	this->randomFadeOutItemFadeInAnim.SetInterpolantValue(1.0f);

	// And finish with a fade out while continuing to move
	this->finalFadeOutAnim.SetLerp(endOfFadeOutFadeIn, TOTAL_ANIMATION_TIME, 1.0f, 0.0f);
	this->finalFadeOutAnim.SetInterpolantValue(1.0f);

	this->isAnimating = true;
}

void RandomToItemAnimation::Stop() {
	this->isAnimating = false;
}

void RandomToItemAnimation::Draw(const Camera& camera, double dT) {
	if (!this->isAnimating) {
		return;
	}

	// Tick all the animations
	this->initialFadeInAnim.Tick(dT);
	this->randomFadeOutItemFadeInAnim.Tick(dT);
	this->isAnimating = !this->finalFadeOutAnim.Tick(dT);
	this->itemMoveAnim.Tick(dT);

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
	float randomItemAlpha = this->initialFadeInAnim.GetInterpolantValue() * this->finalFadeOutAnim.GetInterpolantValue() * this->randomFadeOutItemFadeInAnim.GetInterpolantValue();
	float actualItemAlpha = (1.0f - this->randomFadeOutItemFadeInAnim.GetInterpolantValue()) * this->finalFadeOutAnim.GetInterpolantValue();

	glMultMatrixf(screenAlignMatrix.begin());
    glScalef(GameItem::ITEM_WIDTH, GameItem::ITEM_HEIGHT, 1.0f);

	bool didDraw = false;
	float alphaOfDraw = 0.0f;

	if (randomItemAlpha > 0.0f && actualItemAlpha < 1.0f) {
		glColor4f(1.0f, 1.0f, 1.0f, randomItemAlpha);
		this->randomItemTexture->BindTexture();
		GeometryMaker::GetInstance()->DrawQuad();
		didDraw = true;
		alphaOfDraw = randomItemAlpha;
	}

	if (actualItemAlpha > 0.0f) {
		glColor4f(1.0f, 1.0f, 1.0f, actualItemAlpha);
		this->currItemFromRandomTexture->BindTexture();
		GeometryMaker::GetInstance()->DrawQuad();
		didDraw = true;
		alphaOfDraw = std::max<float>(alphaOfDraw, actualItemAlpha);
	}

	// Draw the outlines...
	if (didDraw) {
		glLineWidth(2.0f);
		glColor4f(0.0f, 0.0f, 0.0f, alphaOfDraw);
		glPolygonMode(GL_FRONT, GL_LINE);
		GeometryMaker::GetInstance()->DrawQuad();
	}

	glPopMatrix();
	glPopAttrib();

	
}