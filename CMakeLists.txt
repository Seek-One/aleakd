cmake_minimum_required (VERSION 3.0)

project (libaleakd VERSION 1.0.0 DESCRIPTION "Another leak detection library")

# Compile properties
SET (LIBALEAKD_SRC
	src/alloc-entry.c
	src/alloc-entry.h
	src/aleakd.c
	src/aleakd.h
)

add_library (aleakd SHARED ${LIBALEAKD_SRC})

target_compile_definitions(aleakd PRIVATE RUNTIME -D_GNU_SOURCE)

# Library version
set_target_properties(aleakd PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(aleakd PROPERTIES SOVERSION 1)

# Library headers
set_target_properties(aleakd PROPERTIES PUBLIC_HEADER src/aleakd.h)

#set_target_properties(toolkit_dbg_mymalloc PROPERTIES CMAKE_EXE_LINKER_FLAGS "-Wl,-wrap,malloc -shared")

install (TARGETS aleakd
		PUBLIC_HEADER DESTINATION include/libaleakd
        LIBRARY DESTINATION lib)
