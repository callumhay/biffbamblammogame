/**
 * PointsHUD.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __POINTSHUD_H__
#define __POINTSHUD_H__

#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Colour.h"

class Camera;
class Texture;
class TextLabel2D;
class PointAward;

/**
 * Class for encapsulating all animations and display for the Points Heads-up-display
 * which displays the current point total of the player, their current multiplier, number of
 * stars and a running display of any currently obtained point bonuses / scores.
 */
class PointsHUD {
public:
    static const int SCREEN_EDGE_VERTICAL_GAP;
    static const int SCREEN_EDGE_HORIZONTAL_GAP;

    PointsHUD();
    ~PointsHUD();

    //enum AnimationType { CreationAnimation, IdleAnimation };

    void Draw(int displayWidth, int displayHeight, double dT);

    void Reinitialize();

    void SetNumStars(int numStars);
    void SetScore(long pointScore);
    void SetMultiplier(int multiplierAmt);
    void PostPointNotification(const PointAward& pointAward);

    static const char* GetPointNotificationName(const PointAward& pointAward);

private:
    static const int STAR_SIZE;
    static const int STAR_GAP;
    static const int STAR_TO_SCORE_VERTICAL_GAP;
    static const int SCORE_TO_MULTIPLER_HORIZONTAL_GAP;
    static const int ALL_STARS_WIDTH;

    class PointNotification {
    public:
        static const int NOTIFIER_TO_NOTIFIER_VERTICAL_GAP;
        static const int INITIAL_NOTIFY_VERTICAL_GAP;

        PointNotification(const PointAward& pointAward, float initialTopY, float finalTopY);
        ~PointNotification();

        bool Draw(int displayWidth, double dT);
        
        void SetAlpha(float alpha);
        const Point2D& GetTopLeftCorner() const;
        float GetHeight() const;

    private:
        static const int NAME_POINT_VERTICAL_GAP;
        int pointAmount;
        TextLabel2D* notificationName;  // The name of the bonus/notification, may be NULL
        TextLabel2D* pointLabel;        // The points earned/removed, always not NULL

        AnimationLerp<float> moveAnimation;
        AnimationMultiLerp<ColourRGBA> rgbaAnimation;

        DISALLOW_COPY_AND_ASSIGN(PointNotification);
    };

    class MultiplierHUD {
    public:
        MultiplierHUD();
        ~MultiplierHUD();

        void Reinitialize();
        
        void SetAlpha(float alpha);
        void SetMultiplier(int multiplierAmt);
        void Draw(float rightMostX, float topMostY, double dT);

    private:
        static const float BANG_TEXT_BORDER;

        enum AnimationState { None, CreationAnimation, IncreaseAnimation, IdleAnimation, LeaveAnimation };
        AnimationState currState;

        float size;                     // The total size (width/height) of the multiplier
        int currPtMultiplier;           // The current score multiplier
        TextLabel2D* ptMultiplierLabel; // The display label for the multiplier
        Texture* multiplierBangTex;     // The 'bang' texture behind the multiplier label

        // Animations
        AnimationMultiLerp<float> scaleAnim;
        AnimationMultiLerp<ColourRGBA> rgbaLabelAnim;
        
        void DrawHUD(float rightMostX, float topMostY);
        void SetCurrentAnimationState(const AnimationState& state);

        
        static float  GetMultiplierScale(int multiplier);

        DISALLOW_COPY_AND_ASSIGN(MultiplierHUD);
    };

    typedef std::list<PointNotification*> PointNotifyList;
    typedef PointNotifyList::iterator PointNotifyListIter;
    typedef PointNotifyList::const_iterator PointNotifyListConstIter;

    //AnimationType currAnimState;

    int numStars;                                   // The current number of stars awarded to the player
    long currPtScore;                               // The current score
    std::list<PointNotification*> ptNotifications;  // FIFO Queue of current point notifications
    MultiplierHUD* multiplier;                      // HUD for the multiplier
    
    // Labels
    TextLabel2D* ptScoreLabel;
    // Animations
    AnimationLerp<long> scoreAnimator;   // Animates the current score (so that it looks like it's tallying points over time)
    // Textures
    Texture* starTex;

    void DrawMultiplier(float rightMostX, float topMostY);
    void DrawIdleStars(float rightMostX, float topMostY, double dT);
    void DrawQuad(float centerX, float centerY, float size);

    void SetAlpha(float alpha);
    void ClearNotifications();

    DISALLOW_COPY_AND_ASSIGN(PointsHUD);
};

inline void PointsHUD::SetMultiplier(int multiplierAmt) {
    assert(multiplierAmt >= 1);
    this->multiplier->SetMultiplier(multiplierAmt);
}

#endif // __POINTSHUD_H__