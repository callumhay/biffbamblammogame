/**
 * GameControl.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMECONTROL_H__
#define __GAMECONTROL_H__

class GameControl {
public:
    enum ActionButton { UpButtonAction, DownButtonAction, LeftButtonAction, RightButtonAction, 
                        EnterButtonAction, EscapeButtonAction, PauseButtonAction, SpecialCheatButtonAction,
                        OtherButtonAction};

    enum ActionMagnitude { ZeroMagnitude, SmallMagnitude, NormalMagnitude, LargeMagnitude, FullMagnitude };

    enum MouseButton { LeftMouseButton, RightMouseButton };

private:
	GameControl(){};
	~GameControl(){};
};

#endif // __GAMECONTROLACTIONBUTTONS_H__