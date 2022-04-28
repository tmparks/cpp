CXX = clang++
CC = ${CXX}
CXXFLAGS = -std=c++20 -Werror -Wall -Wextra
LDLIBS = -lgtest_main -lgtest

SRC = \
	Array.cpp \
	StretchableArray.cpp \
	Verbose.cpp \
	VirtualDataPtr.cpp \
	VirtualDataRef.cpp

OBJ = ${SRC:.cpp=.o}

TEST = TestVirtualDataPtr TestVirtualDataRef

all: test

test: ${TEST}
	for test in $^; do ./$$test || exit; done

check: ${SRC}
	clang-tidy $^ -- ${CPPFLAGS} ${CXXFLAGS} 

clean:
	rm -f ${TEST} ${OBJ}

TestVirtualDataPtr: Array.o StretchableArray.o Verbose.o

TestVirtualDataRef: Array.o StretchableArray.o Verbose.o

Test%: %.o
	${CC} ${LDFLAGS} --output $@ $^ ${LOADLIBES} ${LDLIBS}

.PHONY: all test check clean
