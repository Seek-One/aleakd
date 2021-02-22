cmake_minimum_required (VERSION 3.0)

project (libaleakd VERSION 1.0.0 DESCRIPTION "Another leak detection library")

# Compile properties
SET (LIBALEAKD_SRC
	src/alloc-entry.c
	src/alloc-entry.h
	src/aleakd.c
	src/aleakd.h
)

add_library (libaleakd SHARED ${LIBALEAKD_SRC})

target_compile_definitions(libaleakd PRIVATE RUNTIME -D_GNU_SOURCE)

# Library version
set_target_properties(libaleakd PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(libaleakd PROPERTIES SOVERSION 1)

# Library headers
set_target_properties(libaleakd PROPERTIES PUBLIC_HEADER src/aleakd.h)

#set_target_properties(toolkit_dbg_mymalloc PROPERTIES CMAKE_EXE_LINKER_FLAGS "-Wl,-wrap,malloc -shared")

install (TARGETS libaleakd
		PUBLIC_HEADER DESTINATION include
        LIBRARY DESTINATION lib)
