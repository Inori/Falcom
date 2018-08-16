#ifndef EDAO_HOOK_H
#define EDAO_HOOK_H

#include <stdint.h>


#include "log.h"
#include "tools.h"
#include "translator.h"


extern TL_CONTEXT g_tl_context_scena;
extern TL_CONTEXT g_tl_context_sys;

int init_hooks();


#endif //EDAO_HOOK_H