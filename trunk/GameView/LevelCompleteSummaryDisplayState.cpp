/**
 * LevelCompleteSummaryDisplayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "LevelCompleteSummaryDisplayState.h"
#include "GameDisplay.h"
#include "GameFontAssetsManager.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/StringHelper.h"
#include "../BlammoEngine/GeometryMaker.h"
#include "../GameModel/GameModel.h"
#include "../GameModel/GameProgressIO.h"
#include "../ConfigOptions.h"
#include "../ResourceManager.h"

const double LevelCompleteSummaryDisplayState::FADE_OUT_TIME                                 = 0.75;
const double LevelCompleteSummaryDisplayState::FOOTER_FLASH_TIME                             = 0.5;
const float LevelCompleteSummaryDisplayState::FOOTER_VERTICAL_PADDING                        = 20.0f;
const float LevelCompleteSummaryDisplayState::LEVEL_NAME_HORIZONTAL_PADDING                  = 20.0f;
const float LevelCompleteSummaryDisplayState::HEADER_LEVEL_NAME_VERTICAL_PADDING             = 20.0f;
const float LevelCompleteSummaryDisplayState::HEADER_INBETWEEN_VERTICAL_PADDING              = 30.0f;
const float LevelCompleteSummaryDisplayState::TOTAL_SCORE_VALUE_INBETWEEN_HORIZONTAL_PADDING = 10.0f;
const float LevelCompleteSummaryDisplayState::HEADER_SCORE_INBETWEEN_VERTICAL_PADDING        = 50.0f;
const float LevelCompleteSummaryDisplayState::SCORE_INBETWEEN_VERTICAL_PADDING               = 30.0f;
const float LevelCompleteSummaryDisplayState::FINAL_SCORE_INBETWEEN_VERTICAL_PADDING         = 40.0f;
const float LevelCompleteSummaryDisplayState::STAR_SIZE                                      = 60.0f;
const float LevelCompleteSummaryDisplayState::STAR_HORIZONTAL_GAP                            = 10.0f;
const float LevelCompleteSummaryDisplayState::SCORE_LABEL_SIDE_PADDING                       = 100.0f;

const double LevelCompleteSummaryDisplayState::POINTS_PER_SECOND                = 20000;
const double LevelCompleteSummaryDisplayState::PER_SCORE_VALUE_FADE_IN_TIME     = 0.25;

const double LevelCompleteSummaryDisplayState::SHOW_DIFFICULTY_CHOICE_PANE_TIME = 0.75;
const double LevelCompleteSummaryDisplayState::HIDE_DIFFICULTY_CHOICE_PANE_TIME = 0.75;


LevelCompleteSummaryDisplayState::LevelCompleteSummaryDisplayState(GameDisplay* display) :
DisplayState(display), waitingForKeyPress(true),
levelNameLabel(NULL), difficultyChoicePane(NULL), difficultyChoiceHandler(NULL),
levelCompleteLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge), "Level Complete!"),
pressAnyKeyLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium), "- Press Any Key to Continue -"),
maxBlocksTextLabel(NULL), itemsAcquiredTextLabel(NULL), levelTimeTextLabel(NULL),
itemsAcquiredValueLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium), ""),
maxBlocksValueLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium), ""),
levelTimeValueLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium), ""),
totalScoreLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big), "Total score:"),
scoreValueLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big), "0"),
newHighScoreLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium), "New High Score!"),
maxScoreValueWidth(0), starTexture(NULL), glowTexture(NULL), sparkleTexture(NULL), starBgRotator(90.0f, ESPParticleRotateEffector::CLOCKWISE),
starFgRotator(45.0f, ESPParticleRotateEffector::CLOCKWISE), starFgPulser(ScaleEffect(1.0f, 1.5f)), gameProgressWasSaved(false) {
    
    const Camera& camera = this->display->GetCamera();
    GameModel* gameModel = this->display->GetModel();
    assert(gameModel != NULL);

    // Save game progress
    this->gameProgressWasSaved = GameProgressIO::SaveGameProgress(gameModel);

    const Colour smallScoreLabelColour(0.15f, 0.15f, 0.15f);

    this->maxBlocksTextLabel = new TextLabel2DFixedWidth(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium),
        camera.GetWindowWidth()/2 - this->display->GetTextScalingFactor() * SCORE_LABEL_SIDE_PADDING, "Maximum consecutive blocks destroyed:");
    this->maxBlocksTextLabel->SetAlignment(TextLabel2DFixedWidth::RightAligned);
    this->maxBlocksTextLabel->SetColour(smallScoreLabelColour);
    this->maxBlocksTextLabel->SetScale(this->display->GetTextScalingFactor() * 0.8f);

    std::stringstream maxBlocksValStrStream;
    maxBlocksValStrStream << gameModel->GetMaxConsecutiveBlocksDestroyed();
    this->maxBlocksValueLabel.SetText(maxBlocksValStrStream.str());
    this->maxBlocksValueLabel.SetColour(smallScoreLabelColour);
    this->maxBlocksValueLabel.SetScale(this->display->GetTextScalingFactor() * 0.9f);

    this->itemsAcquiredTextLabel = new TextLabel2DFixedWidth(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium),
        camera.GetWindowWidth()/2 - this->display->GetTextScalingFactor() * SCORE_LABEL_SIDE_PADDING, "Number of items acquired:");
    this->itemsAcquiredTextLabel->SetAlignment(TextLabel2DFixedWidth::RightAligned);
    this->itemsAcquiredTextLabel->SetColour(smallScoreLabelColour);
    this->itemsAcquiredTextLabel->SetScale(this->display->GetTextScalingFactor() * 0.8f);

    std::stringstream itemsAcquiredValStrStream;
    int numGoodItems, numNeutralItems, numBadItems;
    gameModel->GetNumItemsAcquired(numGoodItems, numNeutralItems, numBadItems);
    itemsAcquiredValStrStream << (numGoodItems + numNeutralItems + numBadItems);
    this->itemsAcquiredValueLabel.SetText(itemsAcquiredValStrStream.str());
    this->itemsAcquiredValueLabel.SetScale(this->display->GetTextScalingFactor() * 0.9f);
    this->itemsAcquiredValueLabel.SetColour(smallScoreLabelColour);

    this->levelTimeTextLabel = new TextLabel2DFixedWidth(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium),
        camera.GetWindowWidth()/2 - this->display->GetTextScalingFactor() * SCORE_LABEL_SIDE_PADDING, "Total time:");
    this->levelTimeTextLabel->SetAlignment(TextLabel2DFixedWidth::RightAligned);
    this->levelTimeTextLabel->SetColour(smallScoreLabelColour);
    this->levelTimeTextLabel->SetScale(this->display->GetTextScalingFactor() * 0.8f);

    std::stringstream levelTimeValStrStream;
    double levelTimeInSecs = gameModel->GetLevelTimeInSeconds();
    if (levelTimeInSecs >= 3600) {
        levelTimeValStrStream << static_cast<int>(levelTimeInSecs/3600.0) << ":";
        levelTimeValStrStream << ((static_cast<int>(levelTimeInSecs) % 3600)/60) << ":";
        levelTimeValStrStream << (static_cast<int>(levelTimeInSecs) % 60);
    }
    else {
        levelTimeValStrStream << static_cast<int>(levelTimeInSecs/60.0) << ":";
        
        int seconds = static_cast<int>(levelTimeInSecs) % 60;
        if (seconds < 10) {
            levelTimeValStrStream << "0";
        }
        levelTimeValStrStream << seconds;
    }
    this->levelTimeValueLabel.SetText(levelTimeValStrStream.str());
    this->levelTimeValueLabel.SetScale(this->display->GetTextScalingFactor() * 0.9f);
    this->levelTimeValueLabel.SetColour(smallScoreLabelColour);

    this->totalScoreLabel.SetScale(this->display->GetTextScalingFactor() * 1.2f);
    this->scoreValueLabel.SetScale(this->display->GetTextScalingFactor() * 1.3f);
    this->maxTotalLabelHeight = this->totalScoreLabel.GetHeight();
    this->newHighScoreLabel.SetDropShadow(Colour(0,0,0), this->display->GetTextScalingFactor() * 0.1f);


	// Pause all game play elements in the game model
	gameModel->SetPauseState(GameModel::PausePaddle | GameModel::PauseBall);

	// Setup the fade animation to be inactive initially
	this->fadeAnimation.SetInterpolantValue(0.0f);
	this->fadeAnimation.SetLerp(0.0f, 0.0f, 0.0f, 0.0f);
	this->fadeAnimation.SetRepeat(false);

    static const double BEGIN_ANIM_END_TIME = 0.5;
    this->levelCompleteTextScaleAnimation.SetInterpolantValue(0.0f);
    this->levelCompleteTextScaleAnimation.SetLerp(0.0, BEGIN_ANIM_END_TIME, 0.01f, 1.0f);
    this->levelCompleteTextScaleAnimation.SetRepeat(false);

    static const double STAR_ANIM_TIME_LENGTH = 0.2;
    double starAnimBeginTime = BEGIN_ANIM_END_TIME;
    for (int i = 0; i < GameLevel::MAX_STARS_PER_LEVEL; i++) {
        double starAnimEndTime = starAnimBeginTime + STAR_ANIM_TIME_LENGTH;
        AnimationLerp<float>* starAnimation = new AnimationLerp<float>();
        starAnimation->SetLerp(starAnimBeginTime, starAnimEndTime, 0.01f, 1.0f);
        this->starAnimations.push_back(starAnimation);
        starAnimBeginTime = starAnimEndTime;
    }


	// Setup the label for the press any key text...
	this->pressAnyKeyLabel.SetDropShadow(Colour(0, 0, 0), this->display->GetTextScalingFactor() * 0.1f);
	this->pressAnyKeyLabel.SetScale(this->display->GetTextScalingFactor());

	// Set the footer colour flash animation
	std::vector<double> timeVals;
	timeVals.reserve(3);
	timeVals.push_back(0.0);
	timeVals.push_back(FOOTER_FLASH_TIME);
	timeVals.push_back(2 * FOOTER_FLASH_TIME);
	std::vector<Colour> colourVals;
	colourVals.reserve(3);
	colourVals.push_back(Colour(0.0f, 0.6f, 0.9f));
	colourVals.push_back(Colour(1.0f, 0.8f, 0.0f));
	colourVals.push_back(Colour(0.0f, 0.6f, 0.9f));
	this->footerColourAnimation.SetLerp(timeVals, colourVals);
	this->footerColourAnimation.SetRepeat(true);

    this->levelCompleteLabel.SetColour(Colour(1.0f, 0.8f, 0.0f));
    this->levelCompleteLabel.SetDropShadow(Colour(0, 0, 0), this->display->GetTextScalingFactor() * 0.1f);

    this->levelNameLabel = new TextLabel2DFixedWidth(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium), 
        display->GetCamera().GetWindowWidth() - 2*LEVEL_NAME_HORIZONTAL_PADDING, 
        std::string("\"") + this->display->GetModel()->GetCurrentLevel()->GetName() + std::string("\""));
    this->levelNameLabel->SetColour(Colour(0.2f, 0.6f, 1.0f));
    this->levelNameLabel->SetDropShadow(Colour(0,0,0), this->display->GetTextScalingFactor() * 0.1f);

    double nextStartTime = BEGIN_ANIM_END_TIME;
    double nextEndTime   = nextStartTime + PER_SCORE_VALUE_FADE_IN_TIME;
    maxBlocksFadeIn.SetInterpolantValue(0.0f);
    maxBlocksFadeIn.SetLerp(nextStartTime, nextEndTime, 0.0f, 1.0f);
    maxBlocksFadeIn.SetRepeat(false);

    nextStartTime = nextEndTime;
    nextEndTime  += PER_SCORE_VALUE_FADE_IN_TIME;
    numItemsFadeIn.SetInterpolantValue(0.0f);
    numItemsFadeIn.SetLerp(nextStartTime, nextEndTime, 0.0f, 1.0f);
    numItemsFadeIn.SetRepeat(false);

    nextStartTime = nextEndTime;
    nextEndTime  += PER_SCORE_VALUE_FADE_IN_TIME;
    totalTimeFadeIn.SetInterpolantValue(0.0f);
    totalTimeFadeIn.SetLerp(nextStartTime, nextEndTime, 0.0f, 1.0f);
    totalTimeFadeIn.SetRepeat(false);

    // Setup the animation for the score
    static const double START_TALLEY_TIME = nextEndTime + 0.5;
    double gameScore = static_cast<double>(gameModel->GetScore());
    double totalTime = gameScore / POINTS_PER_SECOND;
    this->scoreValueAnimation.SetInterpolantValue(0.0);
    this->scoreValueAnimation.SetLerp(START_TALLEY_TIME, START_TALLEY_TIME+totalTime, 0.0, gameScore);
    this->scoreValueAnimation.SetRepeat(false);

    // Figure out the maximum score value label width
    std::stringstream maxScoreValueStrStream;
    maxScoreValueStrStream << static_cast<int>(this->scoreValueAnimation.GetTargetValue());
    this->scoreValueLabel.SetText(maxScoreValueStrStream.str());
    this->maxScoreValueWidth = this->scoreValueLabel.GetLastRasterWidth();

    std::stringstream initScoreValueStrStream;
    maxScoreValueStrStream << static_cast<int>(this->scoreValueAnimation.GetInterpolantValue());
    this->scoreValueLabel.SetText(initScoreValueStrStream.str());
    
    const float totalScoreXSize = this->totalScoreLabel.GetLastRasterWidth();
    float totalScoreXPos = camera.GetWindowWidth()/2 - totalScoreXSize;
    float scoreStartX = -(this->maxScoreValueWidth + this->totalScoreLabel.GetLastRasterWidth() +
        2*TOTAL_SCORE_VALUE_INBETWEEN_HORIZONTAL_PADDING);
    this->totalScoreFlyInAnimation.SetInterpolantValue(scoreStartX);
    this->totalScoreFlyInAnimation.SetLerp(nextEndTime, START_TALLEY_TIME, scoreStartX, totalScoreXPos);
    this->totalScoreFlyInAnimation.SetRepeat(false);

    this->totalScoreFadeInAnimation.SetInterpolantValue(0.0f);
    this->totalScoreFadeInAnimation.SetLerp(nextEndTime, START_TALLEY_TIME, 0.0f, 1.0f);
    this->totalScoreFadeInAnimation.SetRepeat(false);

    this->newHighScoreFade.SetInterpolantValue(0.0f);
    this->newHighScoreFade.SetLerp(START_TALLEY_TIME+totalTime, START_TALLEY_TIME+totalTime+0.5, 0.0f, 1.0f);
    this->newHighScoreFade.SetRepeat(false);

    // Grab any required texture resources
    this->starTexture = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_STAR, 
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->starTexture != NULL);
    this->glowTexture = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, 
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->glowTexture != NULL);
    this->sparkleTexture = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SPARKLE, 
        Texture::Trilinear, GL_TEXTURE_2D);
    assert(this->sparkleTexture != NULL);

    // Initialize any of the shiny background emitters for stars that were acquired
    this->starBgEmitters.reserve(gameModel->GetNumStarsAwarded());
    this->starFgEmitters.reserve(gameModel->GetNumStarsAwarded());
    const Colour STAR_COLOUR_BG = 1.2f * GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR;
    const Colour STAR_COLOUR_FG = 1.5f * GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR;
    for (int i = 0; i < gameModel->GetNumStarsAwarded(); i++) {

        
        ESPPointEmitter* starFgEmitter = new ESPPointEmitter();
        starFgEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	    starFgEmitter->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	    starFgEmitter->SetParticleLife(ESPParticle::INFINITE_PARTICLE_LIFETIME);
	    starFgEmitter->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	    starFgEmitter->SetParticleAlignment(ESP::ScreenAligned);
	    starFgEmitter->SetParticleRotation(ESPInterval(0));
        starFgEmitter->SetParticleColour(ESPInterval(STAR_COLOUR_FG.R()), ESPInterval(STAR_COLOUR_FG.G()), 
            ESPInterval(STAR_COLOUR_FG.B()), ESPInterval(1));
	    starFgEmitter->SetParticleSize(ESPInterval(STAR_SIZE));
        starFgEmitter->AddEffector(&this->starFgRotator);
        starFgEmitter->AddEffector(&this->starFgPulser);
        starFgEmitter->SetParticles(1, static_cast<Texture2D*>(this->sparkleTexture));

        this->starFgEmitters.push_back(starFgEmitter);

        ESPPointEmitter* starBgEmitter = new ESPPointEmitter();
        starBgEmitter->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	    starBgEmitter->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	    starBgEmitter->SetParticleLife(ESPParticle::INFINITE_PARTICLE_LIFETIME);
	    starBgEmitter->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	    starBgEmitter->SetParticleAlignment(ESP::ScreenAligned);
	    starBgEmitter->SetParticleRotation(ESPInterval(0));
        starBgEmitter->SetParticleColour(ESPInterval(STAR_COLOUR_BG.R()), ESPInterval(STAR_COLOUR_BG.G()), 
            ESPInterval(STAR_COLOUR_BG.B()), ESPInterval(1));
	    starBgEmitter->SetParticleSize(ESPInterval(1.4f*STAR_SIZE));
        starBgEmitter->AddEffector(&this->starBgRotator);
        starBgEmitter->SetParticles(1, static_cast<Texture2D*>(this->glowTexture));

        this->starBgEmitters.push_back(starBgEmitter);
    }

    // Depending on whether the level was the tutorial level or not, we create the difficulty choice pane
    if (gameModel->IsCurrentLevelTheTutorialLevel()) {
        this->difficultyChoiceHandler = new DifficultyPaneEventHandler(this);
        this->difficultyChoicePane = new DecoratorOverlayPane(this->difficultyChoiceHandler,
            static_cast<size_t>(camera.GetWindowWidth() * 0.65f),
            GameViewConstants::GetInstance()->TUTORIAL_PANE_COLOUR);

        // Determine the suggested difficulty level based on how they did in the tutorial...
        int numStars             = gameModel->GetNumStarsAwarded();
        int numLivesLost         = gameModel->GetNumLivesLostInCurrentLevel();
        int maxConsecutiveBlocks = gameModel->GetMaxConsecutiveBlocksDestroyed();

        GameModel::Difficulty difficulty = GameModel::MediumDifficulty;
        if (numLivesLost <= 2) {
            if (numStars >= 3) {
                if (maxConsecutiveBlocks >= GameModelConstants::GetInstance()->FOUR_TIMES_MULTIPLIER_NUM_BLOCKS) {
                    if (numLivesLost <= 1) {
                        difficulty = GameModel::HardDifficulty;
                    }
                    else {
                        difficulty = GameModel::MediumDifficulty;
                    }
                }
                else {
                    difficulty = GameModel::MediumDifficulty;
                }
            }
            else {
                if (numLivesLost <= 1) {
                    difficulty = GameModel::MediumDifficulty;
                }
                else {
                    difficulty = GameModel::EasyDifficulty;
                }
            }
        }
        else {
            // They lost more than 2 lives... that's pretty bad
            difficulty = GameModel::EasyDifficulty;
        }
        std::vector<std::string> difficultyOptions = ConfigOptions::GetDifficultyItems();
        
        this->difficultyChoicePane->AddText("Based on your performance, you should try playing on a difficulty setting of...");
        this->difficultyChoicePane->AddText(difficultyOptions[static_cast<int>(difficulty)], Colour(1, 0.75f, 0.0f), 1.2f);
        this->difficultyChoicePane->AddText("You may select any difficulty you wish if you think this recommendation is lame:");

        this->difficultyChoicePane->SetSelectableOptions(difficultyOptions, static_cast<int>(difficulty));

        this->difficultyChoicePane->Show(SHOW_DIFFICULTY_CHOICE_PANE_TIME);
    }
}

LevelCompleteSummaryDisplayState::~LevelCompleteSummaryDisplayState() {
    delete this->levelNameLabel;
    this->levelNameLabel = NULL;
    delete this->maxBlocksTextLabel;
    this->maxBlocksTextLabel = NULL;
    delete this->itemsAcquiredTextLabel;
    this->itemsAcquiredTextLabel = NULL;
    delete this->levelTimeTextLabel;
    this->levelTimeTextLabel = NULL;

    bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->starTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->glowTexture);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->sparkleTexture);
    assert(success);

    for (size_t i = 0; i < this->starAnimations.size(); i++) {
        delete this->starAnimations[i];
    }
    this->starAnimations.clear();
    for (size_t i = 0; i < this->starBgEmitters.size(); i++) {
        delete this->starBgEmitters[i];
    }
    this->starBgEmitters.clear();
    for (size_t i = 0; i < this->starFgEmitters.size(); i++) {
        delete this->starFgEmitters[i];
    }
    this->starFgEmitters.clear();

    if (this->difficultyChoiceHandler != NULL) {
        delete this->difficultyChoiceHandler;
        this->difficultyChoiceHandler = NULL;

        assert(this->difficultyChoicePane != NULL);
        delete this->difficultyChoicePane;
        this->difficultyChoicePane = NULL;
    }
}

void LevelCompleteSummaryDisplayState::RenderFrame(double dT) {
    const Camera& camera = this->display->GetCamera();

	// Clear the screen to a white background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (this->difficultyChoicePane != NULL) {
        this->difficultyChoicePane->Draw(dT, camera.GetWindowWidth(), camera.GetWindowHeight());
        
        // If the difficulty pane is done, clean it up
        if (this->difficultyChoicePane->IsFinished()) {
            delete this->difficultyChoicePane;
            this->difficultyChoicePane = NULL;
            delete this->difficultyChoiceHandler;
            this->difficultyChoiceHandler = NULL;
        }
    }
    else {

        if (this->waitingForKeyPress) {
            // Set the score animation value
            this->scoreValueAnimation.Tick(dT);

            this->levelCompleteTextScaleAnimation.Tick(dT);
            this->levelCompleteLabel.SetScale(this->levelCompleteTextScaleAnimation.GetInterpolantValue());

            for (size_t i = 0; i < this->starAnimations.size(); i++) {
                this->starAnimations[i]->Tick(dT);
            }

            this->totalScoreFlyInAnimation.Tick(dT);
            this->totalScoreFadeInAnimation.Tick(dT);
            this->newHighScoreFade.Tick(dT);

            this->maxBlocksFadeIn.Tick(dT);
            this->numItemsFadeIn.Tick(dT);
            this->totalTimeFadeIn.Tick(dT);
            
            if (this->scoreValueAnimation.GetInterpolantValue() >= this->scoreValueAnimation.GetTargetValue()) {
                // Animate and draw the "Press any key..." label
                this->footerColourAnimation.Tick(dT);
                this->DrawPressAnyKeyTextFooter(camera.GetWindowWidth());
            }
        }

        long currScoreTally = static_cast<long>(this->scoreValueAnimation.GetInterpolantValue());
        std::string scoreStr = stringhelper::AddNumberCommas(currScoreTally);
        this->scoreValueLabel.SetText(scoreStr);

        Camera::PushWindowCoords();
	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();

        float yPos = camera.GetWindowHeight() - HEADER_LEVEL_NAME_VERTICAL_PADDING;
        this->DrawLevelNameLabel(yPos, camera.GetWindowWidth(), camera.GetWindowHeight());
        yPos -= (this->levelCompleteLabel.GetHeight() + HEADER_INBETWEEN_VERTICAL_PADDING + 10);
        this->DrawLevelCompleteLabel(yPos, camera.GetWindowWidth(), camera.GetWindowHeight());
        yPos -= HEADER_INBETWEEN_VERTICAL_PADDING;
        this->DrawStars(dT, yPos, camera.GetWindowWidth(), camera.GetWindowHeight());
        
        // Draw the various level score statistics
        yPos -= (STAR_SIZE + HEADER_SCORE_INBETWEEN_VERTICAL_PADDING);
        this->DrawMaxBlocksLabel(yPos, camera.GetWindowWidth());
        yPos -= (SCORE_INBETWEEN_VERTICAL_PADDING + 
            std::max<size_t>(this->maxBlocksTextLabel->GetHeight(), this->maxBlocksValueLabel.GetHeight()));
        this->DrawNumItemsLabel(yPos, camera.GetWindowWidth());
        yPos -= (SCORE_INBETWEEN_VERTICAL_PADDING + 
            std::max<size_t>(this->itemsAcquiredTextLabel->GetHeight(), this->itemsAcquiredValueLabel.GetHeight()));
        this->DrawTotalTimeLabel(yPos, camera.GetWindowWidth());
        yPos -= (this->display->GetTextScalingFactor() * FINAL_SCORE_INBETWEEN_VERTICAL_PADDING + 
            std::max<size_t>(this->levelTimeTextLabel->GetHeight(), this->levelTimeValueLabel.GetHeight()));

        // Draw the total score
        this->DrawTotalScoreLabel(yPos, camera.GetWindowWidth(), camera.GetWindowHeight());
       
        Camera::PopWindowCoords();
    }

    if (!this->waitingForKeyPress) {
        // We're no longer waiting for a key press - fade out to white and then switch to the next state

        // Begin fading out the summary screen
        bool fadeIsDone = this->fadeAnimation.Tick(dT);
        float fadeValue = this->fadeAnimation.GetInterpolantValue();

		// Draw the fade quad overlay
		glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		GeometryMaker::GetInstance()->DrawFullScreenQuad(camera.GetWindowWidth(), camera.GetWindowHeight(), 1.0f, 
                                                         ColourRGBA(1, 1, 1, fadeValue));
		glPopAttrib();

        if (fadeIsDone) {
            // If we're done fading then we can go to the next state
            // Update the game model until there's a new queued state
            while (!this->display->SetCurrentStateAsNextQueuedState()) {
                this->display->UpdateModel(dT);
            }
		    return;
        }
    }
}

void LevelCompleteSummaryDisplayState::DrawLevelNameLabel(float currYPos, float screenWidth, float screenHeight) {
    UNUSED_PARAMETER(screenHeight);

    const float horizontalLabelSize = this->levelNameLabel->GetWidth();
    this->levelNameLabel->SetTopLeftCorner((screenWidth - horizontalLabelSize) / 2.0f, currYPos);
    this->levelNameLabel->Draw();
}

void LevelCompleteSummaryDisplayState::DrawLevelCompleteLabel(float currYPos, float screenWidth, float screenHeight) {
    UNUSED_PARAMETER(screenHeight);
    
    const float horizontalLabelSize = this->levelCompleteLabel.GetLastRasterWidth();
    const float verticalLabelSize   = this->levelCompleteLabel.GetHeight();
    this->levelCompleteLabel.SetTopLeftCorner((screenWidth - horizontalLabelSize) / 2.0f, currYPos + verticalLabelSize);
    this->levelCompleteLabel.Draw();
}

void LevelCompleteSummaryDisplayState::DrawStars(double dT, float currYPos, float screenWidth, float screenHeight) {
    UNUSED_PARAMETER(screenHeight);

    GameModel* gameModel = this->display->GetModel();

    glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT | GL_ENABLE_BIT);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    this->starTexture->BindTexture();
    
    float currX = screenWidth/2 - ((STAR_SIZE*GameLevel::MAX_STARS_PER_LEVEL + STAR_HORIZONTAL_GAP*(GameLevel::MAX_STARS_PER_LEVEL-1))/2);
    currX += STAR_SIZE/2;
    currYPos -= STAR_SIZE/2;

    Colour starColour;
    for (int i = 0; i < GameLevel::MAX_STARS_PER_LEVEL; i++) {
        
        AnimationLerp<float>* starAnimation = this->starAnimations[i];
        if (i < gameModel->GetNumStarsAwarded()) {
            starColour = GameViewConstants::GetInstance()->ACTIVE_POINT_STAR_COLOUR;

            ESPPointEmitter* starBGEmitter = this->starBgEmitters[i];
            starBGEmitter->SetParticleAlpha(ESPInterval(0.75f*starAnimation->GetInterpolantValue()));
            starBGEmitter->OverwriteEmittedPosition(Point3D(currX, currYPos, 0));
            starBGEmitter->Tick(dT);
            starBGEmitter->Draw(this->display->GetCamera());
        }
        else {
            starColour = GameViewConstants::GetInstance()->INACTIVE_POINT_STAR_COLOUR;
        }

        glColor4f(starColour.R(), starColour.G(), starColour.B(), 1.0f);
       
        glPushMatrix();
        glTranslatef(currX, currYPos, 0);
        glScalef(STAR_SIZE*starAnimation->GetInterpolantValue(), STAR_SIZE*starAnimation->GetInterpolantValue(), 1);
        GeometryMaker::GetInstance()->DrawQuad();
        glPopMatrix();

        if (i < gameModel->GetNumStarsAwarded()) {
            ESPPointEmitter* starFGEmitter = this->starFgEmitters[i];
            starFGEmitter->SetParticleAlpha(ESPInterval(0.85f*starAnimation->GetInterpolantValue()));
            starFGEmitter->OverwriteEmittedPosition(Point3D(currX + STAR_SIZE/7, currYPos + STAR_SIZE/7, 0));
            starFGEmitter->Tick(dT);
            starFGEmitter->Draw(this->display->GetCamera());
        }

        currX += STAR_SIZE + STAR_HORIZONTAL_GAP;
    }

    glPopAttrib();

    debug_opengl_state();
}

void LevelCompleteSummaryDisplayState::DrawMaxBlocksLabel(float currYPos, float screenWidth) {
    
    float currX = screenWidth/2 - this->maxBlocksTextLabel->GetFixedWidth(); 
    float scoreValueX = screenWidth/2 + 2*TOTAL_SCORE_VALUE_INBETWEEN_HORIZONTAL_PADDING;

    this->maxBlocksTextLabel->SetTopLeftCorner(currX, currYPos);
    this->maxBlocksValueLabel.SetTopLeftCorner(scoreValueX, currYPos -
        this->maxBlocksTextLabel->GetHeight() + this->maxBlocksValueLabel.GetHeight());

    this->maxBlocksTextLabel->SetAlpha(this->maxBlocksFadeIn.GetInterpolantValue());
    this->maxBlocksValueLabel.SetAlpha(this->maxBlocksFadeIn.GetInterpolantValue());

    this->maxBlocksTextLabel->Draw();
    this->maxBlocksValueLabel.Draw();
}

void LevelCompleteSummaryDisplayState::DrawNumItemsLabel(float currYPos, float screenWidth) {
    float currX = screenWidth/2 - this->itemsAcquiredTextLabel->GetFixedWidth(); 
    float scoreValueX = screenWidth/2 + 2*TOTAL_SCORE_VALUE_INBETWEEN_HORIZONTAL_PADDING;

    this->itemsAcquiredTextLabel->SetTopLeftCorner(currX, currYPos);
    this->itemsAcquiredValueLabel.SetTopLeftCorner(scoreValueX, currYPos - 
        this->itemsAcquiredTextLabel->GetHeight() + this->itemsAcquiredValueLabel.GetHeight());

    this->itemsAcquiredTextLabel->SetAlpha(this->numItemsFadeIn.GetInterpolantValue());
    this->itemsAcquiredValueLabel.SetAlpha(this->numItemsFadeIn.GetInterpolantValue());

    this->itemsAcquiredTextLabel->Draw();
    this->itemsAcquiredValueLabel.Draw();
}

void LevelCompleteSummaryDisplayState::DrawTotalTimeLabel(float currYPos, float screenWidth) {
    float currX = screenWidth/2 - this->levelTimeTextLabel->GetFixedWidth(); 
    float scoreValueX = screenWidth/2 + 2*TOTAL_SCORE_VALUE_INBETWEEN_HORIZONTAL_PADDING;

    this->levelTimeTextLabel->SetTopLeftCorner(currX, currYPos);
    this->levelTimeValueLabel.SetTopLeftCorner(scoreValueX, currYPos - 
        this->levelTimeTextLabel->GetHeight() + this->levelTimeValueLabel.GetHeight());

    this->levelTimeTextLabel->SetAlpha(this->totalTimeFadeIn.GetInterpolantValue());
    this->levelTimeValueLabel.SetAlpha(this->totalTimeFadeIn.GetInterpolantValue());

    this->levelTimeTextLabel->Draw();
    this->levelTimeValueLabel.Draw();
}

void LevelCompleteSummaryDisplayState::DrawTotalScoreLabel(float currYPos, float screenWidth, float screenHeight) {
    UNUSED_PARAMETER(screenHeight);
    UNUSED_PARAMETER(screenWidth);

    float currX = this->totalScoreFlyInAnimation.GetInterpolantValue();
    float scoreValueX = currX + this->totalScoreLabel.GetLastRasterWidth() + 
        2*TOTAL_SCORE_VALUE_INBETWEEN_HORIZONTAL_PADDING;

    this->totalScoreLabel.SetTopLeftCorner(currX, currYPos);
    this->scoreValueLabel.SetTopLeftCorner(scoreValueX, currYPos);

    this->totalScoreLabel.SetAlpha(this->totalScoreFadeInAnimation.GetInterpolantValue());
    this->scoreValueLabel.SetAlpha(this->totalScoreFadeInAnimation.GetInterpolantValue());

    this->totalScoreLabel.Draw();
    this->scoreValueLabel.Draw();

    GameModel* gameModel = this->display->GetModel();
    GameLevel* gameLevel = gameModel->GetCurrentLevel();

    // Check to see if the player has a new high score
    if (gameLevel->GetHasNewHighScore()) {
        this->newHighScoreLabel.SetColour(this->footerColourAnimation.GetInterpolantValue());
        this->newHighScoreLabel.SetAlpha(this->newHighScoreFade.GetInterpolantValue());
        this->newHighScoreLabel.SetTopLeftCorner(scoreValueX + 3, currYPos - this->scoreValueLabel.GetHeight() - 10);
        this->newHighScoreLabel.Draw();
    }
}

void LevelCompleteSummaryDisplayState::DrawPressAnyKeyTextFooter(float screenWidth) {
	const float horizontalLabelSize = this->pressAnyKeyLabel.GetLastRasterWidth();

	this->pressAnyKeyLabel.SetColour(this->footerColourAnimation.GetInterpolantValue());
	this->pressAnyKeyLabel.SetTopLeftCorner((screenWidth - horizontalLabelSize) / 2.0f, 
        FOOTER_VERTICAL_PADDING + this->pressAnyKeyLabel.GetHeight());

	this->pressAnyKeyLabel.Draw();
}

void LevelCompleteSummaryDisplayState::AnyKeyWasPressed() {
	if (this->waitingForKeyPress) {

        if (this->scoreValueAnimation.GetInterpolantValue() >= this->scoreValueAnimation.GetTargetValue()) {
            // Start the fade out animation - the user wants to start playing!
		    this->fadeAnimation.SetLerp(LevelCompleteSummaryDisplayState::FADE_OUT_TIME, 1.0f);
            waitingForKeyPress = false;
        }
        else {
            // Automatically finish the score tally and other animations
            this->scoreValueAnimation.SetInterpolantValue(this->scoreValueAnimation.GetTargetValue());
            this->scoreValueAnimation.ClearLerp();

            this->levelCompleteTextScaleAnimation.SetInterpolantValue(this->levelCompleteTextScaleAnimation.GetTargetValue());
            this->levelCompleteTextScaleAnimation.ClearLerp();

            for (size_t i = 0; i < this->starAnimations.size(); i++) {
                this->starAnimations[i]->SetInterpolantValue(this->starAnimations[i]->GetTargetValue());
                this->starAnimations[i]->ClearLerp();
            }

            this->totalScoreFlyInAnimation.SetInterpolantValue(this->totalScoreFlyInAnimation.GetTargetValue());
            this->totalScoreFlyInAnimation.ClearLerp();

            this->totalScoreFadeInAnimation.SetInterpolantValue(this->totalScoreFadeInAnimation.GetTargetValue());
            this->totalScoreFadeInAnimation.ClearLerp();

            this->newHighScoreFade.SetInterpolantValue(this->newHighScoreFade.GetTargetValue());
            this->newHighScoreFade.ClearLerp();

            this->maxBlocksFadeIn.SetInterpolantValue(this->maxBlocksFadeIn.GetTargetValue());
            this->maxBlocksFadeIn.ClearLerp();

            this->numItemsFadeIn.SetInterpolantValue(this->maxBlocksFadeIn.GetTargetValue());
            this->numItemsFadeIn.ClearLerp();

            this->totalTimeFadeIn.SetInterpolantValue(this->maxBlocksFadeIn.GetTargetValue());
            this->totalTimeFadeIn.ClearLerp();
        }
	}
}

// Callback function for when an option is selected in the difficulty pane
void LevelCompleteSummaryDisplayState::DifficultyPaneEventHandler::OptionSelected(const std::string& optionText) {
    std::vector<std::string> difficultyOptions = ConfigOptions::GetDifficultyItems();
    for (int i = 0; i < static_cast<int>(difficultyOptions.size()); i++) {
        
        if (optionText.compare(difficultyOptions[i]) == 0) {
            // Set the difficulty to the one selected...
            ConfigOptions cfgOptions = ResourceManager::ReadConfigurationOptions(true);
           
            GameModel::Difficulty difficultyToSet = static_cast<GameModel::Difficulty>(i);
            cfgOptions.SetDifficulty(difficultyToSet);
            this->summaryState->display->GetModel()->SetDifficulty(difficultyToSet);

            ResourceManager::GetInstance()->WriteConfigurationOptionsToFile(cfgOptions);
        }
    }
}

void LevelCompleteSummaryDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton) {
    if (this->difficultyChoicePane != NULL) {
        this->difficultyChoicePane->ButtonPressed(pressedButton);
        
        if (this->difficultyChoicePane->IsOptionSelectedAndActive()) {
            this->difficultyChoicePane->Hide(HIDE_DIFFICULTY_CHOICE_PANE_TIME);
        }

    }
    else {
        this->AnyKeyWasPressed();
    }
}

void LevelCompleteSummaryDisplayState::MousePressed(const GameControl::MouseButton& pressedButton) {
	UNUSED_PARAMETER(pressedButton);
    if (this->difficultyChoicePane == NULL) {
        this->AnyKeyWasPressed();
    }
}