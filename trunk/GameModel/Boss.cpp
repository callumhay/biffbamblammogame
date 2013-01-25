/**
 * Boss.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "Boss.h"
#include "AbstractBossBodyPart.h"
#include "BossBodyPart.h"
#include "BossWeakpoint.h"
#include "BossAIState.h"
#include "ClassicalBoss.h"

const double Boss::WAIT_BEFORE_FADE_TO_BLACK_FINAL_DEAD_BODY_PART_TIME = 1.5;
const double Boss::FADE_TO_BLACK_FINAL_DEAD_BODY_PART_TIME = TOTAL_DEATH_ANIM_TIME / 1.75;
const double Boss::TOTAL_DEATH_ANIM_TIME = 6.5;

Boss::Boss() : currAIState(NULL), nextAIState(NULL), root(NULL), 
deadPartsRoot(NULL), alivePartsRoot(NULL), isBossDeadAndLevelCompleted(false)  {
}

Boss::~Boss() {
    // Clean up the body parts
    for (std::vector<AbstractBossBodyPart*>::iterator iter = this->bodyParts.begin();
         iter != this->bodyParts.end(); ++iter) {

        AbstractBossBodyPart* part = *iter;
        delete part;
        part = NULL;
    }
    this->bodyParts.clear();
    this->root = NULL;

	// Delete the state
    if (this->currAIState != NULL) {
	    delete this->currAIState;
	    this->currAIState = NULL;
    }
    if (this->nextAIState != NULL) {
        delete this->nextAIState;
        this->nextAIState = NULL;
    }
}

/**
 * Static factory function for building boss types based on a given world style.
 */
Boss* Boss::BuildStyleBoss(const GameWorld::WorldStyle& style) {
    Boss* boss = NULL;
    switch (style) {
        case GameWorld::Classical:
            boss = new ClassicalBoss();
            break;
            
        case GameWorld::Deco:
            // TODO

        case GameWorld::Futurism:
            // TODO

        default:
            assert(false);
            break;
    }
    if (boss != NULL) {
        boss->Init();
    }

    return boss;
}

Collision::AABB2D Boss::GenerateDyingAABB() const {
    return this->currAIState->GenerateDyingAABB();
}

void Boss::Tick(double dT, GameModel* gameModel) {
    // Tick the body parts
    this->root->Tick(dT);

    // Tick the current state of the boss
    if (this->currAIState != NULL) {
        this->currAIState->Tick(dT, gameModel);
    }

    // Update the AI state if necessary
    this->UpdateAIState();
}

/**
 * Queries for whether the boss is completely dead or not.
 * Returns: true if dead, false if not.
 */
bool Boss::GetIsStateMachineFinished() const {
    if (this->currAIState != NULL) {
        return this->currAIState->IsStateMachineFinished();
    }
    return false;
}

void Boss::CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart) {
    ball.SetLastThingCollidedWith(collisionPart);

    // NOTE: Make sure the part is collided with FIRST - it informs the current state
    collisionPart->CollisionOccurred(gameModel, ball);
    this->currAIState->CollisionOccurred(gameModel, ball, collisionPart);
}

void Boss::CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart) {
    collisionPart->CollisionOccurred(gameModel, projectile);
    projectile->SetLastThingCollidedWith(collisionPart);
    this->currAIState->CollisionOccurred(gameModel, projectile, collisionPart);
}

void Boss::CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle, BossBodyPart* collisionPart) {
    collisionPart->CollisionOccurred(gameModel, paddle);
    this->currAIState->CollisionOccurred(gameModel, paddle, collisionPart);
}

bool Boss::CanHurtPaddleWithBody() const {
    if (this->currAIState != NULL) {
        return this->currAIState->CanHurtPaddleWithBody();
    }
    return false;
}

AnimationMultiLerp<ColourRGBA> Boss::BuildBossHurtAndInvulnerableColourAnim() {
    static const int NUM_FLASHES = 20;
    
    std::vector<double> timeValues;
    timeValues.reserve(2*NUM_FLASHES + 1);
    std::vector<ColourRGBA> colourValues;
    colourValues.reserve(timeValues.size());
    
    double timeInc = BossWeakpoint::INVULNERABLE_TIME_IN_SECS / static_cast<double>(2*NUM_FLASHES);
    double timeCount = 0.0;
    
    for (int i = 0; i <= 2*NUM_FLASHES; i++) {
        timeValues.push_back(timeCount);
        timeCount += timeInc;
    }
    for (int i = 0; i < NUM_FLASHES; i++) {
        colourValues.push_back(ColourRGBA(1.0f, 0.25f, 0.25f, 1.0f));
        colourValues.push_back(ColourRGBA(1.0f, 0.9f, 0.9f, 0.5f));
    }
    colourValues.push_back(ColourRGBA(1.0f, 1.0f, 1.0f, 1.0f));

    AnimationMultiLerp<ColourRGBA> hurtColourAnim;
    hurtColourAnim.SetLerp(timeValues, colourValues);
    hurtColourAnim.SetRepeat(false);

    return hurtColourAnim;
}

