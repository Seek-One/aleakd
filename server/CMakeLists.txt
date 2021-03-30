
#################
# Dynamic library
#################

set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Network Charts REQUIRED)

#################
# Sources
#################

SET (ALEAKD_SERVER_SRC
	main.cpp
	QMainEventFilter.cpp
	QMainEventFilter.h
	Global/Utils.cpp
	Global/Utils.h
	GUI/QApplicationWindow.cpp
	GUI/QApplicationWindow.h
	GUI/QMemoryOperationView.cpp
	GUI/QMemoryOperationView.h
	GUI/QMemoryOperationListView.cpp
	GUI/QMemoryOperationListView.h
	GUI/QMemoryStatsView.cpp
	GUI/QMemoryStatsView.h
	GUI/QMemoryUsageView.cpp
	GUI/QMemoryUsageView.h
	GUI/QThreadInfosView.cpp
	GUI/QThreadInfosView.h
	GUIController/QApplicationWindowController.cpp
	GUIController/QApplicationWindowController.h
	GUIModel/QMemoryOperationModel.cpp
	GUIModel/QMemoryOperationModel.h
	GUIModel/QThreadInfosModel.cpp
	GUIModel/QThreadInfosModel.h
	Model/Backtrace.cpp
	Model/Backtrace.h
	Model/MemoryOperation.cpp
	Model/MemoryOperation.h
	Model/MemoryStats.cpp
	Model/MemoryStats.h
	Model/SymbolInfos.cpp
	Model/SymbolInfos.h
	Model/ThreadInfos.cpp
	Model/ThreadInfos.h
	Model/ThreadOperation.cpp
	Model/ThreadOperation.h
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
target_link_libraries(aleakd-server Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::Charts)

install (TARGETS aleakd-server DESTINATION bin)
