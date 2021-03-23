#################
# Sources
#################

SET (ALEAKD_LIB_SRC
	aleakd.c
	aleakd.h
	wrapper.c
	wrapper.h
	server-comm.c
	server-comm.h
	../shared/common.c
)

#################
# Dynamic library
#################

add_library (aleakd SHARED ${ALEAKD_LIB_SRC})

# Library version
set_target_properties(aleakd PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(aleakd PROPERTIES SOVERSION 1)

# Library headers
set_target_properties(aleakd PROPERTIES PUBLIC_HEADER aleakd.h)

#set_target_properties(aleakd PROPERTIES CMAKE_EXE_LINKER_FLAGS "-fPIC -O0 -Wl,--wrap,malloc -Wl,--wrap,free")

set_target_properties(aleakd PROPERTIES LINK_LIBRARIES "-lpthread -ldl")

install (TARGETS aleakd
		PUBLIC_HEADER DESTINATION include/libaleakd
        LIBRARY DESTINATION lib)
