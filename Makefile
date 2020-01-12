CPP_FLAGS=-arch x86_64 -m64
DEBUG=-g -DDEBUG
OPT=-O2
GCCSTACK=-mpreferred-stack-boundary=32
CLANGSTACK=-fdiagnostics-show-option
BIN_DIR=./
LIB_DIR=./
LST_DIR=./

all:	clean optimized debug X Xdbg

release:clean optimized X
	rm -fR $(BIN_DIR)*.o
	rm -fR $(BIN_DIR)*.lst

X:
	nasm -f macho64 julia.asm -o $(BIN_DIR)julia_a.o
	g++ -c $(OPT) $(CPP_FLAGS) -I/opt/X11/include xjulia.cpp -o $(BIN_DIR)xjulia_cpp.o
	g++ -Wl,-no_pie $(OPT) $(CPP_FLAGS) -L/opt/X11/lib/ -lX11 $(BIN_DIR)xjulia_cpp.o $(BIN_DIR)julia_a.o -o $(BIN_DIR)xjulia
	strip -no_uuid -A -u -S -X -N -x $(BIN_DIR)xjulia

Xdbg:
	nasm -f macho64 -g -DDEBUG -l $(BIN_DIR)julia_a.lst julia.asm -o $(BIN_DIR)julia_a-dbg.o
	g++ -c  $(DEBUG) $(CPP_FLAGS) -I/opt/X11/include xjulia.cpp -o $(BIN_DIR)xjulia_cpp-dbg.o
	g++ -Wl,-no_pie $(DEBUG) $(CPP_FLAGS) -L/opt/X11/lib/ -lX11 $(BIN_DIR)julia_a-dbg.o $(BIN_DIR)xjulia_cpp-dbg.o -o $(BIN_DIR)xjulia-dbg

optimized: 
	nasm -f macho64 julia.asm -o $(BIN_DIR)julia_a.o
	g++ -c $(OPT) $(CPP_FLAGS) julia.cpp -o $(BIN_DIR)julia_cpp.o
	g++ -Wl,-no_pie $(OPT) $(CPP_FLAGS) $(BIN_DIR)julia_cpp.o $(BIN_DIR)julia_a.o -o $(BIN_DIR)julia 
	strip -no_uuid -A -u -S -X -N -x $(BIN_DIR)julia

debug:
	nasm -f macho64 -g -DDEBUG -l $(BIN_DIR)julia_a.lst julia.asm -o $(BIN_DIR)julia_a-dbg.o
	g++ -c $(DEBUG) $(CPP_FLAGS) julia.cpp -o $(BIN_DIR)julia_cpp-dbg.o 
	g++ -Wl,-no_pie $(DEBUG) $(CPP_FLAGS) $(BIN_DIR)julia_cpp-dbg.o $(BIN_DIR)julia_a-dbg.o -o $(BIN_DIR)julia-dbg
	dsymutil $(BIN_DIR)julia-dbg

clean : 
	rm -fR $(BIN_DIR)julia
	rm -fR $(BIN_DIR)julia-dbg
	rm -fR $(BIN_DIR)xjulia
	rm -fR $(BIN_DIR)xjulia-dbg
	rm -fR $(BIN_DIR)*.*~
	rm -fR $(BIN_DIR)Makefile~
	rm -fR $(BIN_DIR)*.o
	rm -fR $(BIN_DIR)*.lst
	rm -fR $(BIN_DIR)julia-dbg.dSYM
