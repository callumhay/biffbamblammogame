/**
 * PointAward.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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