AnimationMultiLerp<ColourRGBA> Boss::BuildBossHurtFlashAndFadeAnim(double totalAnimTime) {
    const double FIRST_FADE_OUT_TIME = 0.5;
    const double NUM_FLASHES = 25 * totalAnimTime / 4.0;
    const double FLASH_TIME_INC = (totalAnimTime - FIRST_FADE_OUT_TIME) / (2*NUM_FLASHES + 1);

    std::vector<double> timeValues;
    timeValues.reserve(2 + 2*NUM_FLASHES + 1);
    timeValues.push_back(0.0);
    timeValues.push_back(FIRST_FADE_OUT_TIME);
    for (int i = 0; i <= 2*NUM_FLASHES; i++) {
        timeValues.push_back(timeValues.back() + FLASH_TIME_INC);
    }

    static const float FIRST_ALPHA_FADE_OUT_VALUE  = 0.5f;
    static const float SECOND_ALPHA_FADE_OUT_VALUE = 0.25f;
    std::vector<ColourRGBA> alphaValues;
    alphaValues.reserve(timeValues.size());
    alphaValues.push_back(ColourRGBA(1,1,1,1));
    alphaValues.push_back(ColourRGBA(1,1,1,FIRST_ALPHA_FADE_OUT_VALUE));
    for (int i = 0; i < NUM_FLASHES; i++) {
        alphaValues.push_back(ColourRGBA(1.0f, 0.0f, 0.0f, SECOND_ALPHA_FADE_OUT_VALUE));
        alphaValues.push_back(ColourRGBA(1.0f, 0.8f, 0.8f, FIRST_ALPHA_FADE_OUT_VALUE));
    }
    alphaValues.push_back(ColourRGBA(0.0f, 0.0f, 0.0f, 0.0f));

    AnimationMultiLerp<ColourRGBA> anim;
    anim.SetLerp(timeValues, alphaValues);
    anim.SetRepeat(false);

    return anim;
}

AnimationMultiLerp<ColourRGBA> Boss::BuildBossFinalDeathFlashAnim() {
    std::vector<double> timeValues;
    timeValues.reserve(4);
    timeValues.push_back(0.0);
    timeValues.push_back(WAIT_BEFORE_FADE_TO_BLACK_FINAL_DEAD_BODY_PART_TIME);
    timeValues.push_back(FADE_TO_BLACK_FINAL_DEAD_BODY_PART_TIME);
    timeValues.push_back(TOTAL_DEATH_ANIM_TIME);

    std::vector<ColourRGBA> colourValues;
    colourValues.reserve(timeValues.size());
    colourValues.push_back(ColourRGBA(1,1,1,1));
    colourValues.push_back(ColourRGBA(1,1,1,1));
    colourValues.push_back(ColourRGBA(0,0,0,1));
    colourValues.push_back(ColourRGBA(0,0,0,0));

    AnimationMultiLerp<ColourRGBA> anim;
    anim.SetLerp(timeValues, colourValues);
    anim.SetRepeat(false);

    return anim;
}

AnimationMultiLerp<ColourRGBA> Boss::BuildBossAngryFlashAnim() {
    static const double COLOUR_FLASH_TIME = 0.2;
    
    std::vector<double> timeVals;
    timeVals.reserve(5);
    timeVals.push_back(0.0);
    timeVals.push_back(COLOUR_FLASH_TIME);
    timeVals.push_back(2*COLOUR_FLASH_TIME);
    timeVals.push_back(3*COLOUR_FLASH_TIME);
    timeVals.push_back(4*COLOUR_FLASH_TIME);

    std::vector<ColourRGBA> colourVals;
    colourVals.reserve(timeVals.size());
    colourVals.push_back(ColourRGBA(1,1,1,1));
    colourVals.push_back(ColourRGBA(1,0,0,1));
    colourVals.push_back(ColourRGBA(1,1,0,1));
    colourVals.push_back(ColourRGBA(1,0,0,1));
    colourVals.push_back(ColourRGBA(1,1,1,1));

    AnimationMultiLerp<ColourRGBA> angryColourAnim;
    angryColourAnim.SetLerp(timeVals, colourVals);
    angryColourAnim.SetRepeat(true);
    return angryColourAnim;
}

