#include "insune.h"
#include <unistd.h>

static void reboot()
{
#ifdef IPOD
	Insune::getSingleton()->dispose();
	execl("/bin/reboot", "reboot", NULL);
	exit(0);
#else
	warn("Stubbornly refusing to reboot your desktop...\n");
#endif
}

INSTANCE(reboot)
