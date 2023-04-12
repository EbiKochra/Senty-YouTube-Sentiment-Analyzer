

CXX = g++                   				# compiler
IMGUI_DIR = ../imgui
CXXFLAGS = -std=c++17 -g -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -Wall -Wno-unused-function -Wno-unused-command-line-argument -pthread -lbcrypt -DSTB_IMAGE_IMPLEMENTATION 	 			# compiler flags

SOURCES = main.cpp \
          translator.cpp \
          singlecomment.cpp \
          video.cpp \
          commentSect.cpp \
          analysis.cpp \
          request.cpp \
          account.cpp \
          accountManager.cpp \
          videoManager.cpp \
          appModel.cpp \
          gui.cpp \
		  SentimentIntensityAnalyzer.cpp \
		  SentiText.cpp

OBJECTS = $(SOURCES:.cpp=.o)

IMGUI_SOURCES = $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp \
                $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl2.cpp

IMGUI_OBJECTS = $(IMGUI_SOURCES:.cpp=.o)

EXEC = main                 				# name to execute
UNAME_S := $(shell uname -s)

LIBS =

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += -lGL `pkg-config --static --libs glfw3`

	CXXFLAGS += `pkg-config --cflags glfw3`
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	LIBS += -L/usr/local/lib -L/opt/local/lib -L/opt/homebrew/lib
	LIBS += -lglfw

	CXXFLAGS += -I/usr/local/include -I/opt/local/include -I/opt/homebrew/include 
endif

ifeq ($(OS), Windows_NT)
	ECHO_MESSAGE = "MinGW"
	LIBS += -lglfw3 -lgdi32 -lopengl32 -limm32

	CXXFLAGS += `pkg-config --cflags glfw3`
endif

CXXFLAGS += -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends

.PHONY : all clean

all : ${EXEC}
	@echo Build complete for $(ECHO_MESSAGE)

${EXEC} : ${OBJECTS} ${IMGUI_OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@ -lcurl $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(IMGUI_DIR)/%.o: $(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(IMGUI_DIR)/backends/%.o: $(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean :
	rm -f $(OBJECTS) $(IMGUI_OBJECTS) ${EXEC}