AnimationMultiLerp<Vector3D> Boss::BuildBossAngryShakeAnim(float shakeMagnitude) {
    static const int NUM_SHAKES = 15;
    static const double SHAKE_INC_TIME = 0.03;
    
    std::vector<double> timeVals;
    timeVals.reserve(1 + NUM_SHAKES + 1);
    timeVals.push_back(0.0);
    for (int i = 0; i <= NUM_SHAKES*2; i++) {
        timeVals.push_back(timeVals.back() + SHAKE_INC_TIME);
    }

    std::vector<Vector3D> moveVals;
    moveVals.reserve(timeVals.size());
    moveVals.push_back(Vector3D(0,0,0));
    for (int i = 0; i < NUM_SHAKES; i++) {
        float randomNum1 = Randomizer::GetInstance()->RandomNumNegOneToOne() * shakeMagnitude;
        float randomNum2 = Randomizer::GetInstance()->RandomNumNegOneToOne() * shakeMagnitude;
        moveVals.push_back(Vector3D(randomNum1, randomNum2, 0));
        moveVals.push_back(Vector3D(-randomNum1, -randomNum2, 0));
    }
    moveVals.push_back(Vector3D(0.0f, 0.0f, 0.0f));
    
    AnimationMultiLerp<Vector3D> angryMoveAnim;
    angryMoveAnim.SetLerp(timeVals, moveVals);
    angryMoveAnim.SetRepeat(false);
    return angryMoveAnim;
}

AnimationMultiLerp<Vector3D> Boss::BuildBossFinalDeathShakeAnim(float shakeMagnitude) {
    AnimationMultiLerp<Vector3D> anim = Boss::BuildBossAngryShakeAnim(shakeMagnitude);
    anim.SetRepeat(true);
    return anim;
}

AnimationMultiLerp<Vector3D> Boss::BuildLimbFallOffTranslationAnim(double totalAnimTime, float xDist, float yDist) {
    std::vector<double> timeValues;
    timeValues.reserve(2);
    timeValues.push_back(0.0);
    timeValues.push_back(totalAnimTime);

    std::vector<Vector3D> moveValues;
    moveValues.reserve(timeValues.size());
    moveValues.push_back(Vector3D(0,0,0));
    moveValues.push_back(Vector3D(xDist, yDist, 0.0f));

    AnimationMultiLerp<Vector3D> limbFallOffTransAnim;
    limbFallOffTransAnim.SetLerp(timeValues, moveValues);
    limbFallOffTransAnim.SetRepeat(false);

    return limbFallOffTransAnim;
}

AnimationMultiLerp<float> Boss::BuildLimbFallOffZRotationAnim(double totalAnimTime, float rotAmtInDegs) {
    std::vector<double> timeValues;
    timeValues.reserve(3);
    timeValues.push_back(0.0);
    timeValues.push_back(totalAnimTime);

    std::vector<float> rotationValues;
    rotationValues.reserve(timeValues.size());
    rotationValues.push_back(0.0f);
    rotationValues.push_back(rotAmtInDegs);

    AnimationMultiLerp<float> limbFallOffRotAnim;
    limbFallOffRotAnim.SetLerp(timeValues, rotationValues);
    limbFallOffRotAnim.SetRepeat(false);

    return limbFallOffRotAnim;
}

void Boss::SetNextAIState(BossAIState* nextState) {
    if (this->nextAIState != NULL) {
    	delete this->nextAIState;
    }
    this->nextAIState = nextState;
}

void Boss::UpdateAIState() {
	if (this->nextAIState != NULL) {
		this->SetCurrentAIStateImmediately(this->nextAIState);
		this->nextAIState = NULL;
	}
}

void Boss::SetCurrentAIStateImmediately(BossAIState* newState) {
	assert(newState != NULL);
	if (this->currAIState != NULL) {
		delete this->currAIState;
	}
	this->currAIState = newState;
}

#ifdef _DEBUG
void Boss::DebugDraw() const {
    if (this->root != NULL) {
        this->root->DebugDraw();
    }
}
#endif