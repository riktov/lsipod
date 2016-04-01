cmake_minimum_required (VERSION 2.6)
project (lsipod)
add_executable(lsipod lsipod.c)
set_property(TARGET lsipod APPEND_STRING PROPERTY COMPILE_FLAGS -Wall )

find_package(PkgConfig REQUIRED)

pkg_search_module(glib-2.0 REQUIRED glib-2.0)
pkg_search_module(libgpod-1.0 REQUIRED libgpod-1.0)

#find_package(glib-2.0 REQUIRED glib-2.0)
include_directories(${libgpod-1.0_INCLUDE_DIRS})
#set(LIBS ${LIBS} ${glib_LIBRARIES})

target_link_libraries(lsipod gpod glib-2.0)

install(TARGETS lsipod DESTINATION bin)
