TARGET  = asuka_patch
TITLE_ID = PCSG00246
PSVITAIP = 192.168.1.114

OBJS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

LIBS    = -lSceIofilemgr_stub -lSceThreadmgrForDriver_stub -lSceAppMgr_stub

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CFLAGS  = -Wl,-q -Wall -O3 -nostartfiles 
ASFLAGS = $(CFLAGS)

all: $(TARGET).suprx

%.suprx: %.velf
	vita-make-fself $< $@

%.velf: %.elf
	vita-elf-create $< $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET).suprx $(TARGET).velf $(TARGET).elf $(OBJS)

send: $(TARGET).suprx
	curl -T $(TARGET).suprx ftp://$(PSVITAIP):1337/ux0:/app/$(TITLE_ID)/sce_module/
	@echo "Sent."