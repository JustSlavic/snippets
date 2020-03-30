# project name
PROJECT=		jsonc-test

# compiler
CXX=			g++

# c++ standard
CXX_STANDARD=	c++14

# include directories
INC_DIR=		/usr/include/json-c/


# library directories
LIB_DIR=		/usr/lib/x86_64-linux-gnu/

# libraries
LIBS=			json-c \
				readline

# compilation flags
CXXFLAGS=		-Wall \
				-std=$(CXX_STANDARD)

CXXFLAGS+=  $(addprefix -I, $(INC_DIR))

LDFLAGS=    $(addprefix -L, $(LIB_DIR)) \
            $(addprefix -l, $(LIBS))


$(PROJECT): main.cpp
	g++ -o $(PROJECT) main.cpp $(LDFLAGS) $(CXXFLAGS)


clean:
	rm -f $(PROJECT)


.PHONY: clean
