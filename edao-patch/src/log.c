#include "log.h"


#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>

#define LOG_FILENAME "ux0:asuka_log.txt"

void debug_print(const char* text, ...)
{
	va_list list;
	char string[512];

	va_start(list, text);
	vsprintf(string, text, list);
	va_end(list);

	SceUID fd = sceIoOpen(LOG_FILENAME, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_APPEND, 0777);
	if (fd < 0) 
	{
		return;
	}

	sceIoWrite(fd, string, strlen(string));
	sceIoClose(fd);
}
