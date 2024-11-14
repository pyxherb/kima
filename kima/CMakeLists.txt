file(GLOB SRC *.h *.c)

add_library(kima ${SRC})

if(WIN32)
    add_subdirectory("win32")
endif()
