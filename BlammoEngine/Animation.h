/**
 * Animation.h
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

#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "BasicIncludes.h"
#include "Algebra.h"

/**
 * Animates a given interpolant over time using basic linear interpolation.
 */
template<class T> 
class AnimationLerp {
public:	
	AnimationLerp() : repeat(false), hasOwnInterpolant(true), interpolant(new T()), x(0), x0(0), x1(1) {}
	AnimationLerp(T value) : repeat(false), hasOwnInterpolant(true), interpolant(new T(value)), x(0), x0(0), x1(1), y0(value), y1(value) {}
	AnimationLerp(T* interpolant) : repeat(false), hasOwnInterpolant(false), interpolant(interpolant), x(0), x0(0), x1(1), y0(*interpolant), y1(*interpolant) {}
	AnimationLerp(const AnimationLerp& copy) : interpolant(NULL), x(copy.x), x0(copy.x0), x1(copy.x1), 
		y0(copy.y0), y1(copy.y1), hasOwnInterpolant(copy.hasOwnInterpolant), repeat(copy.repeat) {
			this->interpolant = this->hasOwnInterpolant ? new T(*copy.interpolant) : copy.interpolant;
	}

	AnimationLerp& operator=(const AnimationLerp &rhs) {
		// Clear out the interpolant if this has its own
		if (this->hasOwnInterpolant) {
			delete this->interpolant;
			this->interpolant = NULL;
		}

		this->x  = rhs.x;
		this->x0 = rhs.x0;
		this->x1 = rhs.x1; 
		this->y0 = rhs.y0;
		this->y1 = rhs.y1;
		this->hasOwnInterpolant = rhs.hasOwnInterpolant;
		this->repeat = rhs.repeat;
		this->interpolant = rhs.hasOwnInterpolant ? new T(*rhs.interpolant) : rhs.interpolant;
		return *this;
	}

	~AnimationLerp() {
		if (this->hasOwnInterpolant) {
			delete this->interpolant;
			this->interpolant = NULL;
		}
	}

	/**
	 * Reset the animation back to the start.
	 */
	void ResetToStart() {
        this->SetInterpolantValue(this->y0);
		this->x = 0.0;
	}
	void SetInterpolantValue(T value) {
		(*this->interpolant) = value;
	}
	const T& GetInterpolantValue() const {
		return *this->interpolant;
	}
    const T& GetTargetValue() const {
        return this->y1;
    }
    double GetFinalTime() const {
        return this->x1;
    }

    bool IsAtStart() const {
        return this->x == 0.0;
    }
    double GetTimeValue() const {
        return this->x;
    }
    void SetTimeValue(double t) {
        assert(t >= 0.0);
        assert(t <= this->GetFinalTime());
        this->x = t;
    }

	/**
	 * Set whether the animation is on repeat or not.
	 */
	void SetRepeat(bool repeatOn) {
		this->repeat = repeatOn;
	}

	/**
	 * Set the linear interpolation values - this will reset the timer of the animation to t = 0 and
	 * set the initial and final times for the animation to start and end, respectively. It will also
	 * set the initial and final values for the interpolant to start and end with, respectively.
	 */
	void SetLerp(double initialTime, double finalTime, T initialValue, T finalValue) {
		this->x = 0.0;
		this->x0 = initialTime;
		this->x1 = finalTime;
		this->y0 = initialValue;
		this->y1 = finalValue;
	}

	/**
	 * Simplifed setup of the linear interpolation values - this will only accept the final time and the
	 * final value for the interpolant. The initial time will be set to zero and the initial value
	 * of the interpolant will be set to the interpolant's current value.
	 */
	void SetLerp(double finalTime, T finalValue) {
		this->x = 0.0;
		this->x0 = x;
		this->x1 = finalTime;
		this->y0 = *interpolant;
		this->y1 = finalValue;
	}

    void ClearLerp() {
        this->x = 0.0;
        this->x0 = 0;
        this->x1 = 0;
        this->y0 = *interpolant;
        this->y1 = *interpolant;
    }

    void ScalePlaySpeed(float scale) {
        assert(scale > 0);
        this->x1 *= scale;
        this->x  *= scale;
    }

