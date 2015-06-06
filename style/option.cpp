#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "insune.h"
#include "option.h"


static struct _def_colors {
	const char *str;
	Color col;
} def_colors[] = {
	{"white", 0xffffff}, {"black", 0x000000}, {"gray",  0x888888},
	{"red",   0xff0000}, {"green", 0x00ff00}, {"blue",  0x0000ff},
	{0, 0}
};

#define RGBA(r,g,b,a) (((a) << 24) |					\
                       (((((r)*(a)) + 0x80) >> 8) << 16) |		\
                       (((((g)*(a)) + 0x80)     ) & 0xFF00) |		\
                        ((((b)*(a)) + 0x80) >> 8))

#define PREMUL(col) (RGBA(((col) & 0x00ff0000) >> 16, \
			  ((col) & 0x0000ff00) >> 8,  \
			  ((col) & 0x000000ff),       \
			  ((col) & 0xff000000) >> 24))

Color atoc(const char *str)
{
	Color col = 0;
	if (*str == '#') {
		str++;
		sscanf(str, "%x", &col);
		if (strlen(str) == 3 || strlen(str) == 4) {
			col =   (((col & 0x000f)      )|((col & 0x000f) <<  4))|
				(((col & 0x00f0) <<  4)|((col & 0x00f0) <<  8))|
				(((col & 0x0f00) <<  8)|((col & 0x0f00) << 12))|
				(((col & 0xf000) << 12)|((col & 0xf000) << 16));
		}
		if (strlen(str) % 4)
			col |= 0xff000000;
		else
			col = PREMUL(col);
	}
	else {
		int i;
		for (i = 0; def_colors[i].str != 0; i++) {
			if (strcasecmp(def_colors[i].str, str) == 0) {
				col = def_colors[i].col;
				break;
			}
		}
		col |= 0xff000000;
	}
	return col;
}

char *match_char(char *p, const char *s)
{
	int level = 0xff000000;
	for (; *p; ++p) {
		if (*p == s[0]) ++level;
		if (*p == s[1]) --level;
		if (level & 0xff) level &= 0xffffff;
		if (!level) break;
	}
	return (*p) ? p : NULL;
}

static char *aptok_r(char *str, const char *delim, char **token)
{
	const char *d;
	char *s;
	if (!str) str = *token;
	*token = 0;
	for (s = str; *s; ++s) {
		if (*s == '[') s = match_char(s, "[]");
		if (*s == '(') s = match_char(s, "()");
		if (!s || !(*s)) break;
		d = delim;
		while (*d && *s != *d) ++d;
		if (*d) {
			*s = 0;
			*token = s + 1;
		}
		else if (*token)
			return str;
	}
	if (str && s > str) {
		*token = s;
		return str;
	}
	return 0;
}

Option::Option()
{ tag = 0, value = 0; }

Option::Option(const char *tag, const char *value)
{
	this->tag = strdup(tag);
	this->value = strdup(value);
}

Option::~Option()
{
	if (tag) free(tag);
	if (value) free(value);
	appearances.deleteAll();
}

void Option::setValue(char *value)
{
	if (this->value) free(this->value);
	this->value = strdup(value);
}

const char *Option::getValue() const
{
	return value;
}

const char *Option::getTag() const
{
	return tag;
}

const Visual &Option::getVisual()
{
	if (appearances.count() == 0)
		setVisual(value);
	return appearances;
}

static inline int percentage(int v, int ref)
{
	return v * ref / 100;
}

const Rect Option::getGeometry(const Rect &ref) const
{
	int t,l,w,h;
	t = l = w = h = 0;
	char *v = value;
	w = strtol(v, &v, 10);
	if (*v == '%') w = percentage(w, ref.width()), ++v;
	if (*v == 'x') {
		h = strtol(v + 1, &v, 10);
		if (*v == '%') h = percentage(h, ref.height()), ++v;
		if (*v == '+' || *v == '-')
			l = strtol(v, &v, 10);
		if (*v == '%') l = percentage(l, ref.width()), ++v;
	}
	else
		l = w, w = 0;
	if (*v == '+' || *v == '-')
		t = strtol(v, &v, 10);
	if (*v == '%') t = percentage(t, ref.height()), ++v;
	if (l < 0) l = ref.width() + l - w;
	if (t < 0) t = ref.height() + t - h;
	return Rect(l, t, l + w, t + h);
}

