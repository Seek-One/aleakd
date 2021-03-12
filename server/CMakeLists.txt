
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
	MemoryOperation.cpp
	MemoryOperation.h
	MemOpRcptServer.cpp
	MemOpRcptServer.h
	QMainEventFilter.cpp
	QMainEventFilter.h
	QMemoryOperationEvent.cpp
	QMemoryOperationEvent.h
	QMemoryOperationModel.cpp
	QMemoryOperationModel.h
	QApplicationWindow.cpp
	QApplicationWindow.h
	QApplicationWindowController.cpp
	QApplicationWindowController.h
	../shared/common.c
)

####################
# Create executable
####################

add_executable(aleakd-server ${ALEAKD_SERVER_SRC})
target_link_libraries(aleakd-server Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)

install (TARGETS aleakd-server DESTINATION bin)
