#include "insune.h"
#include <unistd.h>

static void poweroff()
{
#ifdef IPOD
	Insune::getSingleton()->dispose();
	printf("\nPowering down.\nPress action to power on.\n");
	execl("/bin/poweroff", "poweroff", NULL);
	printf("No poweroff binary available. Rebooting.\n");
	execl("/bin/reboot", "reboot", NULL);
	exit(0);
#else
	warn("Stubbornly refusing to halt your desktop...\n");
#endif
}

INSTANCE(poweroff)
