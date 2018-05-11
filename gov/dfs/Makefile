OBJ := dfs
CPPS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(CPPS))
HS := $(wildcard *.h)
INC := ../../..
CXXFLAGS = -fPIC -O0 -g -std=c++17

${OBJ}.o: ${OBJS}
	ld -r -o ${OBJ}.o ${OBJS}

${OBJS}: ${HS} ${CPPS}
	g++ ${CXXFLAGS} -I${INC} -c ${CPPS}


clean:
	find . -name "*.o" -delete


