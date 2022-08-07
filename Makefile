SRC = $(wildcard *.cpp)
OBJ = ${SRC:.cpp=.o}
EXE = TestRunner

CXXFLAGS += -std=c++20 -Werror -Wall -Wextra -Wpedantic
LDLIBS += -lgtest_main -lgtest

all: ${OBJ}

test: ${EXE}
	./$^ --gtest_output=xml:results/

clean-test:
	${RM} --recursive results

clean: clean-test
	${RM} ${EXE} ${OBJ}

${EXE}: ${OBJ}
	${LINK.o} ${OUTPUT_OPTION} $^ ${LOADLIBES} ${LDLIBS}

.PHONY: all test clean-test clean

# include llvm.mk
include gcc.mk
