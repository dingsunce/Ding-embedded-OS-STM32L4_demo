file(GLOB_RECURSE driver_src *.c)

add_library(DRIVERS STATIC ${driver_src})