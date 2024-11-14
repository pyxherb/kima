file(GLOB SRC *.h *.c)

add_executable(example ${SRC})
target_link_libraries(example kima)
