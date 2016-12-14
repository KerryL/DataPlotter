# makefile.inc (DataPlotter)
# This file contains all of the environment definitions
# common to each of the makefiles within the project. 
#

# Include directories that are not already on the path
# DO NOT include the -I prefix to these paths - it will
# be added automatically
INCDIRS_TEMP = \
	$(CURDIR)/src \
	$(CURDIR)/../LibPlot2D/include

INCDIRS = $(addprefix -I,$(INCDIRS_TEMP))

# Library directories that are not already on the path
# DO NOT include the -L prefix to these paths - it will
# be added automatically
LIBDIRS_TEMP = \
	$(CURDIR)/../LibPlot2D/lib

LIBDIRS = $(addprefix -L,$(LIBDIRS_TEMP))

# Libraries to link against
# DO NOT include the -l prefix to these libraries - it
# will be added automatically
LIBS_TEMP = \
	Plot2d

LIBS_TEMP_D = \
	Plot2d_d

LIBS = $(addprefix -l,$(LIBS_TEMP))
LIBS_D = $(addprefix -l,$(LIBS_TEMP_D))

# Static libraries to be built before the executable
# MUST be listed in order of dependence (i.e. first
# library must not be needed by other libraries and
# it must need information contained in the following
# libraries).
PSLIB = 

# Compiler to use
CC = g++

# Archiver to use
AR = ar rcs
RANLIB = ranlib

# Compiler flags
CFLAGS = -Wall -Wextra $(INCDIRS) `pkg-config --cflags freetype2` -DwxUSE_GUI=1 -Wno-unused-local-typedefs -std=c++14
CFLAGS_RELEASE = $(CFLAGS) -O2 `wx-config --version=3.1 --debug=no --cppflags`
CFLAGS_DEBUG = $(CFLAGS) -g `wx-config --version=3.1 --debug=yes --cppflags`

# Linker flags
LDFLAGS = $(LIBDIRS) $(LIBS) `pkg-config --libs gl,glew,freetype2`
LDFLAGS_D = $(LIBDIRS) $(LIBS_D) `pkg-config --libs gl,glew,freetype2`
LDFLAGS_DEBUG = $(LDFLAGS) `wx-config --version=3.1 --debug=yes --libs all`
LDFLAGS_RELEASE = $(LDFLAGS) `wx-config --version=3.1 --debug=no --libs all`

# Object file output directory
OBJDIR = $(CURDIR)/.obj/
OBJDIR_DEBUG = $(OBJDIR)debug/
OBJDIR_RELEASE = $(OBJDIR)release/

# Binary file output directory
BINDIR = $(CURDIR)/bin/

# Library output directory
LIBOUTDIR = $(CURDIR)/.lib/

# Method for creating directories
MKDIR = mkdir -p

# Method for removing files
RM = rm -f
