#include "insune.h"

static void quit()
{
	Insune::getSingleton()->exit(0);
}

INSTANCE(quit)
