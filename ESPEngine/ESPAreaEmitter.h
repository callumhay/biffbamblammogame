#ifndef __ESPAREAEMITTER_H__
#define __ESPAREAEMITTER_H__

#include "ESPEmitter.h"

class ESPAreaEmitter : public ESPEmitter {

public:
	ESPAreaEmitter();
	virtual ~ESPAreaEmitter();

	virtual void Tick(const double dT);
	virtual void Draw(const Camera& camera);
};
#endif