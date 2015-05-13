/**
 * GameViewEventManager.h
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

#ifndef __GAMEVIEWEVENTMANAGER_H__
#define __GAMEVIEWEVENTMANAGER_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "GameViewEventListener.h"

class GameViewEventManager {

public:
	static GameViewEventManager* Instance();
	static void DeleteInstance();
	
	// Register functions and lists of registered listeners
	void RegisterListener(GameViewEventListener* listener);
	void UnregisterListener(GameViewEventListener* listener);

#define REGISTER_ACTION_FUNC_1ARG(eventName, argType0) void Action##eventName(const argType0& arg0) { \
    std::list<GameViewEventListener*>::iterator listenerIter = this->eventListeners.begin(); \
    for (; listenerIter != this->eventListeners.end(); ++listenerIter) { (*listenerIter)->eventName(arg0); } }
#define REGISTER_ACTION_FUNC_2ARG(eventName, argType0, argType1) void Action##eventName(const argType0& arg0, const argType1& arg1) { \
    std::list<GameViewEventListener*>::iterator listenerIter = this->eventListeners.begin(); \
    for (; listenerIter != this->eventListeners.end(); ++listenerIter) { (*listenerIter)->eventName(arg0, arg1); } }

    REGISTER_ACTION_FUNC_2ARG(DisplayStateChanged, DisplayState::DisplayStateType, DisplayState::DisplayStateType);
    REGISTER_ACTION_FUNC_1ARG(BiffBamBlammoSlamEvent, GameViewEventListener::SlamType);

#undef REGISTER_ACTION_FUNC_1ARG
#undef REGISTER_ACTION_FUNC_2ARG

private:
	GameViewEventManager();

	std::list<GameViewEventListener*> eventListeners;
	static GameViewEventManager* instance;

    DISALLOW_COPY_AND_ASSIGN(GameViewEventManager);
};

#endif
