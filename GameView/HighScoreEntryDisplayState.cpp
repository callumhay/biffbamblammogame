
#include "HighScoreEntryDisplayState.h"
#include "MainMenuDisplayState.h"
#include "GameDisplay.h"
#include "GameFontAssetsManager.h"
#include "MenuBackgroundRenderer.h"
#include "GameAssets.h"
#include "GameFBOAssets.h"

#include "../BlammoEngine/FBObj.h"
#include "../BlammoEngine/StringHelper.h"

#include "../GameModel/ArcadeLeaderboard.h"

const int HighScoreEntryDisplayState::TITLE_Y_PADDING = 50;
const int HighScoreEntryDisplayState::TITLE_BOTTOM_TO_SCORE_RANKING_Y_SPACE = 100;
const int HighScoreEntryDisplayState::SCORE_RANKING_BOTTOM_TO_SCORE_Y_SPACE = 50;

const float HighScoreEntryDisplayState::NAME_CHAR_X_GAP = 0.1f;

const double HighScoreEntryDisplayState::TIME_TO_WAIT_UNTIL_FINISHED = 10.0;

#define ENTRY_SIZE GameFontAssetsManager::Big
#define SCORE_NAME_SCALE_MULTIPLIER 1.20

HighScoreEntryDisplayState::HighScoreEntryDisplayState(GameDisplay* display, 
                                                       const DisplayStateInfo& info) : 
