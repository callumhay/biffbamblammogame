/**
 * IGameMenu.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __IGAMEMENU_H__
#define __IGAMEMENU_H__

#include "../GameControl/GameControl.h"

class IGameMenu {
public:
    IGameMenu() {}
    virtual ~IGameMenu() {}

	virtual void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude) = 0;
	virtual void ButtonReleased(const GameControl::ActionButton& releasedButton) = 0;

    virtual void ActivatedMenuItemChanged()   = 0;
    virtual void DeactivateSelectedMenuItem() = 0;
};

#endif // __IGAMEMENU_H__