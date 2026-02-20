cmake_minimum_required(VERSION "3.12")

if("${CMAKE_CURRENT_BINARY_DIR}/" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}/")
    message(FATAL_ERROR "In-source builds are not allowed.")
endif()

# General
project(
    "libvldmail"
    DESCRIPTION "Your friendly e-mail address validation library."
    HOMEPAGE_URL "https://code.rosaelefanten.org/libvldmail/dir"
    LANGUAGES "C"
    VERSION "1.2.0")
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Modules
include("CMakePackageConfigHelpers")
include("GNUInstallDirs")

# Options
option(LIBVLDMAIL_TESTS "Build the tests." OFF)

# Target
if(NOT ${LIBVLDMAIL_TESTS})
    set(TARGET_NAME "vldmail")
else()
    set(TARGET_NAME "vldmailtest")
endif()

set(SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/vldmail.c")

if(${LIBVLDMAIL_TESTS})
    list(APPEND SRCS "${CMAKE_CURRENT_SOURCE_DIR}/test.c")
endif()

if(NOT ${LIBVLDMAIL_TESTS})
    add_library("${TARGET_NAME}" "${SRCS}")

    set_target_properties("${TARGET_NAME}" PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/vldmail.h")
else()
    enable_testing()
    include("CTest")
    add_executable("${TARGET_NAME}" "${SRCS}")
    add_test(
        NAME "${TARGET_NAME}"
        COMMAND "${TARGET_NAME}"
        CONFIGURATIONS "Debug"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")
endif()

# Headers
target_include_directories(${TARGET_NAME} AFTER PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>
                                                       $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/>)

# Installation
if(NOT ${LIBVLDMAIL_TESTS})
    install(
        TARGETS "${TARGET_NAME}"
        EXPORT "${PROJECT_NAME}-export"
        ARCHIVE
            CONFIGURATIONS "Release"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/"
            PERMISSIONS
                OWNER_READ
                OWNER_WRITE
                OWNER_EXECUTE
                GROUP_READ
                GROUP_EXECUTE
                WORLD_READ
                WORLD_EXECUTE
        BUNDLE
            CONFIGURATIONS "Release"
            DESTINATION "${CMAKE_INSTALL_BINDIR}/"
            PERMISSIONS
                OWNER_READ
                OWNER_WRITE
                OWNER_EXECUTE
                GROUP_READ
                GROUP_EXECUTE
                WORLD_READ
                WORLD_EXECUTE
        FRAMEWORK
            CONFIGURATIONS "Release"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/"
            PERMISSIONS
                OWNER_READ
                OWNER_WRITE
                OWNER_EXECUTE
                GROUP_READ
                GROUP_EXECUTE
                WORLD_READ
                WORLD_EXECUTE
        LIBRARY
            CONFIGURATIONS "Release"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/"
            PERMISSIONS
                OWNER_READ
                OWNER_WRITE
                OWNER_EXECUTE
                GROUP_READ
                GROUP_EXECUTE
                WORLD_READ
                WORLD_EXECUTE
        OBJECTS
            CONFIGURATIONS "Release"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/"
            PERMISSIONS
                OWNER_READ
                OWNER_WRITE
                OWNER_EXECUTE
                GROUP_READ
                GROUP_EXECUTE
                WORLD_READ
                WORLD_EXECUTE
        PRIVATE_HEADER
            CONFIGURATIONS "Release"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/"
            PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
        PUBLIC_HEADER
            CONFIGURATIONS "Release"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/"
            PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
        RESOURCE CONFIGURATIONS "Release"
                 DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/"
                 PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
        RUNTIME
            CONFIGURATIONS "Release"
            DESTINATION "${CMAKE_INSTALL_BINDIR}/"
            PERMISSIONS
                OWNER_READ
                OWNER_WRITE
                OWNER_EXECUTE
                GROUP_READ
                GROUP_EXECUTE
                WORLD_READ
                WORLD_EXECUTE
        INCLUDES
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/")

    install(
        EXPORT "${PROJECT_NAME}-export"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}/"
        FILE "${PROJECT_NAME}-targets.cmake"
        NAMESPACE "${PROJECT_NAME}::")

    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}/")

    write_basic_package_version_file(
        "${PROJECT_NAME}-config-version.cmake"
        COMPATIBILITY SameMajorVersion
        VERSION "${PROJECT_VERSION}")

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
                  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}/")
endif()