DisplayState(display), bbbLogoTex(NULL), playerEntryName(""), timeWaitedInS(0.0),
playerLeaderboardIdx(-1), leaderboard(NULL), largestEntryCharWidth(0), largestScoreWidth(0),
entryCharLabel(GameFontAssetsManager::GetInstance()->GetFont(
               GameFontAssetsManager::AllPurpose, ENTRY_SIZE), "") {

    float textScaleFactor = this->display->GetTextScalingFactor();

    {
        this->largestEntryCharWidth = 0;
        this->entryCharacters.reserve(26+10+4);

#define ENTRY_CHAR(c) { TextLabel2D* label = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, ENTRY_SIZE), ""); \
    std::string temp; temp += c; label->SetText(temp); \
    label->SetScale(SCORE_NAME_SCALE_MULTIPLIER*textScaleFactor); \
    label->SetColour(Colour(1,1,1)); \
    this->entryCharacters.push_back(c); \
    this->entryCharacterMap.insert(std::make_pair(c, label)); \
    float w = label->GetLastRasterWidth(); if (w > this->largestEntryCharWidth) { this->largestEntryCharWidth = w; } }

        // Initialize all the entry characters
        // A-Z
        for (int i = 0; i < 26; i++) {
            ENTRY_CHAR(static_cast<char>(i+65));
        }
        // 0-9
        for (int i = 0; i < 10; i++) {
            ENTRY_CHAR(static_cast<char>(i+48));
        }

        // Punctuation/Misc.
        ENTRY_CHAR('!');
        ENTRY_CHAR('?');
        ENTRY_CHAR('.');
        ENTRY_CHAR(' ');

#undef ENTRY_CHAR

        this->selectedEntryCharIdx = 0;
    }
    
    this->entryCharLabel.SetScale(SCORE_NAME_SCALE_MULTIPLIER*textScaleFactor);

    static const double TIME_BETWEEN_TITLE_FADEIN = 0.5;

    {
        const double BLINK_TIME = 0.1;
        const double BLINK_HOLD_TIME = 0.5;
        std::vector<double> times;
        std::vector<float> intensities;
        times.push_back(0.0); 
        times.push_back(times.back() + BLINK_TIME); 
        times.push_back(times.back() + BLINK_HOLD_TIME);
        times.push_back(times.back() + BLINK_TIME);
        times.push_back(times.back() + BLINK_HOLD_TIME/1.75);

        intensities.push_back(0.0);
        intensities.push_back(1.0);
        intensities.push_back(1.0);
        intensities.push_back(0.0);
        intensities.push_back(0.0);

        this->cursorAnim.SetLerp(times, intensities);
        this->cursorAnim.SetRepeat(true);
        this->cursorAnim.SetInterpolantValue(0.0f);
    }
    this->arcadeFlashAnim.ClearLerp();
    this->arcadeFlashAnim.SetInterpolantValue(0.0f);

    double lastFadeInTime = 0.0;
    this->logoFadeAnim.SetLerp(lastFadeInTime, lastFadeInTime + TIME_BETWEEN_TITLE_FADEIN, 0.0f, 1.0f);
    this->logoFadeAnim.SetRepeat(false);
    lastFadeInTime = this->logoFadeAnim.GetFinalTime();

    this->bbbLogoTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BBB_LOGO, Texture::Trilinear));
    assert(this->bbbLogoTex != NULL);

    static const double TIME_BETWEEN_LABEL_FADEINS = 0.15;
    this->labelFadeAnims.reserve(1 + NUM_NAME_CHARACTERS);

    this->scoreRankingLabel = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(
        GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big), "Score Ranking");
    this->scoreRankingLabel.SetScale(textScaleFactor*1.1);
    this->scoreRankingLabel.SetColour(Colour(1,1,1), 0.0f);
    AnimationLerp<float> temp(0.0f);
    
    temp.SetLerp(lastFadeInTime, lastFadeInTime + TIME_BETWEEN_LABEL_FADEINS, 0.0, 1.0);
    temp.SetRepeat(false);
    this->labelFadeAnims.push_back(temp);
    lastFadeInTime = temp.GetFinalTime();

    // Gather the existing high scores, make labels for them...
    const GameModel* model = this->display->GetModel();
    assert(model != NULL);
    
    this->leaderboard = new ArcadeLeaderboard(ResourceManager::ReadLeaderboard(false, *model));

    // Leaderboard entries are always stored highest to lowest... on the screen this will appear
    // from top to bottom, respectively
    this->largestScoreWidth = 0;
    const int numEntries = this->leaderboard->GetNumEntries();
    for (int i = 0; i < numEntries; i++) {
        LeaderboardEntry& currEntry = this->leaderboard->GetEntry(i);

        TextLabel2D* scoreLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(
            GameFontAssetsManager::AllPurpose, ENTRY_SIZE), "");

        if (currEntry.score >= info.GetHighScore()) {
            scoreLabel->SetText(stringhelper::AddNumberCommas(currEntry.score));
        }
        else if (this->playerLeaderboardIdx < 0) {
            // The player has placed on the leaderboard:
            // i is the index on the leaderboard where the player will be able to enter their name
            this->playerLeaderboardIdx = i;
            currEntry.playerName = "";
            currEntry.score = info.GetHighScore();
            scoreLabel->SetText(stringhelper::AddNumberCommas(info.GetHighScore()));
        }

        scoreLabel->SetScale(textScaleFactor*SCORE_NAME_SCALE_MULTIPLIER);
        scoreLabel->SetColour(Colour(1,1,1), 0.0f);

        float scoreLblWidth = scoreLabel->GetLastRasterWidth();
        if (scoreLblWidth > this->largestScoreWidth) {
            this->largestScoreWidth = scoreLblWidth;
        }

        temp.SetLerp(lastFadeInTime, lastFadeInTime + TIME_BETWEEN_LABEL_FADEINS, 0.0, 1.0);
        temp.SetRepeat(false);
        lastFadeInTime = temp.GetFinalTime();
        this->labelFadeAnims.push_back(temp);

        this->leaderboardScoreLabels.push_back(scoreLabel);
    }

    this->SetState(Waiting);
}

