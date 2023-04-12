# makefile for 
# GROUP: group26
# Desc: Compiles main.cpp

CXX = g++                   				# compiler
CXXFLAGS = -std=c++17 -g -Wall -Wno-unused-function				# compiler flags
OBJECTS = main.o translator.o singlecomment.o	video.o	comment.o	analysis.o	SentimentIntensityAnalyzer.o	SentiText.o	request.o
EXEC = main                 				# name to execute
DEPENDS = ${OBJECTS:.o=.d}  				# sub objects with dependancies

########## Targets ##########

.PHONY : all clean

${EXEC} : ${OBJECTS}                		# link executable
	${CXX} ${CXXFLAGS} $^ -o $@ -lcurl
-include ${DEPENDS}

clean :                     				# remove files that can be regenerated
	rm -f ${DEPENDS} ${OBJECTS} ${EXEC}
