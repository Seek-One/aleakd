
#################
# Dynamic library
#################

set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)

#################
# Sources
#################

SET (ALEAKD_SERVER_SRC
	main.cpp
	QMainEventFilter.cpp
	QMainEventFilter.h
	GUI/QApplicationWindow.cpp
	GUI/QApplicationWindow.h
	GUIController/QApplicationWindowController.cpp
	GUIController/QApplicationWindowController.h
	GUIModel/QMemoryOperationModel.cpp
	GUIModel/QMemoryOperationModel.h
	Model/MemoryOperation.cpp
	Model/MemoryOperation.h
	Model/MemoryStats.cpp
	Model/MemoryStats.h
	Server/IMemOpRcptServerHandler.cpp
	Server/IMemOpRcptServerHandler.h
	Server/MemOpRcptServer.cpp
	Server/MemOpRcptServer.h
	../shared/common.c
)

include_directories(".")

####################
# Create executable
####################

add_executable(aleakd-server ${ALEAKD_SERVER_SRC})
target_link_libraries(aleakd-server Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)

install (TARGETS aleakd-server DESTINATION bin)
