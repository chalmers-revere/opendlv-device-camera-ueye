# Copyright (C) 2018  Christian Berger
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.2)

project(opendlv-device-camera-ueye)

################################################################################
# Defining the relevant version of libcluon.
set(CLUON_COMPLETE cluon-complete-v0.0.117.hpp)

################################################################################
# Set the search path for .cmake files.
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH})

################################################################################
# This project requires C++14 or newer.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Build a static binary.
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
# Add further warning levels.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -D_XOPEN_SOURCE=700 \
    -D_FORTIFY_SOURCE=2 \
    -O2 \
    -fstack-protector \
    -fomit-frame-pointer \
    -pipe \
    -Weffc++ \
    -Wall -Wextra -Wshadow -Wdeprecated \
    -Wdiv-by-zero -Wfloat-equal -Wfloat-conversion -Wsign-compare -Wpointer-arith \
    -Wuninitialized -Wunreachable-code \
    -Wunused -Wunused-function -Wunused-label -Wunused-parameter -Wunused-but-set-parameter -Wunused-but-set-variable \
    -Wunused-value -Wunused-variable -Wunused-result \
    -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn")

################################################################################
# Create symbolic link to cluon-complete.hpp.
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/cluon-complete.hpp
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/src/${CLUON_COMPLETE} ${CMAKE_BINARY_DIR}/cluon-complete.hpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/${CLUON_COMPLETE})

# Add current build directory as include directory as it contains generated files.
include_directories(SYSTEM ${CMAKE_BINARY_DIR})

################################################################################
# Threads are necessary for linking the resulting binaries as UDPReceiver is running in parallel.
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
set(LIBRARIES Threads::Threads)

if(UNIX)
    if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        find_package(LibRT REQUIRED)
        set(LIBRARIES ${LIBRARIES} ${LIBRT_LIBRARIES})
        include_directories(SYSTEM ${LIBRT_INCLUDE_DIR})
    endif()
endif()

find_package(Libyuv REQUIRED)
include_directories(SYSTEM ${YUV_INCLUDE_DIRS})
set(LIBRARIES ${LIBRARIES} ${YUV_LIBRARIES})

find_package(LibuEye REQUIRED)
include_directories(SYSTEM ${LIBUEYE_INCLUDE_DIRS})
set(LIBRARIES ${LIBRARIES} ${LIBUEYE_LIBRARIES})

find_package(X11 REQUIRED)
include_directories(SYSTEM ${X11_INCLUDE_DIR})
set(LIBRARIES ${LIBRARIES} ${X11_X11_LIB})

################################################################################
# Create executable.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}.cpp ${CMAKE_BINARY_DIR}/cluon-complete.hpp)
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

################################################################################
# Install executable.
install(TARGETS ${PROJECT_NAME} DESTINATION bin COMPONENT ${PROJECT_NAME})
