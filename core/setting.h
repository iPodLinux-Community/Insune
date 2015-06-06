#ifndef _SETTING_H_
#define _SETTING_H_

#include <stdio.h>
#include "list.h"

class Value
{
 public:
	Value(const char *string);
	Value(int integer);
	Value(double decimal);
	Value(void *data, int length);
	~Value();

	void write(FILE *fp) const;
	static Value *read(const char *line);

	void *getData() const;
	double getDecimal() const;
	int getInteger() const;
	const char *getString() const;

 private:
 
 	int type;
	union {
		char *string;
		int integer;
		double decimal;
		void *data;
	} value;
	int size;
};

class Setting
{
 public:
	Setting(const char *k, Value *v);
	~Setting();

	void setValue(Value *v);
	const Value *getValue() const;
	const char *getKey() const;

	void write(FILE *fp) const;
	static Setting *read(FILE *fp);

 private:
 	char *key;
	Value *value;
};

class SettingFile
{
 public:
	SettingFile();
	SettingFile(const char *filename);
	~SettingFile();

	void save();
	void load(const char *filename);

	Setting *getSetting(const char *key) const;
	void addSetting(Setting *s);
	void setSetting(const char *key, Value *value);

 private:
	char *filename;
	bool changed;
	list<Setting *> settings;
};

#endif
