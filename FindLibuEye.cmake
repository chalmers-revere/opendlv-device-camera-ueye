# FindUEye.cmake - CMake module to find the uEye drivers.
# Copyright (C) 2015  Christian Berger
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

IF(NOT LIBUEYE_FOUND)
    FIND_PATH(LIBUEYE_INCLUDE_DIR
        NAMES ueye.h
        PATHS ${LIBUEYE_PATH}/include/
              /usr/local/include/
              /usr/include/
    )

    FIND_FILE(LIBUEYE_LIBRARIES libueye_api.so
        PATHS ${LIBUEYE_PATH}/lib/
              /usr/local/lib64/
              /usr/local/lib/
              /usr/lib/i386-linux-gnu/
              /usr/lib/x86_64-linux-gnu/
              /usr/lib64/
              /usr/lib/
    )

    IF(LIBUEYE_INCLUDE_DIR AND LIBUEYE_LIBRARIES)
        SET (LIBUEYE_FOUND TRUE)
    ENDIF()

    IF(LIBUEYE_FOUND)
        MESSAGE(STATUS "Found uEye: ${LIBUEYE_INCLUDE_DIR}, ${LIBUEYE_LIBRARIES}")
    ELSE()
        IF(LIBUEYE_FIND_REQUIRED)
            MESSAGE(FATAL_ERROR "Could not find uEye, try to setup LIBUEYE_PATH accordingly.")
        ELSE()
            MESSAGE(STATUS "uEye not found.")
        ENDIF()
    ENDIF()
ENDIF()

