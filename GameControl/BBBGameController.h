/**
 * BBBGameController.h
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

#ifndef __BBBGAMECONTROLLER_H__
#define __BBBGAMECONTROLLER_H__

#define USE_XBOX360_CONTROLLER
//#define USE_KINECT_CONTROLLER

#include "../BlammoEngine/BasicIncludes.h"

class GameModel;
class GameDisplay;

/**
 * Abstract superclass for a generalized game controller to be used in a game.
 */
class BBBGameController {
public:
	virtual ~BBBGameController() {};

	enum VibrateAmount { NoVibration = 0, VerySoftVibration, SoftVibration, MediumVibration, HeavyVibration, VeryHeavyVibration };
	virtual void Vibrate(double lengthInSeconds, const VibrateAmount& leftMotorAmt, const VibrateAmount& rightMotorAmt) = 0;
    virtual void ClearVibration() = 0;

	virtual bool ProcessState(double dT)  = 0;
	virtual void Sync(size_t frameID, double dT) = 0;

	virtual bool IsConnected() const = 0;

protected:
	BBBGameController(GameModel* model, GameDisplay* display) : model(model), display(display) {
		assert(model != NULL);
		assert(display != NULL);
	};
	
	GameModel* model; 
	GameDisplay* display;

private:
	DISALLOW_COPY_AND_ASSIGN(BBBGameController);
};

#endif // __BBBGAMECONTROLLER_H__