cmake_minimum_required (VERSION 3.13)

project (aleakd VERSION 1.0.0 DESCRIPTION "Another leak detection library")

# Sources
SET (LIBALEAKD_SRC
	src/alloc-entry.c
	src/alloc-entry.h
	src/aleakd.c
	src/aleakd.h
	src/aleakd-data.c
	src/aleakd-data.h
	src/thread-entry.c
	src/thread-entry.h
	src/wrapper.c
	src/wrapper.h
)

#################
# Dynamic library
#################

add_subdirectory("lib")

#################
# Server
#################

add_subdirectory("server")

#################
# Static library
#################

#add_library (aleakd-static STATIC ${LIBALEAKD_SRC})

#target_compile_definitions(aleakd PRIVATE RUNTIME -D_GNU_SOURCE)

# Library version
#set_target_properties(aleakd-static PROPERTIES VERSION ${PROJECT_VERSION})
#set_target_properties(aleakd-static PROPERTIES SOVERSION 1)

#set_target_properties(toolkit_dbg_mymalloc PROPERTIES CMAKE_EXE_LINKER_FLAGS "-Wl,-wrap,malloc -shared")

#install (TARGETS aleakd-static
 #       LIBRARY DESTINATION lib)

#################
# Test program
#################


# Sources
SET (ALEAKDTEST_SRC
	test/aleakd-test.c
)

add_executable(aleakd-test ${ALEAKDTEST_SRC})
#set_target_properties(aleakd-test PROPERTIES LINK_LIBRARIES "-lpthread -ldl")

target_link_libraries(aleakd-test aleakd)

#set_target_properties(aleakd-test PROPERTIES CMAKE_EXE_LINKER_FLAGS "-lpthread  -ldl -fPIC -O0")
