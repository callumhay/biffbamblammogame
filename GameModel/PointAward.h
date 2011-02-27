#ifndef __POINTAWARD_H__
#define __POINTAWARD_H__

#include "ScoreTypes.h"

class PointAward {
public:
    PointAward(int amount, const ScoreTypes::BonusTypes& type, const Point2D& location);
    PointAward(const PointAward& copy) { *this = copy; };
    ~PointAward() {};

    const ScoreTypes::BonusTypes& GetType() const { return this->type; }
    int GetPointAmount() const { return this->amount; }
    int GetTotalPointAmount() const { return this->amount * this->multiplier; }
    int GetMultiplierAmount() const { return this->multiplier; }
    const Point2D& GetLocation() const { return this->location; }

    void SetPointAmount(int amt) { this->amount = amt; };
    void SetMultiplierAmount(int amt) { this->multiplier = amt; }

    PointAward& operator=(const PointAward& copy) {
        this->type = copy.type;
        this->amount = copy.amount;
        this->location = copy.location;
        return (*this);
    }

private:
    ScoreTypes::BonusTypes type;
    int amount;                     // The base point amount awarded (without multiplier)
    int multiplier;                 // The multiplier when awarded
    Point2D location;               // Location in the level that the points were awarded
};

inline PointAward::PointAward(int amount, const ScoreTypes::BonusTypes& type, const Point2D& location) :
amount(amount), type(type), location(location), multiplier(1) {
}

#endif // __POINTAWARD_H__