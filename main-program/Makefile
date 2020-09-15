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
	g++ -ITestCookieDough/Fakes -std=$(STD-VERSION) $(ALLOC-TRACKER) -g -D "private=public" \
		-D "DEBUG(VALUE)= " \
		-D "DEBUG_PRINT(VALUE)= " \
		-o $^/Testrun/latestTestRun $^/*.cpp $^/TestHelper/*.cpp \
		./lib/CookieDoughLibrary/LinkedList.cpp \
		./lib/CookieDoughLibrary/Key.cpp \
		./lib/CookieDoughLibrary/EditMode.cpp \
		./lib/CookieDoughLibrary/DataPacket.cpp \
		./lib/CookieDoughLibrary/Controller.cpp

test: compileTests # "Dependent" on compileTests. ("compileTests" will run before this command).
	./TestCookieDough/Testrun/latestTestRun

testWithMemoryCheck:
	make test STD-VERSION=c++14 USE-ALLOC-TRACKER=1

lint:
	cpplint --recursive \
	src \
	lib