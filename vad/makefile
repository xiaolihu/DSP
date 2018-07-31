include ../defs.mk

TARGET_EXE = vadTest

INCLUDE_PATH = ../filter/inc \
	       .

CFLAGS += $(patsubst %, -I%, $(INCLUDE_PATH))

LOCAL_SRC = testVad.c \
	    iir_biquad.c \
	    vad.c \
	    vad_dled.c

VPATH = ../filter/src:.

OBJDIR = obj
OBJS = $(addprefix $(OBJDIR)/, $(LOCAL_SRC:%.c=%.o))


all : $(TARGET_EXE)

$(OBJDIR) :
	-mkdir -p $@

$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(TARGET_EXE) : $(OBJDIR) $(OBJS) 
	$(CC) -o $@ $(OBJS)

clean : 
	-rm -rf $(OBJDIR)
	-rm -rf $(TARGET_EXE)