	/**
	 * Tick the animation (i.e., animate it using linear interpolation).
	 * Returns: true if the animation is complete, false otherwise.
	 */
	bool Tick(double dT) {
		if (fabs(this->x1 - this->x0) < EPSILON) {
			return true;
		}
		
		if (this->x >= this->x1) {
			if (this->repeat) {
				this->ResetToStart();
			}
			else {
				(*this->interpolant) = this->y1;
				this->x = this->x1;
				return true;
			}
		}

		// If the current amount of time is less than the initial time for the
		// animation then we just increment the time and do nothing else
		if (this->x < this->x0) {
			this->x += dT;
			return false;
		}

		// Linearly interpolate the given interpolate over time
		(*this->interpolant) = this->y0 + (x - this->x0) * (this->y1 - this->y0) / (this->x1 - this->x0);

		if (this->x < this->x1) {
			this->x += dT;
			return false;
		}
		else if (this->x >= this->x1) {
			this->x = this->x1;
			(*this->interpolant) = this->y1;
		}
		
		return !this->repeat;
	}

        // Get the derivative of the interpolant with respect to time for the current
    // Lerp interval that this animation is animating on
    T GetDxDt() const {
        double dT = x1 - x0;
        if (dT <= 0) {
            return T(0);
        }

        T dX = y1 - y0;

        return dX / dT;
    }

private:
	bool hasOwnInterpolant;	// Whether an interpolant was provided or not
	bool repeat;						// Whether we repeat the animation or not

	T* interpolant;	// The given interpolant pointer
	T y0, y1;				// Initial and final values of the interpolant
	double x0, x1;	// Initial and final time
	double x;				// The currently tracked time value - increases with each tick until it reaches x1 (final time)
};


/**
 * Animates a given interpolant over time using linear interpolation over multiple points.
 */
template<class T> 
class AnimationMultiLerp {
public:	
	AnimationMultiLerp() : hasOwnInterpolant(true), interpolant(new T()), repeat(false), x(0.0), tracker(0) {}
	AnimationMultiLerp(T value) : hasOwnInterpolant(true), interpolant(new T(value)), repeat(false), x(0.0), tracker(0) {}
	AnimationMultiLerp(T* interpolant) : hasOwnInterpolant(false), repeat(false), interpolant(interpolant), x(0.0), tracker(0) {}
	AnimationMultiLerp(const AnimationMultiLerp<T>& copy) : interpolant(NULL), repeat(copy.repeat), interpolationPts(copy.interpolationPts),
		timePts(copy.timePts), x(copy.x), tracker(copy.tracker), hasOwnInterpolant(copy.hasOwnInterpolant){
			this->interpolant = this->hasOwnInterpolant ? new T(*copy.interpolant) : copy.interpolant;
	}

	AnimationMultiLerp& operator=(const AnimationMultiLerp &rhs) {
		// Clear out the interpolant if this has its own
		if (this->hasOwnInterpolant) {
			delete this->interpolant;
			this->interpolant = NULL;
		}

		this->x  = rhs.x;
		this->interpolationPts = rhs.interpolationPts;
		this->timePts = rhs.timePts;
		this->tracker = rhs.tracker;
		this->hasOwnInterpolant = rhs.hasOwnInterpolant;
		this->repeat = rhs.repeat;
		this->interpolant = rhs.hasOwnInterpolant ? new T(*rhs.interpolant) : rhs.interpolant;
		return *this;
	}

	virtual ~AnimationMultiLerp() {
		if (this->hasOwnInterpolant) {
			delete this->interpolant;
			this->interpolant = NULL;
		}
	}

	/**
	 * Reset the animation back to the start.
	 */
	void ResetToStart() {
		assert(this->interpolationPts.size() > 0);
		this->x = 0.0;
		this->tracker = 0;
		this->SetInterpolantValue(this->interpolationPts[0]);
	}
    void SetToRandom() {
        assert(this->interpolationPts.size() > 0);

        size_t randomIdx = Randomizer::GetInstance()->RandomUnsignedInt() % this->interpolationPts.size();
        this->x = this->timePts[randomIdx];
        this->tracker = randomIdx;
        this->SetInterpolantValue(this->interpolationPts[randomIdx]);
    }
	void SetInterpolantValue(T value) {
		(*this->interpolant) = value;
	}
	T& GetInterpolantValue() const {
		return *this->interpolant;
	}
	std::vector<T> GetInterpolationValues() const {
		return this->interpolationPts;
	}
    std::vector<T>& GetEditableInterpolationValues() {
        return this->interpolationPts;
    }
    bool GetHasInterpolationSet() const {
        return !this->interpolationPts.empty();
    }
    
    void ScalePlaySpeed(float scale) {
        assert(scale > 0);
        for (int i = 0; i < static_cast<int>(this->timePts.size()); i++) {
            this->timePts[i] *= scale;
        }
        this->x *= scale;
    }

	void SetInitialInterpolationValue(const T& value) {
		assert(this->interpolationPts.size() > 0);
		this->interpolationPts[0] = value;
	}

