all: # Builds the project.
	platformio run

upload: # Builds and uploads the project to the targeted arduino.
	platformio run --target upload

# Name of command: Dependent on file. ($^ -The dependent file. -g -Keep symbols for debugging. -o -Output file)
compileTests: TestCookieDough
	python $^/generateTestSuite.py TestCookieDough && \
	g++ -Dprivate=public -ITestCookieDough/Fakes -std=c++11 -g \
		-o $^/Testrun/latestTestRun $^/*.cpp $^/TestHelper/*.cpp \
		./lib/CookieDoughLibrary/LinkedList.cpp \
		./lib/CookieDoughLibrary/Key.cpp \
		./lib/CookieDoughLibrary/EditMode.cpp \
		./lib/CookieDoughLibrary/DataPacket.cpp \
		./lib/CookieDoughLibrary/Controller.cpp


test: compileTests # "Dependent" on compileTests. ("compileTests" will run before this command).
	./TestCookieDough/Testrun/latestTestRun
