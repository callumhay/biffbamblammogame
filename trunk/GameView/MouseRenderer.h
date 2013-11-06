/**
 * MouseRenderer.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __MOUSERENDERER_H__
#define __MOUSERENDERER_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"

class Texture2D;
class GameModel;

class MouseRenderer {
public:
    MouseRenderer();
    ~MouseRenderer();

    void HideMouse();
    void ShowMouse(int oglWindowCoordX, int oglWindowCoordY);
    void SetWindowHasFocus(bool hasFocus);

    void Draw(double dT, const GameModel& gameModel);

private:
    static const double TIME_TO_SHOW_MOUSE_BEFORE_FADE;
    static const double FADE_IN_TIME;
    static const double FADE_OUT_TIME;

    enum MouseState { NotShowing = 0, FadingIn, Showing, FadingOut };
    
    MouseState currState;
    int mouseX, mouseY;
    double timeSinceLastShowMouse;
    bool windowHasFocus;
    AnimationLerp<float> mouseAlphaAnim;

    Texture2D* defaultMouseTex;
    Texture2D* boostMouseTex;

    void Render(const GameModel& gameModel);
    void SetState(MouseState newState);

    DISALLOW_COPY_AND_ASSIGN(MouseRenderer);
};

inline void MouseRenderer::HideMouse() {
    this->SetState(NotShowing);
}

inline void MouseRenderer::ShowMouse(int oglWindowCoordX, int oglWindowCoordY) {
    this->mouseX = oglWindowCoordX;
    this->mouseY = oglWindowCoordY;
    this->timeSinceLastShowMouse = 0.0;
    this->SetState(FadingIn);
}

inline void MouseRenderer::SetWindowHasFocus(bool hasFocus) {
    this->windowHasFocus = hasFocus;
}

#endif // __MOUSERENDERER_H__