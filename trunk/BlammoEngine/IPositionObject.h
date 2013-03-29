/**
 * IPositionObject.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */


#ifndef __IPOSITIONOBJECT_H__
#define __IPOSITIONOBJECT_H__

#include "Point.h"

/**
 * Ridiculously simple interface for objects with a position.
 */
class IPositionObject {
public:
    IPositionObject() {}
    virtual ~IPositionObject() {}

    virtual Point3D GetPosition3D() const = 0;
};

#endif // __IPOSITIONOBJECT_H__