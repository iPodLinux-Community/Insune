#ifndef _DRAWABLE_H_
#define _DRAWABLE_H_
#include "hotdog.h"

class Drawable
{
 public:
	int x() const;
	int y() const;
	uint width() const;
	uint height() const;

	void setDirty(bool);

	void move(int x, int y);
	void resize(uint width, uint height);
	void scale(uint width, uint height);

	hd_surface surface() const;
	hd_object *object() const;

 protected:
 	Drawable(uint w, uint h);
 	Drawable(hd_object *);
	virtual ~Drawable();
	hd_object *obj;
	bool surfaceOwner;
};

#endif
