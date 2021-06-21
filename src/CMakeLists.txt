add_library(sfloat sfloat.c sfloat.h)
add_executable(float main.c)
target_link_libraries(float sfloat)
