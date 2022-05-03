# Use LLVM tools.
CXX = clang++
CC = ${CXX}

# See [Source Based Code Coverage](https://clang.llvm.org/docs/SourceBasedCodeCoverage.html)
coverage: CXXFLAGS += -fprofile-instr-generate -fcoverage-mapping
coverage: LDFLAGS += -fprofile-instr-generate -fcoverage-mapping
coverage: test
	llvm-profdata merge -sparse default.profraw -o default.profdata
	llvm-cov show -format=html -output-dir=results ./${EXE} -instr-profile=default.profdata

check: ${SRC}
	clang-tidy $^ -- ${CPPFLAGS} ${CXXFLAGS} 

clean-coverage:
	${RM} --recursive results
	${RM} default.profraw default.profdata

clean: clean-coverage

.PHONY: coverage check clean-coverage clean
