GNU_ARM_ROOT = $(HOME)/Apps/gcc-arm-none-eabi-9-2019-q4-major

vpath %.c src ui/src

CFLAGS += -Iinc -Iui/inc 
APP_SRCS += ui.c app.c \
Widget.c Event.c Touch.c STMPE811QTR.c