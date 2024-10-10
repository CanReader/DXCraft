#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "../Object.h"

class Texture : public Object
{
public:
	Texture();
	Texture(const char* TextureFile);



private:
	const char* FilePath;

};


#endif // _TEXTURE_H_