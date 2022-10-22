# To compile and use valgrind, using the Nvidia drivers,
# set the environment variable:
#       export LD_ASSUME_KERNEL=2.3.98

CC = gcc
# CFLAGS = -O2 -Wall
CFLAGS = -g -Wall
LDFLAGS =  -lGLU -lGL -lm -lX11
# LDFLAGS =  -lm
OBJECTS = matrices.o vertices.o lists.o faces.o edges.o geometry.o objParser.o normals3D.o tools.o
GLOBJECTS = glMain.o glDisplay.o glObjectLists.o glObjDisplay.o glScreenCapture.o glTools.o glTextures.o

MAIN = objReader


all: $(MAIN)

$(MAIN): $(MAIN).o $(OBJECTS) $(GLOBJECTS)
	$(CC) $(CFLAGS) \
           -o $@ $^ $(LDFLAGS)
#	rm -f *.o

tags:
	ctags *.c *.h

clean:
	-rm -f *.o $(MAIN)


# Debugging using Valgrind

# SAMPLEFILE = Data/Q-be/cube-05.obj
# SAMPLEFILE = Data/Tori/torus_smooth.obj
SAMPLEFILE = Data/Tori/torus_final.obj
# SAMPLEFILE = Data/Tori/torus_orig.obj
# SAMPLEFILE = Data/Objects/object-08-3.obj 1
# SAMPLEFILE = Data/weirdTriangle.obj 1
# SAMPLEFILE = Data/Cones/cone04-1.obj 1
# SAMPLEFILE = Data/Saddles/monkey-saddle.obj 1
EXECUTABLE = $(MAIN) $(SAMPLEFILE)
VALGRINDFLAGS = --suppressions=suppresedErrors.supp --leak-check=yes --show-reachable=yes --num-callers=6
GDBATTACH = --gdb-attach=yes --gdb-path=/usr/bin/gdb

check:
	valgrind $(VALGRINDFLAGS) ./$(EXECUTABLE)

debug:
	valgrind $(VALGRINDFLAGS) $(GDBATTACH) ./$(EXECUTABLE)

.PHONY: all clean tags
