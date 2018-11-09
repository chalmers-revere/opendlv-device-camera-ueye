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

###########################################################################
# Find libyuv.
FIND_PATH(YUV_INCLUDE_DIR
          NAMES libyuv.h
          PATHS /usr/local/include/
                /usr/include/)
MARK_AS_ADVANCED(YUV_INCLUDE_DIR)
FIND_LIBRARY(YUV_LIBRARY
             NAMES yuv
             PATHS ${LIBYUVDIR}/lib/
                    /usr/lib/arm-linux-gnueabihf/
                    /usr/lib/arm-linux-gnueabi/
                    /usr/lib/x86_64-linux-gnu/
                    /usr/local/lib64/
                    /usr/lib64/
                    /usr/lib/)
MARK_AS_ADVANCED(YUV_LIBRARY)

###########################################################################
IF (YUV_INCLUDE_DIR
    AND YUV_LIBRARY)
    SET(YUV_FOUND 1)
    SET(YUV_LIBRARIES ${YUV_LIBRARY})
    SET(YUV_INCLUDE_DIRS ${YUV_INCLUDE_DIR})
ENDIF()

MARK_AS_ADVANCED(YUV_LIBRARIES)
MARK_AS_ADVANCED(YUV_INCLUDE_DIRS)

IF (YUV_FOUND)
    MESSAGE(STATUS "Found libyuv: ${YUV_INCLUDE_DIRS}, ${YUV_LIBRARIES}")
ELSE ()
    MESSAGE(STATUS "Could not find libyuv")
ENDIF()
