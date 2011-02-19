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

#include "../BlammoEngine/Animation.h"

class Camera;
class Texture;
class TextLabel2D;

/**
 * Class for encapsulating all animations and display for the Points Heads-up-display
 * which displays the current point total of the player, their current multiplier, number of
 * stars and a running display of any currently obtained point bonuses / scores.
 */
class PointsHUD {
public:
    PointsHUD();
    ~PointsHUD();

    //enum AnimationType { CreationAnimation, IdleAnimation, DestructionAnimation };

    void Draw(int displayWidth, int displayHeight, double dT);

    void Reinitialize();

    void SetNumStars(int numStars);
    void SetScore(long pointScore);
    void SetMultiplier(int multiplierAmt);
    void PostPointNotification(const std::string& name, int pointAmount);

private:
    static const int STAR_SIZE;
    static const int STAR_GAP;
    static const int SCREEN_EDGE_VERTICAL_GAP;
    static const int SCREEN_EDGE_HORIZONTAL_GAP;
    static const int STAR_TO_SCORE_VERTICAL_GAP;

    class PointNotification {
    public:
        PointNotification(const std::string& name, int pointAmount);
        ~PointNotification();

        bool Draw(double dT);
        
        void SetAlpha(float alpha);

    private:
        int pointAmount;
        TextLabel2D* notificationName;  // The name of the bonus/notification, may be NULL
        TextLabel2D* pointLabel;        // The points earned/removed, always not NULL



        DISALLOW_COPY_AND_ASSIGN(PointNotification);
    };

    typedef std::list<PointNotification*> PointNotifyList;
    typedef PointNotifyList::iterator PointNotifyListIter;

    int numStars;                                   // The current number of stars awarded to the player
    long currPtScore;                               // The current score
    int currPtMultiplier;                           // The current score multiplier
    std::list<PointNotification*> ptNotifications;  // FIFO Queue of current point notifications

    // Labels
    TextLabel2D* ptScoreLabel;
    TextLabel2D* ptMultiplierLabel;

    // Animations
    AnimationLerp<long> scoreAnimator;   // Animates the current score (so that it looks like it's tallying points over time)
    
    // Textures
    Texture* starTex;

    void DrawIdleStars(float rightMostX, float topMostY, double dT);
    void DrawQuad(float centerX, float centerY, float size);

    void SetAlpha(float alpha);
    void ClearNotifications();

    DISALLOW_COPY_AND_ASSIGN(PointsHUD);
};

inline void PointsHUD::SetMultiplier(int multiplierAmt) {
    assert(multiplierAmt >= 1);
    this->currPtMultiplier = multiplierAmt;
}

#endif // __POINTSHUD_H__