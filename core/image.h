#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "drawable.h"

class Image : public Drawable
{
 public:
	static Image *load(const char *);
	~Image();

 private:
	Image(hd_object *);
};

#endif
