
bin_PROGRAMS = evolver

evolver_SOURCES = 				\
	ArticulatedBody.cpp		\
	Controller.cpp			\
	RibRenderer.cpp			\
	NeatEvolver.cpp			\
	PhysicsSimulator.cpp		\
	Tardis.cpp			\
	Utils.cpp			\
	EvolverCmdLine.cpp
	

evolver_LDADD =				\
	neat/libneat.a

SUBDIRS = neat

# -framework Carbon -framework ApplicationServices -lsupc++ `fltk-config --use-gl --ldstaticflags`
LIBS =  -lode -L/Library/Pixie/lib -lri

# `fltk-config --cxxflags` 

AM_CXXFLAGS = -fpermissive

INCLUDES = -I@top_srcdir@/src/ -I/usr/local/include/gmtl-0.6.0/ -I/Library/Pixie/include