	void SetFinalInterpolationValue(const T& value) {
		assert(this->interpolationPts.size() > 0);
		this->interpolationPts[this->interpolationPts.size()-1] = value;
	}
    const T& GetFinalInterpolationValue() const {
        if (this->interpolationPts.empty()) {
            assert(false);
            return *this->interpolant;
        }
        return this->interpolationPts[this->interpolationPts.size()-1];
    }
    T& GetEditableFinalInterpolationValue() {
        if (this->interpolationPts.empty()) {
            assert(false);
            return *this->interpolant;
        }
        return this->interpolationPts[this->interpolationPts.size()-1];
    }

    void SetInterpolationValue(size_t idx, const T& value) {
		assert(idx < this->interpolationPts.size());
		this->interpolationPts[idx] = value;
    }
    const T& GetInterpolationValue(size_t idx) {
		assert(idx < this->interpolationPts.size());
		return this->interpolationPts[idx];
    }
    double GetCurrentTimeValue() const {
        return this->x;
    }
    double GetFinalTimeValue() const {
        assert(!this->timePts.empty());
        return this->timePts.back();
    }
	const std::vector<double>& GetTimeValues() const {
		return this->timePts;
	}

	/**
	 * Set whether the animation is on repeat or not.
	 */
	void SetRepeat(bool repeatOn) {
		this->repeat = repeatOn;
	}
    bool GetRepeat() const {
        return this->repeat;
    }

	/**
	 * Set the linear interpolation values and their corresponding time intervals i.e., 
	 * the value at times[i] will be when the interpolant has reached the values of interpolations[i].
	 * Where times[0] will be when the interpolation begins at interpolations[0] and times[times.size()-1]
	 * will be when all interpolations cease at a value of interpolations[interpolations.size()-1].
	 */
	void SetLerp(const std::vector<double>& times, const std::vector<T>& interpolations) {
		assert(times.size() >= 2);
		assert(times.size() == interpolations.size());
		
		this->x = 0.0;
		this->tracker = 0;
		this->timePts = times;
		this->interpolationPts = interpolations;
	}
    void SetLerp(double t0, double t1, const T& v0, const T& v1) {

        this->x = 0.0;
        this->tracker = 0;
        this->timePts.resize(2);
        this->timePts[0] = t0; this->timePts[1] = t1;
        this->interpolationPts.resize(2);
        this->interpolationPts[0] = v0; this->interpolationPts[1] = v1;
    }
    void SetLerp(double t0, double t1, double t2, const T& v0, const T& v1, const T& v2) {

        this->x = 0.0;
        this->tracker = 0;
        this->timePts.resize(3);
        this->timePts[0] = t0; this->timePts[1] = t1; this->timePts[2] = t2;
        this->interpolationPts.resize(3);
        this->interpolationPts[0] = v0; this->interpolationPts[1] = v1; this->interpolationPts[2] = v2;
    }

	/**
	 * Simplifed setup of the linear interpolation values - this will only accept the final time and the
	 * final value for the interpolant. The initial time will be set to zero and the initial value
	 * of the interpolant will be set to the interpolant's current value.
	 */
	void SetLerp(double finalTime, T finalValue) {
		this->ClearLerp();

		this->x = 0.0;
		this->tracker = 0;
		
		this->timePts.reserve(2);
		this->timePts.push_back(x);
		this->timePts.push_back(finalTime);

		this->interpolationPts.reserve(2);
		this->interpolationPts.push_back(*interpolant);
		this->interpolationPts.push_back(finalValue);
	}

	/**
	 * Append the given lerp to any existing lerp in this animation.
	 */
	void AppendLerp(const std::vector<double>& times, const std::vector<T>& interpolations) {
		assert(times.size() == interpolations.size());

		if (this->timePts.size() == 0) {
			this->SetLerp(times, interpolations);
			return;
		}

		this->timePts.reserve(this->timePts.size() + times.size());
		this->interpolationPts.reserve(this->interpolationPts.size() + interpolations.size());

		std::vector<double>::const_iterator timeIter = times.begin();
		typename std::vector<T>::const_iterator interIter = interpolations.begin();
		double originalEndTime = this->timePts.back();
		for (; timeIter != times.end() && interIter != interpolations.end(); ++timeIter, ++interIter) {
			// Add the times to the last time that was already in the lerp
			this->timePts.push_back(originalEndTime + *timeIter);
			// Just tack the interpolation points on as well
			this->interpolationPts.push_back(*interIter);
		}
	}

