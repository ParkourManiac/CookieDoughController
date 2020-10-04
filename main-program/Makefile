GCC_EXTRA_WARNINGS = -Wpacked -Wpadded -Wmissing-declarations -Wsign-conversion
GCC-WARNINGS ?= -Wall -Weffc++ -pedantic -pedantic-errors -Wextra -Wcast-align -Wcast-qual -Wconversion -Wdisabled-optimization -Wfloat-equal -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport -Winit-self -Winline -Winvalid-pch -Wlong-long -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wpointer-arith -Wredundant-decls -Wshadow -Wstack-protector -Wstrict-aliasing=2 -Wswitch-default -Wswitch-enum -Wunreachable-code -Wunused -Wunused-parameter -Wvariadic-macros -Wwrite-strings -Wctor-dtor-privacy -Wlogical-op -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wstrict-null-sentinel -Wundef -Wno-unused 
ifdef USE-ALLOC-TRACKER
ALLOC-TRACKER = -DUSE_ALLOCATION_TRACKER_H
endif
ALLOC-TRACKER ?= 
STD-VERSION ?= c++11

all: # Builds the project.
	platformio run

upload: # Builds and uploads the project to the targeted arduino.
	platformio run --target upload

# Name of command: Dependent on file. ($^ -The dependent file. -g -Keep symbols for debugging. -o -Output file)
compileTests: TestCookieDough
	python $^/generateTestSuite.py TestCookieDough && \
	g++ -ITestCookieDough/Fakes -std=$(STD-VERSION) $(ALLOC-TRACKER) $(GCC-WARNINGS) -g -D "private=public" \
		-D "TEST_ENVIRONMENT" \
		-D "DEBUG(VALUE)= " \
		-D "DEBUG_PRINT(VALUE)= " \
		-o $^/Testrun/latestTestRun $^/*.cpp $^/TestHelper/*.cpp \
		./lib/CookieDoughLibrary/*.cpp

test: compileTests # "Dependent" on compileTests. ("compileTests" will run before this command).
	./TestCookieDough/Testrun/latestTestRun

testMemory:
	make test STD-VERSION=c++14 USE-ALLOC-TRACKER=1

lint:
	cpplint --recursive \
	src \
	lib

check: # TODO: Unsure if this actually works. Might not be configured correctly.
		# -IC:/MyInstalledPrograms/MinGW/lib/gcc/mingw32/9.2.0/include
		# -IC:/MyInstalledPrograms/MinGW/lib/gcc/mingw32/9.2.0/include-fixed
		# ./TestCookieDough/*
	cppcheck --quiet --platform=avr8 --check-library --enable=all --max-ctu-depth=100 --language=c++ --file-filter=*.cpp --std=$(STD-VERSION) $(ALLOC-TRACKER) -D "private=public" \
		-Ilib/CookieDoughLibrary \
		-ITestCookieDough/Fakes \
		-D "TEST_ENVIRONMENT" \
		-D "DEBUG(VALUE)= " \
		-D "DEBUG_PRINT(VALUE)= " \
		./src/* \
		./lib/CookieDoughLibrary/*

# TODO: Why is it linting the visual studio library???
# TODO: Complete this command
tidy: TestCookieDough
	python $^/generateTestSuite.py TestCookieDough && \
	clang-tidy --checks=* $^/test.cpp -- 
		# -ITestCookieDough/Fakes -extra-args=-std=$(STD-VERSION) $(ALLOC-TRACKER) -g -D "private=public" \
		# -D "TEST_ENVIRONMENT" \
		# -D "DEBUG(VALUE)= " \
		# -D "DEBUG_PRINT(VALUE)= "