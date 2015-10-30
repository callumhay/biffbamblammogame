/**
 * HighScoreEntryDisplayState.h
 * 
 * Copyright (c) 2015, Callum Hay
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

#ifndef __HIGHSCOREENTRYDISPLAYSTATE_H__
#define __HIGHSCOREENTRYDISPLAYSTATE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/TextLabel.h"

#include "DisplayState.h"

class ArcadeLeaderboard;

class HighScoreEntryDisplayState : public DisplayState {
public:
    HighScoreEntryDisplayState(GameDisplay* display, const DisplayStateInfo& info);
    ~HighScoreEntryDisplayState();

    bool AllowsGameModelUpdates() const { return false; }

    void RenderFrame(double dT);

    void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);
    void ButtonReleased(const GameControl::ActionButton&) {};
    void MousePressed(const GameControl::MouseButton&) {};
    void MouseReleased(const GameControl::MouseButton&) {};

    void DisplaySizeChanged(int, int) {}
    DisplayState::DisplayStateType GetType() const { return DisplayState::HighScoreEntry; }

private:
    static const int NUM_NAME_CHARACTERS = 3;

    static const int TITLE_Y_PADDING;
    static const int TITLE_BOTTOM_TO_SCORE_RANKING_Y_SPACE;
    static const int SCORE_RANKING_BOTTOM_TO_SCORE_Y_SPACE;
    static const float NAME_CHAR_X_GAP;

    static const double TIME_TO_WAIT_UNTIL_FINISHED;

    enum State { Waiting, EnteringNonTerminatingChar, EnteringTerminatingChar, Finished };
    State currState;

    std::vector<char> entryCharacters;
    std::map<char, TextLabel2D*> entryCharacterMap; // A listing of all available characters for entry of a player name on the leaderboard
    int selectedEntryCharIdx; // The current index of the selected entry characters
    float largestEntryCharWidth;

    int playerLeaderboardIdx; // The index of the player on the leaderboard, -1 if they aren't placed
    int currPlayerEntryCharacterIdx; // Index of the current character entry for the player name on the leaderboard (only relevant if entering name)
    
    TextLabel2D scoreRankingLabel;
    std::vector<TextLabel2D*> leaderboardScoreLabels;
    TextLabel2D entryCharLabel;
    float largestScoreWidth;
    std::vector<AnimationLerp<float> > labelFadeAnims;
    AnimationMultiLerp<float> cursorAnim;
    std::string playerEntryName;

    Texture2D* bbbLogoTex;
    AnimationLerp<float> logoFadeAnim;

    AnimationMultiLerp<float> arcadeFlashAnim;

    ArcadeLeaderboard* leaderboard;

    double timeWaitedInS;

    void SetState(const State& newState);
    void NameEntered();
    void GoToNextGameDisplayState();

    DISALLOW_COPY_AND_ASSIGN(HighScoreEntryDisplayState);
};

#endif // __HIGHSCOREENTRYDISPLAYSTATE_H__