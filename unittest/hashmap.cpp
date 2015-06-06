#include "hashmap.h"
#include "unitAssert.h"

const char *hashmapGetName()
{
	return "Hashmap";
}

bool hashmapUnitTest()
{
	hashmap<int, void *> map;
	for (int i = 0; i < 100; ++i)
		map[i*0x1000000] = (void *)(i * 0x1000);

	unitAssert(map.size() == 100);
	unitAssert(map.empty() == false);
	map.erase(0x32000000);
	unitAssert(map.size() == 99);
	unitAssert(map.find(0x32000000) == map.end());
	unitAssert(map[0x0E000000] == (void *)0xE000);
	map.clear();
	unitAssert(map.empty() == true);
	return true;
}
