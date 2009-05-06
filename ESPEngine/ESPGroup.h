#ifndef __ESPGROUP_H__
#define __ESPGROUP_H__

#include <list>

#include "ESPUtil.h"

/**
 * Acts as a grouping of emitters and other ESP entities.
 */
class ESPGroup {

private:
	std::list<ESPEmitter*> emitters;

public:
	ESPGroup(){};
	~ESPGroup(){};

	void AddEmitter(ESPEmitter* emitter) {
		this->emitters.push_back(emitter);
	}

	void RemoveEmitter(ESPEmitter* emitter) {
		this->emitters.remove(emitter);
	}

	void Tick(double dT) {
		for (std::list<ESPEmitter*>::iterator iter = this->emitters.begin(); iter != this->emitters.end(); iter++) {
			(*iter)->Tick(dT);
		}
	}

	void Draw(const Camera& camera) {
		for (std::list<ESPEmitter*>::iterator iter = this->emitters.begin(); iter != this->emitters.end(); iter++) {
			(*iter)->Draw(camera);
		}
	}

};
#endif