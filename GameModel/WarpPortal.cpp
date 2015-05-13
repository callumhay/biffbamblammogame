/**
 * WarpPortal.cpp
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

#include "WarpPortal.h"
#include "LevelPiece.h"

void WarpPortal::WarpPortalInfo::Init(size_t wIdx, size_t hIdx, double startTime, double endTime, 
                                      GameWorld::WorldStyle warpToWorldStyle, int warpToLevelNum) {

    this->pos[0] = LevelPiece::PIECE_WIDTH * wIdx + LevelPiece::HALF_PIECE_WIDTH;
    this->pos[1] = LevelPiece::PIECE_HEIGHT * hIdx + LevelPiece::HALF_PIECE_HEIGHT;

    this->startTimeInSecs  = startTime;
    this->endTimeInSecs    = endTime;
    this->warpToWorldStyle = warpToWorldStyle;
    this->warpToLevelNum   = warpToLevelNum;
}

WarpPortal* WarpPortal::WarpPortalInfo::BuildWarpPortal() const {
    // Do some error checking on the info first
    if (this->warpToWorldStyle == GameWorld::None || warpToLevelNum < 1 || warpToLevelNum > 20) {
        return NULL;
    }

    // Unless the warp portal has an infinite life time, the start time should always be less than or equal to the end time
    if (this->endTimeInSecs > 0 && this->startTimeInSecs > this->endTimeInSecs) {
        return NULL;
    }

    return new WarpPortal(this->pos, this->startTimeInSecs, this->endTimeInSecs, this->warpToWorldStyle, this->warpToLevelNum);
}

WarpPortal::WarpPortal(const Point2D& pos, double startTimeInSecs, double endTimeInSecs, 
                       GameWorld::WorldStyle warpToWorldStyle, int warpToLevelNum) :
pos(pos), startTimeInSecs(startTimeInSecs), endTimeInSecs(endTimeInSecs), 
warpToWorldStyle(warpToWorldStyle), warpToLevelNum(warpToLevelNum) {

}

WarpPortal::~WarpPortal() {
}

