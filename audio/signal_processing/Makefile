TARGET = libsgl.a
PREFIX_BIN =

CC = gcc
CXX = g++
INCLUDES = -Iinterface -I.. -I../signal_processing/include
LIBS = 
CFLAGS = -Wall -Werror -O2
LINKFLAGS=

#C_SOURCES = $(wildcard *.c)
C_SOURCES = \
    auto_corr_to_refl_coef.c \
    auto_correlation.c \
    complex_fft.c \
    copy_set_operations.c \
    division_operations.c \
    dot_product_with_scale.c \
    energy.c \
    filter_ar.c \
    filter_ma_fast_q12.c \
    get_hanning_window.c \
    get_scaling_square.c \
    ilbc_specific_functions.c \
    levinson_durbin.c \
    lpc_to_refl_coef.c \
    min_max_operations.c \
    randomization_functions.c \
    refl_coef_to_lpc.c \
    resample.c \
    resample_48khz.c \
    resample_by_2.c \
    resample_by_2_internal.c \
    resample_fractional.c \
    spl_sqrt.c \
    spl_version.c \
    splitting_filter.c \
    sqrt_of_one_minus_x_squared.c \
    vector_scaling_operations.c \
    cross_correlation.c \
    downsample_fast.c \
    filter_ar_fast_q12.c \
    complex_bit_reverse.c \
    spl_sqrt_floor.c
C_OBJS = $(patsubst %.c, %.o, $(C_SOURCES))

CPP_SOURCES = 
CPP_OBJS = $(patsubst %.cpp, %.o, $(CPP_SOURCES))

.c.o:
	$(CC) -c -o $*.o $(CFLAGS) $(INCLUDES) $*.c
.cpp.o:
	$(CXX) -c -o $*.o $(CFLAGS) $(INCLUDES) $*.cpp

compile: $(CPP_OBJS) $(C_OBJS)
	ar crs $(TARGET) $^

clean:
	rm -f $(CPP_OBJS) $(C_OBJS)
	rm -f $(TARGET)

install: $(TARGET)
	cp $(TARGET) $(PREFIX_BIN)

uninstall:
	rm -f $(PREFIX)/$(PREFIX_BIN)

rebuild: clean compile 
