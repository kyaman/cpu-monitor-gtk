cmake_minimum_required (VERSION 2.6)

project (cpu_monitor_gtk)
add_definitions(-Wall)

find_package(PkgConfig)

pkg_check_modules(GTK2 REQUIRED gtk+-2.0)
pkg_check_modules(LIBGTOP2 REQUIRED libgtop-2.0)

include_directories(
  ${GTK2_INCLUDE_DIRS}
  ${LIBGTOP2_INCLUDE_DIRS}
)

link_directories(
  ${GTK2_LIBRARY_DIRS}
  ${LIBGTOP2_LIBRARY_DIRS}
)

add_executable (cpu_monitor_gtk main.c fifo.c)

target_link_libraries(cpu_monitor_gtk
  ${GTK2_LIBRARIES}
  ${LIBGTOP2_LIBRARIES}
)
