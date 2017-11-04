#include "log.h"


#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>

//Note:
//It seems that the game folder will be mount to app0 on start, 
//and can't write anything in it.
//So we have to find a place where we have write permission.
#define LOG_FILENAME "ux0:plugins/asuka_log.txt"
#define LOG_BUFFER_LEN 4096
void debug_print(const char* format, ...)
{
	va_list list;
	char string[LOG_BUFFER_LEN] = {0};

	va_start(list, format);
	vsprintf(string, format, list);
	va_end(list);

	SceUID fd = sceIoOpen(LOG_FILENAME, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_APPEND, 0777);
	if (fd < 0) 
	{
		return;
	}

	sceIoWrite(fd, string, strlen(string));
	sceIoClose(fd);
}
