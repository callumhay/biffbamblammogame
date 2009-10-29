#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "BasicIncludes.h"
#include "Algebra.h"

/**
 * Animates a given interpolant over time using basic linear interpolation.
 */
template<class T> 
class AnimationLerp {

private:
	bool hasOwnInterpolant;	// Whether an interpolant was provided or not
	bool repeat;						// Whether we repeat the animation or not

	T* interpolant;	// The given interpolant pointer
	T y0, y1;				// Initial and final values of the interpolant
	double x0, x1;	// Initial and final time
	double x;				// The currently tracked time value - increases with each tick until it reaches x1 (final time)

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
		this->x = 0.0;
	}
	void SetInterpolantValue(T value) {
		(*this->interpolant) = value;
	}
	T GetInterpolantValue() const {
		return *this->interpolant;
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
};


/**
 * Animates a given interpolant over time using linear interpolation over multiple points.
 */
template<class T> 
class AnimationMultiLerp {

private:
	bool hasOwnInterpolant;	// Whether an interpolant was provided or not
	bool repeat;						// Whether we repeat the animation or not

	T* interpolant;										// The given interpolant pointer
	std::vector<T> interpolationPts;	// Values to interpolate across for the interpolant
	std::vector<double>	timePts;			// Times for each interpolation
	double x;													// The currently tracked time value - increases with each tick until it reaches x1 (final time)

	unsigned int tracker;		// Tracks the index of the interpolation/time values currently being used

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

	~AnimationMultiLerp() {
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
	void SetInterpolantValue(T value) {
		(*this->interpolant) = value;
	}
	T GetInterpolantValue() const {
		return *this->interpolant;
	}

	std::vector<T> GetInterpolationValues() const {
		return this->interpolationPts;
	}

	void SetInitialInterpolationValue(const T& value) {
		assert(this->interpolationPts.size() > 0);
		this->interpolationPts[0] = value;
	}

	void SetFinalInterpolationValue(const T& value) {
		assert(this->interpolationPts.size() > 0);
		this->interpolationPts[this->interpolationPts.size()-1] = value;
	}

	std::vector<double> GetTimeValues() const {
		return this->timePts;
	}

	/**
	 * Set whether the animation is on repeat or not.
	 */
	void SetRepeat(bool repeatOn) {
		this->repeat = repeatOn;
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

	/**
	 * Simplifed setup of the linear interpolation values - this will only accept the final time and the
	 * final value for the interpolant. The initial time will be set to zero and the initial value
	 * of the interpolant will be set to the interpolant's current value.
	 */
	void SetLerp(double finalTime, T finalValue) {
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
		T &valueStart = this->interpolationPts[this->tracker];
		T &valueEnd   = this->interpolationPts[this->tracker+1];
		double timeStart  = this->timePts[this->tracker];
		double timeEnd		= this->timePts[this->tracker+1];

		if (fabs(timeEnd - timeStart) < EPSILON) {
			x = timeEnd;
			(*this->interpolant) = valueEnd;
			this->tracker++;
			return !this->repeat;
		}

		// Linearly interpolate the given interpolate over the current value and time interval
		(*this->interpolant) = valueStart + (x - timeStart) * (valueEnd - valueStart) / (timeEnd - timeStart);

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
};


#endif