const Padding Option::getPadding() const
{
	char *tok, *p;
	int mask = 0;
	int top, left, bottom, right;

	top = left = bottom = right = 0;

	char *str = strdup(value);
	if (!(p = strtok_r(str, " ,", &tok))) goto err;
	top = strtol(p, 0, 10);
	if (strchr(p, '%'))
		mask |= PERPAD_TOP;

	if (!(p = strtok_r(NULL, " ,", &tok))) goto err;
	right = strtol(p, 0, 10);
	if (strchr(p, '%'))
		mask |= PERPAD_RIGHT;

	if ((p = strtok_r(NULL, " ,", &tok))) {
		bottom = strtol(p, 0, 10);
		if (strchr(p, '%'))
			mask |= PERPAD_BOT;

		if (!(p = strtok_r(NULL, " ,", &tok))) goto err;
		left = strtol(p, 0, 10);
		if (strchr(p, '%'))
			mask |= PERPAD_LEFT;
	}
	else {
		bottom = top;
		if (mask & PERPAD_TOP)
			mask |= PERPAD_BOT;

		right = left;
		if (mask & PERPAD_LEFT)
			mask |= PERPAD_RIGHT;
	}
	free(str);
	return Padding(mask, top, right, bottom, left);

 err:
	return Padding(0,0,0,0);
}


bool Option::getBoolean() const
{
	if (strcasecmp(value, "on"))
		return true;
	else if (strcasecmp(value, "off"))
		return false;
	else if (strcasecmp(value, "true"))
		return true;
	else if (strcasecmp(value, "false"))
		return false;
	else if (*value == '1')
		return true;
	else if (*value == '0')
		return false;
	return false;
}

void Option::setVisual(char *str)
{
	char *tok;
	char *s = str;
	s = aptok_r(s, ", ", &tok);
	while (s) {
		Appearance *app = parseAppearance(s);
		if (app)
			appearances.append(app);
		s = aptok_r(NULL, ", ", &tok);
	}
}

Appearance *Option::parseAppearance(char *str)
{
	Appearance *app = 0;
	char *p;

	if (*str == '[') {
		char *tok;
		int mask = 0;
		int top, left, bottom, right;

		top = left = bottom = right = 0;

		p = strtok_r(str, "[] ,", &tok);
		app = parseAppearance(p);

		if (!(p = strtok_r(NULL, "[] ,", &tok))) goto err;
		top = strtol(p, 0, 10);
		if (strchr(p, '%'))
			mask |= PERPAD_TOP;

		if (!(p = strtok_r(NULL, "[] ,", &tok))) goto err;
		right = strtol(p, 0, 10);
		if (strchr(p, '%'))
			mask |= PERPAD_RIGHT;

		if ((p = strtok_r(NULL, "[] ,", &tok))) {
			bottom = strtol(p, 0, 10);
			if (strchr(p, '%'))
				mask |= PERPAD_BOT;

			if (!(p = strtok_r(NULL, "[] ,", &tok))) goto err;
			left = strtol(p, 0, 10);
			if (strchr(p, '%'))
				mask |= PERPAD_LEFT;
		}
		else {
			bottom = top;
			if (mask & PERPAD_TOP)
				mask |= PERPAD_BOT;

			right = left;
			if (mask & PERPAD_LEFT)
				mask |= PERPAD_RIGHT;
		}
		app->setPadding(Padding(mask, top, right, bottom, left));
	}
	else if ((p = strstr(str, "gradient"))) {
		int direction = 0;
		char *tok;
		p = strchr(p, '(');
		if (!p) goto err;
		p = strtok_r(p, ", ()", &tok);
		if (strcmp(p, "vert") == 0)
			direction = GRAD_VERT;
		else 
			direction = GRAD_HORZ;

		app = new GradientAppearance(direction);
		GradientAppearance *gapp = dynamic_cast<GradientAppearance *>(app);

		while((p = strtok_r(NULL, ", ()", &tok)) != NULL){
			gapp->addColor(atoc(p));
		}
	}
	else if ((p = strstr(str, "image"))) {
		char *tok;
		p = strchr(p, '(');
		if (!p) goto err;

		if (!(p = strtok_r(p, "(\")", &tok))) goto err;

		Setting *setting = Insune::getSingleton()->getSetting("style");
		const char *style = setting->getValue()->getString();
		char stylepath[256];
		snprintf(stylepath, 256, STYLEDIR "%s/%s", style, p);

		char *align = NULL;
		char *type = NULL;
		if ((p = strtok_r(NULL, "(\")", &tok))) {
			if (*p == '@') {
				align = p + 1;
				if ((p = strchr(p + 1, '!'))) {
					*p = '\0';
					type = p + 1;
				}
			} else if (*p == '!') {
				type = p + 1;
				if ((p = strchr(p + 1, '@'))) {
					*p = '\0';
					align = p + 1;
				}
			}
		}
		if (align && type)
			app = new ImageAppearance(stylepath, align, type);
		else if (align)
			app = new ImageAppearance(stylepath, align);
		else if (type)
			app = new ImageAppearance(stylepath, NULL, type);
		else
			app = new ImageAppearance(stylepath);

	}

	else {
		app = new SolidAppearance(atoc(str));
	}

	return app;
err:
	fprintf(stderr, "Error parsing appearances. Value:\n```%s'''\n", str);
	return NULL;
}
