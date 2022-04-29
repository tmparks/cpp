CXX = clang++
CC = ${CXX}
CXXFLAGS = -std=c++20 -Werror -Wall -Wextra
LDLIBS = -lgtest_main -lgtest

SRC = $(wildcard *.cpp)
OBJ = ${SRC:.cpp=.o}
EXE = TestVirtualDataPtr TestVirtualDataRef

all: ${OBJ}

test: ${EXE}
	for test in $^; do ./$$test || exit; done

check: ${SRC}
	clang-tidy $^ -- ${CPPFLAGS} ${CXXFLAGS} 

clean:
	rm --force ${EXE} ${OBJ}

TestVirtualDataPtr: Array.o StretchableArray.o Verbose.o

TestVirtualDataRef: Array.o StretchableArray.o Verbose.o

Test%: %.o
	${CC} ${LDFLAGS} --output $@ $^ ${LOADLIBES} ${LDLIBS}

.PHONY: all test check clean
