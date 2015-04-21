# Makefile for CSC 541 Assignment 4 - B-Trees

# Source, data, and include folders
SRCDIR = ./src
IDIR = ./include
ODIR = ./obj
LDIR = ./lib

# Compile params
CC = g++
CFLAGS = -Wall -I $(IDIR)
_DEPS = main.h
DEPS = $(patsubst %, $(IDIR)/$, $(_DEPS))

_OBJ = main.o
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))
EXEC = assn_4

EXDIR = ./data/examples
TEST_RESULTS = test_results.txt
INPUT01A = input-01a.txt
INPUT02A = input-02a.txt
OUTPUT01A = output-01a.txt
OUTPUT02A = output-02a.txt
MY_OUTPUT01A = my-output-01a.txt
MY_OUTPUT02A = my-output-02a.txt

$(ODIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

assn_4: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

all: assn_4

clean:
	rm -rf $(ODIR) *.bin

test: assn_4 clean
	# Input 01a
	./$(EXEC) index.bin 4 < $(EXDIR)/$(INPUT01A) > $(MY_OUTPUT01A)
	echo "--- TEST ON INPUT 01A ---" >> $(TEST_RESULTS)
	echo "--- DIFF OF OUTPUT BASED ON INPUT 01A:" >> $(TEST_RESULTS)
	-diff $(MY_OUTPUT01A) $(EXDIR)/$(OUTPUT01A) >> $(TEST_RESULTS)
	./$(EXEC) index.bin 4 < $(EXDIR)/$(INPUT02A) > $(MY_OUTPUT02A)
	echo "--- TEST ON INPUT 02A ---" >> $(TEST_RESULTS)
	echo "--- DIFF OF OUTPUT BASED ON INPUT 02A:" >> $(TEST_RESULTS)
	-diff $(MY_OUTPUT02A) $(EXDIR)/$(OUTPUT02A) >> $(TEST_RESULTS)