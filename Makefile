CXX = clang++
CC = ${CXX}
CXXFLAGS = -std=c++20 -Werror -Wall -Wextra
LDLIBS = -lgtest_main -lgtest

SRC = $(wildcard *.cpp)
OBJ = ${SRC:.cpp=.o}
EXE = TestVirtualData

all: ${OBJ}

test: ${EXE}
	./$^ --gtest_output=xml

check: ${SRC}
	clang-tidy $^ -- ${CPPFLAGS} ${CXXFLAGS} 

clean:
	rm --force ${EXE} ${OBJ} *.xml

${EXE}: ${OBJ}
	${LINK.o} ${OUTPUT_OPTION} $^ ${LOADLIBES} ${LDLIBS}

.PHONY: all test check clean
