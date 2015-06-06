#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "setting.h"
#include "insune.h"

#define VALUE_STRING  1
#define VALUE_INTEGER 2
#define VALUE_DECIMAL 3
#define VALUE_DATA    4

Value::Value(const char *string)
{
	type = VALUE_STRING;
	if (string != NULL)
		value.string = strdup(string);
	else value.string = NULL;
}

Value::Value(int integer)
{
	type = VALUE_INTEGER;
	value.integer = integer;
}

Value::Value(double decimal)
{
	type = VALUE_DECIMAL;
	value.decimal = decimal;
}

Value::Value(void *data, int length)
{
	type = VALUE_DATA;
	if (data != NULL) {
		value.data = xmalloc(length);
		memcpy(value.data, data, length);
		size = length;
	}
	else value.data = NULL;
}

Value::~Value()
{
	switch (type) {
	case VALUE_STRING:
		if (value.string != NULL)
			free(value.string);
		break;
	case VALUE_DATA:
		if (value.data != NULL)
			free(value.data);
		break;
	default:break;
	}
}

void Value::write(FILE *fp) const
{
	switch (type) {
	case VALUE_STRING:
		fprintf(fp, "S%s", value.string);
		break;
	case VALUE_INTEGER:
		fprintf(fp, "I%d", value.integer);
		break;
	case VALUE_DECIMAL:
		fprintf(fp, "D%g", value.decimal);
		break;
	case VALUE_DATA:
		fprintf(fp, "*");
		for (int i = 0; i < size; ++i)
			fprintf(fp, "%02x",
				((unsigned char *)value.data)[i]);
		break;
	default:break;
	}
}

unsigned char atodata(const char *st)
{
	unsigned char ret = 0;
	for (int i = 0; i < 2; ++i) {
		ret <<= 4;
		if (st[i] >= '0' && st[i] <= '9')
			ret += st[i] - '0';
		else if (st[i] >= 'a' && st[i] <= 'f')
			ret += st[i] - 'a' + 0xa;
		else if (st[i] >= 'A' && st[i] <= 'F')
			ret += st[i] - 'A' + 0xa;
	}
	return ret;
}

Value *Value::read(const char *string)
{
	Value *ret = NULL;
	int length;
	void *data;
	switch (string[0]) {
	case 'S':
		ret = new Value(string + 1);
		break;
	case 'I':
		ret = new Value(atoi(string + 1));
		break;
	case 'D':
		ret = new Value(strtod(string + 1, 0));
		break;
	case '*':
		length = strlen(string + 1) / 2;
		data = xmalloc(length);
		for (int i = 0; i < length; ++i)
			((unsigned char *)data)[i] = atodata(string + 1 + i*2);
		ret = new Value(data, length);
		break;
	default:break;
	}
	return ret;
}

void *Value::getData() const
{
	return value.data;
}

double Value::getDecimal() const
{
	return value.decimal;
}

int Value::getInteger() const
{
	return value.integer;
}

const char *Value::getString() const
{
	return value.string;
}

Setting::Setting(const char *k, Value *v)
 : value(v)
{
	if (k)
		key = strdup(k);
	else key = NULL;
}

Setting::~Setting()
{
	if (key)
		free((void *)key);
	delete value;
}

void Setting::setValue(Value *v)
{
	delete value;
	value = v;
}

const Value *Setting::getValue() const
{
	return value;
}

const char *Setting::getKey() const
{
	return key;
}

void Setting::write(FILE *fp) const
{
	fprintf(fp, "%s: ", key);
	value->write(fp);
	fprintf(fp, "\n");
}

Setting *Setting::read(FILE *fp)
{
	Setting *ret = NULL;

	int size = 2048, pos = 0;
	char *buf = NULL;
	char *eof;

	do {
		size *= 2;
		buf = (char *)realloc((void *)buf, size);
		buf[pos] = 0;
		if ((eof = fgets(buf + pos, size - pos, fp)))
			pos = strlen(buf);
	} while (eof && !strrchr(buf, '\n'));

	int len = strchr(buf, ':') - buf + 1;
	if (len > 0) {
		char tag[512];
		buf[strlen(buf) - 1] = 0;
		strncpy(tag, buf, len - 1);
		tag[len - 1] = 0;
		ret = new Setting(tag, Value::read(buf + len + 1));
	}

	free((void *)buf);

	return ret;
}


SettingFile::SettingFile()
 : filename(NULL), changed(true)
{ }

SettingFile::SettingFile(const char *filename)
 : filename(NULL), changed(true)
{
	load(filename);
}

SettingFile::~SettingFile()
{
	free(filename);
}

void SettingFile::save()
{
	if (!changed || !filename)
		return;
	FILE *fp = fopen(filename, "w");
	if (!fp) {
		perror(filename);
		return;
	}
	list<Setting *>::iterator it = settings.start();
	for (; it != settings.end(); ++it)
		(*it)->write(fp);

	fclose(fp);
}

void SettingFile::load(const char *filename)
{
	if (!filename)
		return;
	settings.destroy();
	if (this->filename)
		free(this->filename);
	this->filename = strdup(filename);
	Setting *setting;
	FILE *fp = fopen(filename, "r");
	if (!fp) return;
	while ((setting = Setting::read(fp)))
		settings.push(setting);
	fclose(fp);
	changed = false;
}

Setting *SettingFile::getSetting(const char *key) const
{
	list<Setting *>::iterator it = settings.start();
	for (; it != settings.end(); ++it)
		if (!strcmp(key, (*it)->getKey()))
			return (*it);
	return NULL;
}

void SettingFile::addSetting(Setting *setting)
{
	settings.push(setting);
	changed = true;
}

void SettingFile::setSetting(const char *key, Value *value)
{
	list<Setting *>::iterator it = settings.start();
	for (; it != settings.end(); ++it)
		if (!strcmp(key, (*it)->getKey()))
			(*it)->setValue(value);
	changed = true;
}

