include ../defs.mk

TARGET_EXE = filterTest

INCLUDE_PATH = ./inc \
	       .

CFLAGS += $(patsubst %, -I%, $(INCLUDE_PATH))

LOCAL_SRC = main_test.c \
	    iir_biquad.c \
	    resample.c \
	    rfir.c \
	    vectorutils.c \
	    decimation.c \
	    notch.c

VPATH = src:coeffs:.

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
