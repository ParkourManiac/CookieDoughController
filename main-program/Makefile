all: # Builds the project.
	platformio run

upload: 
	platformio run --target upload

compileTests: TestCookieDough/test.cpp
	g++ $^ -g -o TestCookieDough/Testrun/latestTestRun

test: compileTests # "Dependent" on compileTests. ("compileTests" will run before this command).
	./TestCookieDough/Testrun/latestTestRun