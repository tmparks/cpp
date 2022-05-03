# Use GCC tools.
CXX = g++
CC = ${CXX}

coverage: CXXFLAGS += -ftest-coverage -fprofile-arcs
coverage: LDFLAGS += -ftest-coverage -fprofile-arcs
coverage: test
	gcovr --xml=results/ --html-details=results/ --txt

check: ${SRC}
	clang-tidy $^ -- ${CPPFLAGS} ${CXXFLAGS} 

clean-coverage:
	${RM} --recursive results
	${RM} ${SRC:.cpp=.gcda} ${SRC:.cpp=.gcno}

clean: clean-coverage

.PHONY: coverage check clean-coverage clean
