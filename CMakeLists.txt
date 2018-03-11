cmake_minimum_required(VERSION 3.5)

project(libvldmail C)

# Set the includes for the heart of the sun:
include_directories(src)

# Build the library:
add_library(vldmail STATIC src/vldmail.c)

if (BUILD_THE_TEST)
  # Build the test program as well:
  add_executable(vldmailtest test.c)

  # Link the test program:
  target_link_libraries(vldmailtest vldmail)
endif ()
