
#include "crosslog.h"

#ifdef WIN32


#else
    CSLog g_imlog = CSLog(LOG_MODULE_IM);
#endif
