#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "image.h"
#include "exception.h"

Image::Image(hd_object *object)
 : Drawable(object)
{ }

Image::~Image()
{
	free(obj->canvas);
}

static int is_png(const char *file)
{
	char magic[4];
	FILE *fp;

	if (!(fp = fopen(file, "r")))
		return 0;
	fread(magic, 1, 4, fp);
	fclose(fp);
	return !memcmp(magic, "\x89PNG", 4);
}

static int is_jpg(const char *file)
{
	char magic[2];
	FILE *fp;

	if (!(fp = fopen(file, "r")))
		return 0;
	fread(magic, 1, 2, fp);
	fclose(fp);
	return !memcmp(magic, "\377\330", 2);
}

Image *Image::load(const char *file)
{

	hd_object *object;
	if (access(file, R_OK)) {
		throw Exception(strerror(errno));
	}
        else if (is_png(file))
                object = HD_PNG_Create(file);
        else if (is_jpg(file))
                object = HD_JPEG_Create(file);
        else {
		throw Exception("Not a recognised image.");
        }
	Image *ret = new Image(object);
	free(object);
	return ret;
}