HighScoreEntryDisplayState::~HighScoreEntryDisplayState() {
    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->bbbLogoTex);
    assert(success);
    UNUSED_VARIABLE(success);

    for (int i = 0; i < static_cast<int>(this->leaderboardScoreLabels.size()); i++) {
        delete this->leaderboardScoreLabels[i];
    }
    this->leaderboardScoreLabels.clear();
    for (std::map<char, TextLabel2D*>::iterator iter = this->entryCharacterMap.begin(); 
         iter != this->entryCharacterMap.end(); ++iter) {

        delete iter->second;
    }
    this->entryCharacterMap.clear();

    delete this->leaderboard;
    this->leaderboard = NULL;
}

void HighScoreEntryDisplayState::RenderFrame(double dT) {

    Camera& camera = this->display->GetCamera();
    Vector3D shakeAmt = camera.TickAndGetCameraShakeTransform(dT);
    float scaleFactor = GameDisplay::GetTextScalingFactor();

    GameAssets* assets       = this->display->GetAssets();
    GameFBOAssets* fboAssets = assets->GetFBOAssets();

    FBObj* fbo = fboAssets->GetFullSceneFBO();

    MenuBackgroundRenderer* bgRenderer = this->display->GetMenuBGRenderer();
    bgRenderer->SetStarMoveFreq(MenuBackgroundRenderer::FAST_MOVE_FREQUENCY);

    const int DISPLAY_WIDTH	 = Camera::GetWindowWidth();
    const int HALF_DISPLAY_WIDTH = DISPLAY_WIDTH / 2.0f;
    const int DISPLAY_HEIGHT = Camera::GetWindowHeight();

    fbo->BindFBObj();

    // Render background stuffs (behind the menu)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Draw the background...
    bgRenderer->DrawShakeBG(camera, shakeAmt[0], shakeAmt[1]);

    const float VERT_DIST_FROM_EDGE = 20 * scaleFactor;
    const float logoXSize = 512.0f * scaleFactor;
    const float logoYSize = 256.0f * scaleFactor;
    const float logoXSizeDiv2 = logoXSize / 2.0f;
    const float logoYSizeDiv2 = logoYSize / 2.0f;
    float currY = DISPLAY_HEIGHT - logoYSize - VERT_DIST_FROM_EDGE;

    // Draw labels
    int labelIdx = 0;
    bool finishedLabelFading = true;
    {
        const float GAME_TITLE_TO_TITLE_LABEL_V_GAP = 50.0f*scaleFactor;
        currY -= GAME_TITLE_TO_TITLE_LABEL_V_GAP;
        
        AnimationLerp<float>& fadeAnim = this->labelFadeAnims[labelIdx++];
        finishedLabelFading &= fadeAnim.Tick(dT);
        
        this->scoreRankingLabel.SetAlpha(fadeAnim.GetInterpolantValue());
        this->scoreRankingLabel.SetCenter(Point2D(HALF_DISPLAY_WIDTH, currY));
        this->scoreRankingLabel.Draw();

        const float TITLE_LABEL_TO_FIRST_SCORE_V_GAP = 50.0f*scaleFactor;
        currY -= (this->scoreRankingLabel.GetHeight()/2.0f + TITLE_LABEL_TO_FIRST_SCORE_V_GAP);
    }
    
    {
        const float SCORE_NAME_X_GAP = 70.0f * scaleFactor;
        const float PLACING_NO_TO_NAME_X_GAP = 20.0f *scaleFactor;
        const float HALF_SCORE_NAME_X_GAP = SCORE_NAME_X_GAP / 2.0f;
        const float LEADERBOARD_ENTRY_Y_GAP = 35.0f * scaleFactor;

        // Draw all the leaderboard entries...
        const int numLeaderboardLbls = static_cast<int>(this->leaderboardScoreLabels.size());

        Colour labelColour(1,1,1);
        for (int i = 0; i < numLeaderboardLbls; i++) {
            const LeaderboardEntry& entry = this->leaderboard->GetEntry(i);
            
            AnimationLerp<float>& fadeAnim = this->labelFadeAnims[labelIdx++];
            finishedLabelFading &= fadeAnim.Tick(dT);
            float labelAlpha = fadeAnim.GetInterpolantValue();

            TextLabel2D* currScoreLbl = this->leaderboardScoreLabels[i];

            std::stringstream ss;
            ss << (i+1) << ".";
            TextLabel2D tempNumber(this->entryCharLabel.GetFont(), ss.str());

            float currX = HALF_DISPLAY_WIDTH - (HALF_SCORE_NAME_X_GAP + this->largestEntryCharWidth/2.0f + 
                PLACING_NO_TO_NAME_X_GAP + NUM_NAME_CHARACTERS*this->largestEntryCharWidth + 
                (NUM_NAME_CHARACTERS-1)*NAME_CHAR_X_GAP);

            tempNumber.SetScale(this->entryCharLabel.GetScale());
            tempNumber.SetColour(labelColour, labelAlpha);
            tempNumber.SetCenter(Point2D(currX, currY-tempNumber.GetHeight()/2.0f));
            tempNumber.Draw();

            currX += this->largestEntryCharWidth/2.0f + PLACING_NO_TO_NAME_X_GAP;

            // Draw the name label so far...
            const std::string& nameStr = entry.playerName;
            for (int j = 0; j < static_cast<int>(nameStr.size()); j++) {
                // Draw the name out...
                char c = nameStr[j];
                
                TextLabel2D* currLbl = this->entryCharacterMap[c];
                currLbl->SetCenter(Point2D(currX + this->largestEntryCharWidth/2.0f, currY - currLbl->GetHeight()/2.0f));
                currLbl->SetColour(labelColour, labelAlpha);
                currLbl->Draw();

                currX += (this->largestEntryCharWidth + NAME_CHAR_X_GAP);
            }
            
            if (i == this->playerLeaderboardIdx) {
                // Player will be entering their name here...
                switch (this->currState) {
                    case EnteringNonTerminatingChar:
                    case EnteringTerminatingChar: {

                        // Draw the currently selected character with blinking cursor prompt...
                        this->cursorAnim.Tick(dT);
                        const float cursorIntensity = this->cursorAnim.GetInterpolantValue();
                        const Colour cursorColour(cursorIntensity, cursorIntensity, cursorIntensity);

                        std::string currChar = " ";
                        currChar[0] = this->entryCharacters[this->selectedEntryCharIdx];

                        this->entryCharLabel.SetText(currChar);
                        float entryCharHeight = this->entryCharLabel.GetHeight();
                        this->entryCharLabel.SetCenter(Point2D(currX + this->largestEntryCharWidth/2.0f, currY - entryCharHeight/2.0f));
                        this->entryCharLabel.SetColour(Colour(1,1,1) - cursorColour, labelAlpha);

                        // Draw the cursor
                        {
                            const float cursorWidth = this->largestEntryCharWidth;
                            const float cursorHeight = this->largestEntryCharWidth;

                            glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_LINE_BIT);
                            glEnable(GL_BLEND);
                            glDisable(GL_DEPTH_TEST);
                            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                            Camera::PushWindowCoords();
                            glMatrixMode(GL_MODELVIEW);
                            glPushMatrix();
                            glLoadIdentity();

                            glTranslatef(currX + cursorWidth/2.0f, currY - entryCharHeight/2.0f, 0);
                            glScalef(cursorWidth, cursorHeight, 1.0f);
                            glColor4f(cursorColour.R(), cursorColour.G(), cursorColour.B(), labelAlpha);

                            GeometryMaker::GetInstance()->DrawQuad();

                            glPopMatrix();
                            Camera::PopWindowCoords();

                            glPopAttrib();
                        }

                        // Draw the character
                        this->entryCharLabel.Draw();

                        break;
                    }

                    default:
                        break;
                }
            }

            currX = HALF_DISPLAY_WIDTH + HALF_SCORE_NAME_X_GAP;
            currScoreLbl->SetTopLeftCorner(currX, currY);
            currScoreLbl->SetColour(labelColour, labelAlpha);
            currScoreLbl->Draw();

            currY -= (currScoreLbl->GetHeight() + LEADERBOARD_ENTRY_Y_GAP);
            labelColour = labelColour - Colour(0.15f, 0.15f, 0.15f);
        }
    }

    fbo->UnbindFBObj();
    FBObj* finalFbo = fboAssets->RenderInitialFullscreenEffects(DISPLAY_WIDTH, DISPLAY_HEIGHT, dT);
    finalFbo->GetFBOTexture()->RenderTextureToFullscreenQuad();

    // Draw game title
    {
        currY = 0;
        glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_LINE_BIT);
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        Camera::PushWindowCoords();
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        currY = DISPLAY_HEIGHT - logoYSizeDiv2 - VERT_DIST_FROM_EDGE;
        glTranslatef((DISPLAY_WIDTH - logoXSize) / 2.0f + logoXSizeDiv2, currY, 0);
        glScalef(logoXSize, logoYSize, 1.0f);

        this->logoFadeAnim.Tick(dT);
        float logoAlpha = this->logoFadeAnim.GetInterpolantValue();

        glColor4f(1,1,1,logoAlpha);
        this->bbbLogoTex->BindTexture();
        GeometryMaker::GetInstance()->DrawQuad();
        this->bbbLogoTex->UnbindTexture();

        glPopMatrix();
        Camera::PopWindowCoords();

        glPopAttrib();
    }

    if (finishedLabelFading && this->currState == Waiting) {
        if (this->playerLeaderboardIdx >= 0) {
            this->SetState(EnteringNonTerminatingChar);
        }
        else if (this->currState != Finished) {
            // Wait for a bit of time then go to the finished state...
            this->timeWaitedInS += dT;
            if (this->timeWaitedInS >= TIME_TO_WAIT_UNTIL_FINISHED) {
                this->SetState(Finished);
                finishedLabelFading = false;

                // Fade-out all the labels
                this->logoFadeAnim.SetInitialInterpolantValue(this->logoFadeAnim.GetInterpolantValue());
                this->logoFadeAnim.SetFinalInterpolantValue(0.0f);
                this->logoFadeAnim.ResetToStart();

                for (int i = 0; i < static_cast<int>(this->labelFadeAnims.size()); i++) {
                    AnimationLerp<float>& anim = this->labelFadeAnims[i];
                    anim.SetInitialInterpolantValue(anim.GetInterpolantValue());
                    anim.SetFinalInterpolantValue(0.0f);
                    anim.ResetToStart();
                }
            }
        }
    }

    if (finishedLabelFading && this->currState == Finished) {
        // Full screen flash when the player is done entering their name...
        bool finished = this->arcadeFlashAnim.Tick(dT);
        float flashAmt = this->arcadeFlashAnim.GetInterpolantValue();
        if (flashAmt > 0.0f) {
            Camera::PushWindowCoords();
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

            glPushAttrib(GL_CURRENT_BIT | GL_POLYGON_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glPolygonMode(GL_FRONT, GL_FILL);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_DEPTH_TEST);

            glColor4f(1, 1, 1, flashAmt);
            glBegin(GL_QUADS);
            glVertex2f(Camera::GetWindowWidth(), 0);
            glVertex2f(Camera::GetWindowWidth(), Camera::GetWindowHeight());
            glVertex2f(0, Camera::GetWindowHeight());
            glVertex2f(0, 0);
            glEnd();

            glPopAttrib();
            glPopMatrix();
            Camera::PopWindowCoords();
        }



        if (finished) {
            this->GoToNextGameDisplayState();
        }
    }

    debug_opengl_state();
}

void HighScoreEntryDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton, 
                                               const GameControl::ActionMagnitude&) {

    if (this->currState == EnteringNonTerminatingChar || this->currState == EnteringTerminatingChar) {
        GameSound* sound = this->display->GetSound();

        switch (pressedButton) {
            case GameControl::LeftButtonAction: {
                // Move to the left-next letter to be selected by the player if the player is entering their name
                this->selectedEntryCharIdx--;
                if (this->selectedEntryCharIdx < 0) {
                    this->selectedEntryCharIdx = static_cast<int>(this->entryCharacters.size()) - 1;
                }
                
                sound->PlaySound(GameSound::MenuItemChangedSelectionEvent, false, false);
                break;
            }

            case GameControl::RightButtonAction: {
                // Move to the right-next letter to be selected by the player if the player is entering their name
                this->selectedEntryCharIdx = (this->selectedEntryCharIdx+1) % static_cast<int>(this->entryCharacters.size());
                sound->PlaySound(GameSound::MenuItemChangedSelectionEvent, false, false);
                break;
            }

            case GameControl::EnterButtonAction: {
                // Enter the letter selected by the player if they're entering their name, exit if done
                LeaderboardEntry& playerEntry = this->leaderboard->GetEntry(this->playerLeaderboardIdx);
                playerEntry.playerName += this->entryCharacters[this->selectedEntryCharIdx];

                switch (this->currPlayerEntryCharacterIdx) {
                    case 0:
                        sound->PlaySound(GameSound::MainMenuTitleBiffSlamEvent, false, false);
                        break;
                    case 1:
                        sound->PlaySound(GameSound::MainMenuTitleBamSlamEvent, false, false);
                        break;
                    case 2:
                        sound->PlaySound(GameSound::MainMenuTitleBlammoSlamEvent, false, false);
                        break;
                    default:
                        break;
                }

                this->currPlayerEntryCharacterIdx++;
                if (this->currState == EnteringNonTerminatingChar) {
                    // Are we on the last character or not?
                    if (this->currPlayerEntryCharacterIdx < NUM_NAME_CHARACTERS-1) {
                        this->SetState(EnteringNonTerminatingChar); // Not really necessary, but for robustness...
                    }
                    else {
                        this->SetState(EnteringTerminatingChar);
                    }
                }
                else {
                    // Terminating char...
                    this->SetState(Finished);
                    this->NameEntered();
                }
                break;
            }

            default:
                break;
        }
    }
    else if ((this->currState == Finished || this->currState == Waiting) && !GameControl::IsDirectionActionButton(pressedButton)) {
        // Go back to the main menu immediately
        this->GoToNextGameDisplayState();
    }
}

