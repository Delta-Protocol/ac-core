OBJ := nova
CPPS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(CPPS))
HS := $(wildcard *.h)
INC := ../..
CXXFLAGS = -fPIC -O0 -g -std=c++17

${OBJ}.o: ${OBJS} locking.o
	ld -r -o ${OBJ}.o ${OBJS} locking.o

locking.o: $(wildcard locking_programs/*.h) $(wildcard locking_programs/*.cpp)
	g++ ${CXXFLAGS} -I${INC} -c $(wildcard locking_programs/*.cpp) -o locking.o

${OBJS}: ${HS} ${CPPS}
	g++ ${CXXFLAGS} -I${INC} -c ${CPPS}


clean:
	find . -name "*.o" -delete


