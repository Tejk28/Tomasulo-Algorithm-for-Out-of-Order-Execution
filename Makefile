C = gcc
OPT = -O3
#OPT = -g
WARN = -Wall
CFLAGS = $(OPT) $(WARN) $(INC) $(LIB)

# List all your .cc files here (source files, excluding header files)
SIM_SRC = sim_proc.c pipeline.c

# List corresponding compiled object files here (.o files)
SIM_OBJ = sim_proc.o pipeline.o
 
#################################

# default rule

all: sim
	@echo "my work is done here..."


# rule for making sim

sim: $(SIM_OBJ)
	$(C) -o sim $(CFLAGS) $(SIM_OBJ) -lm
	@echo "-----------DONE WITH SIM-----------"


# generic rule for converting any .cc file to any .o file
 
.c.o:
	$(C) $(CFLAGS)  -c $*.c


# type "make clean" to remove all .o files plus the sim binary

clean:
	rm -f *.o sim


# type "make clobber" to remove all .o files (leaves sim binary)

clobber:
	rm -f *.o


