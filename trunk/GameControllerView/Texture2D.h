#ifndef __TEXTURE2D_H__
#define __TEXTURE2D_H__

#include <string>

class Texture2D {

private:
	bool isInitialized;

public:
	Texture2D();
	Texture2D(const std::string& filepath);
	~Texture2D();
	
	bool IsInitialized() const {
		return this->isInitialized;
	}

};

#endif