	void AppendLerp(double finalTime, T finalValue) {
		if (this->timePts.size() == 0) {
			this->SetLerp(finalTime, finalValue);
			return;
		}

		this->timePts.reserve(this->timePts.size() + 1);
		this->interpolationPts.reserve(this->interpolationPts.size() + 1);
		this->timePts.push_back(finalTime + this->timePts.back());
		this->interpolationPts.push_back(finalValue);
	}

	/**
	 * Completely clear the interpolation animation values (i.e., next time
	 * tick is called nothing will happen).
	 */
	void ClearLerp() {
		this->x = 0.0;
		this->tracker = 0;
		this->timePts.clear();
		this->interpolationPts.clear();
	}

	/**
	 * Tick the animation (i.e., animate it using linear interpolation).
	 * Returns: true if the animation is complete, false otherwise.
	 */
	bool Tick(double dT) {
		assert(this->timePts.size() == this->interpolationPts.size());	
		// As a safety precaution exit if the animation isn't setup
		if (this->timePts.size() < 2 || this->timePts.size() != this->interpolationPts.size()) {
			return true;
		}

		// Check to see if we've reached the end of the animation
		if (this->tracker == this->timePts.size()-1) {
			if (this->repeat) {
				// The animation is on repeat so just restart it and continue onwards
				this->ResetToStart();
			}
			else {
				return true;
			}
		}

		// If the current amount of time is less than the initial time for the
		// animation then we just increment the time and do nothing else
		if (x < this->timePts[0]) {
			assert(this->tracker == 0);
			x += dT;
			return false;
		}

		// Grab the current interpolation values
		const T& valueStart = this->interpolationPts[this->tracker];
		const T& valueEnd   = this->interpolationPts[this->tracker+1];
		const double& timeStart  = this->timePts[this->tracker];
		const double& timeEnd		= this->timePts[this->tracker+1];

		if (fabs(timeEnd - timeStart) < EPSILON) {
			x = timeEnd;
			(*this->interpolant) = valueEnd;
			this->tracker++;
			return !this->repeat && (this->tracker == this->timePts.size()-1);
		}

		// Linearly interpolate the given interpolate over the current value and time interval
		(*this->interpolant) = this->ExecuteLerp(valueStart, valueEnd, timeStart, timeEnd);

		// Figure out if we move on to the next interval or not
		if (x < timeEnd) {
			x += dT;
			return false;
		}
		else if (x >= timeEnd) {
			x = timeEnd;
			(*this->interpolant) = valueEnd;
			this->tracker++;
		}
		
		return !this->repeat && (this->tracker == this->timePts.size()-1);
	}

    // Get the derivative of the interpolant with respect to time for the current
    // Lerp interval that this animation is animating on
    T GetDxDt() const {

        if (this->timePts.size() < 2 || this->timePts.size() != this->interpolationPts.size() || x < this->timePts[0]) {
            return T(0);
        }
        else if (this->tracker == this->timePts.size()-1) {
            if (this->repeat) {
                // Bit tricky, need to wrap...
                const double timeStart = this->timePts[this->tracker];
                const double timeEnd   = timeStart + (this->timePts[1] - this->timePts[0]);
                double dT = timeEnd - timeStart;
                if (dT <= 0) {
                    return T(0);
                }

                const T& valueStart = this->interpolationPts[this->tracker];
		        const T& valueEnd   = this->interpolationPts[this->tracker+1];
                T dX = valueEnd - valueStart;

                return dX / dT;
            }
            else {
                return T(0);
            }
        }

		double timeStart = this->timePts[this->tracker];
		double timeEnd   = this->timePts[this->tracker+1];
        double dT = timeEnd - timeStart;

        if (dT <= 0) {
            return T(0);
        }

        const T& valueStart = this->interpolationPts[this->tracker];
		const T& valueEnd   = this->interpolationPts[this->tracker+1];
        T dX = valueEnd - valueStart;

        return dX / dT;
    }

protected:
    virtual T ExecuteLerp(const T& valueStart, const T& valueEnd, double timeStart, double timeEnd) const {
        return valueStart + (this->x - timeStart) * (valueEnd - valueStart) / (timeEnd - timeStart);
    }

	bool hasOwnInterpolant;	// Whether an interpolant was provided or not
	bool repeat;						// Whether we repeat the animation or not

	T* interpolant;										// The given interpolant pointer
	std::vector<T> interpolationPts;	// Values to interpolate across for the interpolant
	std::vector<double>	timePts;			// Times for each interpolation
	double x;													// The currently tracked time value - increases with each tick until it reaches x1 (final time)

	unsigned int tracker;		// Tracks the index of the interpolation/time values currently being used

};

#endif