void HighScoreEntryDisplayState::SetState(const State& newState) {
    switch (newState) {
        case Waiting:
            this->currPlayerEntryCharacterIdx = 0;
            this->timeWaitedInS = 0.0;
            break;

        case EnteringNonTerminatingChar:
            assert(this->currPlayerEntryCharacterIdx < NUM_NAME_CHARACTERS-1);
            break;

        case EnteringTerminatingChar:
            this->currPlayerEntryCharacterIdx = NUM_NAME_CHARACTERS-1;
            break;

        case Finished:
            break;

        default:
            break;
    }
    this->currState = newState;
}

void HighScoreEntryDisplayState::NameEntered() {
    // Save the leaderboard to file...
    ResourceManager::WriteLeaderboard(*this->leaderboard);

    // Flash animation
    {
        std::vector<double> timeVals(3);
        timeVals[0] = 0.0; timeVals[1] = 0.1; timeVals[2] = 0.15;
        std::vector<float> flashAlphaVals(timeVals.size());
        flashAlphaVals[0] = 0.0; flashAlphaVals[1] = 1.0; flashAlphaVals[2] = 0.0;

        this->arcadeFlashAnim.SetLerp(timeVals, flashAlphaVals);
        this->arcadeFlashAnim.SetRepeat(false);
    }
}

void HighScoreEntryDisplayState::GoToNextGameDisplayState() {
    // Go back to the main menu / title state
    GameViewEventManager::Instance()->ActionArcadeWaitingForPlayerState(false);
    this->display->SetCurrentState(new MainMenuDisplayState(this->display, DisplayStateInfo::BuildMainMenuInfo(false)));
}