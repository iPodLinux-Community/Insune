#ifndef _ARTIST_H_
#define _ARTIST_H_

class Artist
{
 public:
	Artist(const char *name);
	~Artist();

	const char *getName() const;

	void setName(const char *);

 private:
	char *name;
};

#endif
