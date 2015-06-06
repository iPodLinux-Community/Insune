#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "style.h"

extern char *match_char(char *p, const char *s);

Style::Style(const char *name)
 : name(0)
{
	setName(name);
}

Style::~Style()
{
	children.deleteAll();
	options.deleteAll();
	if (name)
		free(name);
}

void Style::setName(const char *n)
{
	if (name)
		free(name);
	if (n)
		name = strdup(n);
	else
		name = 0;
}

const char *Style::getName() const
{
	return name;
}

Option *Style::getOption(const char *tag) const
{
	list<Option *>::iterator it = options.start();
	for (; it != options.end(); ++it) {
		if (strcmp(tag, (*it)->getTag()) == 0)
			return (*it);
	}
	return NULL;
}

const Style *Style::findStyle(const char *re) const
{
	char *tok, *t;
	const Style *s = this;

	char *str = strdup(re);
	t = strtok_r(str, "*/", &tok);
	do {
		list<Style *>::iterator it = s->children.start();
		for (; it != s->children.end(); ++it) {
			if (strcmp(t, (*it)->getName()) == 0) {
				s = (*it);
				break;
			}
		}
		if (it == s->children.end()) {
			free(str);
			return NULL;
		}

	} while ((t = strtok_r(NULL, "*/", &tok)));
	free(str);
	return s;

}

Option *Style::resolve(const char *re) const
{
	Option *o;
	char *tok, *t;
	const Style *s = this;

	char *str = strdup(re);
	t = strtok_r(str, "*/", &tok);
	do {
		list<Style *>::iterator it = s->children.start();
		for (; it != s->children.end(); ++it) {
			if (strcmp(t, (*it)->getName()) == 0) {
				s = (*it);
				break;
			}
		}
		if (it != s->children.end()) continue;

		o = s->getOption(t);
		free(str);
		return o;
	} while ((t = strtok_r(NULL, "*/", &tok)));
	free(str);
	return NULL;
}

Style *Style::parseSection(char *name, char *buf, long len)
{
	char *p, *ep, *s_name;
	int s_len;
	Style *n = NULL, *ch;

	while (isspace(*buf)) ++buf, --len;
	if (len <= 0) return NULL;

	p = buf;
	while (p < buf + len) {
		while (isspace(*p)) ++p;
		s_len = strcspn(p, "{:");

		if (p + s_len > buf + len || s_len == 0)
			return n;

		ep = p + s_len;
		while (isspace(*ep) || *ep == '{' || *ep == ':') --ep;
		++ep;
		s_name = (char *)xmalloc(ep - p + 1);
		strncpy(s_name, p, ep - p);
		s_name[ep - p] = '\0';
		p += s_len;
		if (*p == '{') {
			ep = match_char(p, "{}");
			if (!ep || ep > buf + len) {
				fprintf(stderr, "Parse error in style\n");
				return NULL;
			}
			if (!n)
				n = new Style(name);
			if ((ch = parseSection(s_name, ++p, ep - p))) {
				n->children.push(ch);
			}
			free(s_name);
			p = ep + 1;
		}
		else if (*p == ':') {
			ep = strchr(p, ';');
			if (!ep || ep > buf + len) {
				fprintf(stderr, "Parse error in style\n");
				return NULL;
			}
			if (!n)
				n = new Style(name);
			++p;
			s_len = 0;
			while (isspace(*ep - s_len)) ++s_len;
			while (isspace(*p)) ++p;
			s_len = (ep - s_len) - p;
			if (s_len > 0) {
				char *value = (char *)xmalloc(s_len + 1);
				strncpy(value, p, s_len);
				value[s_len] = '\0';
				n->options.push(new Option(s_name,value));
				free(value);
				free(s_name);
			}
			p = ep + 1;
		}
	}
	return n;
}

Style *Style::parse(const char *filename)
{
	Style *style;
	FILE *fp;
	long len, cur;
	char *buf;

	if ((fp = fopen(filename, "r")) == NULL) {
		perror(filename);
		return NULL;
	}

	fseek(fp, 0L, SEEK_END);
	cur = len = ftell(fp);
	rewind(fp);

	if ((buf = (char *)calloc(1, len + 1)) == NULL) {
		fprintf(stderr, "Unable to malloc style space");
		return NULL;
	}

	while (cur > 0)
		cur -= fread(buf + (len - cur), 1, cur, fp);
	fclose(fp);

	buf[len] = 0;

	style = parseSection(NULL, buf, len);

	free(buf);

	return style;
}
