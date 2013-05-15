/**
 * PointAward.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __POINTAWARD_H__
#define __POINTAWARD_H__

#include "ScoreTypes.h"
#include "LevelPiece.h"

class PointAward {
public:
    PointAward(int amount, const ScoreTypes::BonusTypes& type, const Point2D& location);
    PointAward(const PointAward& copy) { *this = copy; };
    ~PointAward() {};

    const LevelPiece::DestructionMethod& GetDestructionMethod() const { return this->destructionMethod; }
    const ScoreTypes::BonusTypes& GetType() const { return this->type; }
    int GetPointAmount() const { return this->amount; }
    int GetTotalPointAmount() const { return this->amount * this->multiplier; }
    float GetMultiplierAmount() const { return this->multiplier; }
    const Point2D& GetLocation() const { return this->location; }

    void SetDestructionMethod(const LevelPiece::DestructionMethod& method) { this->destructionMethod = method; }
    void SetPointAmount(int amt) { this->amount = amt; };
    void SetMultiplierAmount(float amt);

    PointAward& operator=(const PointAward& copy) {
        this->type = copy.type;
        this->amount = copy.amount;
        this->location = copy.location;
        return (*this);
    }

private:
    LevelPiece::DestructionMethod destructionMethod;    // Any destruction method that may be associated with the point award
    ScoreTypes::BonusTypes type;
    int amount;                     // The base point amount awarded (without multiplier)
    float multiplier;               // The multiplier when awarded
    Point2D location;               // Location in the level that the points were awarded
};

inline PointAward::PointAward(int amount, const ScoreTypes::BonusTypes& type, const Point2D& location) :
amount(amount), type(type), location(location), multiplier(1) {
}

inline void PointAward::SetMultiplierAmount(float amt) {
    
    switch (this->destructionMethod) {

        // Multipliers have no affect when the destruction is by lasers
        case LevelPiece::LaserProjectileDestruction:
        case LevelPiece::LaserBeamDestruction:
            this->multiplier = 1;
            break;

        // Multipliers are halved when the destruction is by rockets
        case LevelPiece::RocketDestruction:
            this->multiplier = std::max<float>(1.0f, 0.5f * amt);
            break;

        default:
            this->multiplier = amt;
            break;
    }
}

#endif // __POINTAWARD_H__