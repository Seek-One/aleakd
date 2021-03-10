
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
	MemOpRcptServer.cpp
	MemOpRcptServer.h
)

####################
# Create executable
####################

add_executable(aleakd-server ${ALEAKD_SERVER_SRC})
target_link_libraries(aleakd-server Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)

install (TARGETS aleakd-server DESTINATION bin)
