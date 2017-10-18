#ifndef EDAO_HOOK_H
#define EDAO_HOOK_H

#include <stdint.h>

int hook_scp_process_text(uint32_t arg1, char* text, uint32_t arg3, uint32_t arg4);


int init_hooks();
#endif //EDAO_HOOK_H