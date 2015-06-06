#ifndef _SIZE_H_
#define _SIZE_H_

class Size
{
 public:
	Size(int w, int h);
	Size(const Size &size);

	int width() const;
	int height() const;

	bool operator==(const Size &a) const;
	bool operator!=(const Size &a) const;

 private:
	int w;
	int h;
};

#endif
