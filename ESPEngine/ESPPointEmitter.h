#ifndef __ESPPOINTEMITTER_H__
#define __ESPPOINTEMITTER_H__

#include "ESPEmitter.h"

class ESPPointEmitter : public ESPEmitter {

public:
	ESPPointEmitter();
	virtual ~ESPPointEmitter();

	virtual void Tick(const double dT);
	
};
#endif