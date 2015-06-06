#include "insune.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define inl(p) (*(volatile unsigned long *) (p))
#define outl(v,p) (*(volatile unsigned long *) (p) = (v))
 
#ifdef IPOD
static void reboot()
{
	Insune::getSingleton()->dispose();
	execl("/bin/reboot", "reboot", NULL);
	exit(0);
}
#endif

static void diskmode()
{
#ifdef IPOD
	unsigned char * storage_ptr;
	unsigned int iram_len;

	if (Insune::getSingleton()->getVersion() < 0x40000) {
		printf("You must force diskmode on this iPod.\n");
		reboot();
	}
	iram_len = ((((inl(0x70000000) << 8) >> 24) == 0x32) ? 128 : 96) * 1024;
	storage_ptr = (unsigned char *)0x40000000 + iram_len - 0x100;

	memcpy(storage_ptr, "diskmode\0\0hotstuff\0\0\1", 21);
	sleep(1);
	reboot();
#else
	warn("Stubbornly refusing to reboot your desktop...\n");
#endif
}

INSTANCE(diskmode)
