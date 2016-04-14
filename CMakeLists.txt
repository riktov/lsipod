cmake_minimum_required (VERSION 2.6)
project (lsipod)
add_executable(lsipod lsipod.c)
set_property(TARGET lsipod APPEND_STRING PROPERTY COMPILE_FLAGS -Wall )

add_executable(imobtest imobtest.c)
set_property(TARGET imobtest APPEND_STRING PROPERTY COMPILE_FLAGES -Wall )

find_package(PkgConfig REQUIRED)

pkg_search_module(glib-2.0 REQUIRED glib-2.0)
pkg_search_module(libgpod-1.0 REQUIRED libgpod-1.0)
pkg_search_module(libimobiledevice-1.0 REQUIRED libimobiledevice-1.0)
pkg_search_module(libplist REQUIRED libplist)
find_package(libxml2 REQUIRED)

#find_package(glib-2.0 REQUIRED glib-2.0)
include_directories(${libgpod-1.0_INCLUDE_DIRS})
include_directories(${libimobiledevice-1.0_INCLUDE_DIRS})
include_directories(${libplist_INCLUDE_DIRS})
include_directories(${libxml-2.0_INCLUDE_DIRS})

#set(LIBS ${LIBS} ${glib_LIBRARIES})

target_link_libraries(lsipod gpod glib-2.0)
target_link_libraries(imobtest gpod glib-2.0 imobiledevice plist xml2)

install(TARGETS lsipod imobtest DESTINATION bin)
