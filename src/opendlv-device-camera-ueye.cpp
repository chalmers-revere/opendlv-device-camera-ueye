/*
 * Copyright (C) 2018  Christian Berger
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cluon-complete.hpp"

#include <ueye.h>
#include <X11/Xlib.h>
#include <libyuv.h>

#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>

int32_t main(int32_t argc, char **argv) {
    int32_t retCode{0};
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if ( (0 == commandlineArguments.count("width")) ||
         (0 == commandlineArguments.count("height")) ||
         (0 == commandlineArguments.count("freq")) ) {
        std::cerr << argv[0] << " interfaces with the given IDS uEye camera (e.g., UI122xLE-M) and provides the captured image in two shared memory areas: one in I420 format and one in ARGB format." << std::endl;
        std::cerr << "Usage:   " << argv[0] << " --width=<width> --height=<height> [--pixel_clock=<value>] [--name.i420=<unique name for the shared memory in I420 format>] [--name.argb=<unique name for the shared memory in ARGB format>] [--verbose]" << std::endl;
        std::cerr << "         --name.i420:   name of the shared memory for the I420 formatted image; when omitted, 'ueye.i420' is chosen" << std::endl;
        std::cerr << "         --name.argb:   name of the shared memory for the I420 formatted image; when omitted, 'ueye.argb' is chosen" << std::endl;
        std::cerr << "         --pixel_clock: desired pixel clock (default: 10)" << std::endl;
        std::cerr << "         --width:       desired width of a frame" << std::endl;
        std::cerr << "         --height:      desired height of a frame" << std::endl;
        std::cerr << "         --freq:        desired frequency" << std::endl;
        std::cerr << "         --verbose:     display captured image" << std::endl;
        std::cerr << "Example: " << argv[0] << " --width=752 --height=480 --pixel_clock=10 --freq=20 --verbose" << std::endl;
        retCode = 1;
    }
    else {
        const uint32_t WIDTH{static_cast<uint32_t>(std::stoi(commandlineArguments["width"]))};
        const uint32_t HEIGHT{static_cast<uint32_t>(std::stoi(commandlineArguments["height"]))};
        const bool VERBOSE{commandlineArguments.count("verbose") != 0};
        const uint32_t PIXEL_CLOCK{(commandlineArguments["pixel_clock"].size() != 0) ?static_cast<uint32_t>(std::stoi(commandlineArguments["pixel_clock"])) : 10};

        const float FREQ{static_cast<float>(std::stof(commandlineArguments["freq"]))};
        if ( !(FREQ > 0) ) {
            std::cerr << "[opendlv-device-camera-ueye]: freq must be larger than 0; found " << FREQ << "." << std::endl;
            return retCode = 1;
        }

        // Set up the names for the shared memory areas.
        std::string NAME_I420{"ueye.i420"};
        if ((commandlineArguments["name.i420"].size() != 0)) {
            NAME_I420 = commandlineArguments["name.i420"];
        }
        std::string NAME_ARGB{"ueye.argb"};
        if ((commandlineArguments["name.argb"].size() != 0)) {
            NAME_ARGB = commandlineArguments["name.argb"];
        }

        // Initialize camera.
        HIDS capture{0};
        retCode = is_InitCamera(&capture, nullptr);
        if (IS_SUCCESS == retCode) {
            // Configure camera.
            char *imageMemory{nullptr};
            int imageMemoryID{0};
            {
                SENSORINFO cameraInfo;
                std::memset(&cameraInfo, 0, sizeof(SENSORINFO));
                is_GetSensorInfo(capture, &cameraInfo);
                std::clog << "[opendlv-device-camera-ueye]: Found IDS uEye device: '" << cameraInfo.strSensorName << "', max width = " << cameraInfo.nMaxWidth << ", max height = " << cameraInfo.nMaxHeight << std::endl;

                retCode = is_PixelClock(capture, IS_PIXELCLOCK_CMD_SET, (void*)&PIXEL_CLOCK, sizeof(PIXEL_CLOCK));
                std::clog << "[opendlv-device-camera-ueye]: Setting pixel clock: " << ((IS_SUCCESS == retCode) ? "succeeded" : "failed") << std::endl;

                retCode = is_SetColorMode(capture, IS_CM_UYVY_MONO_PACKED);
                std::clog << "[opendlv-device-camera-ueye]: Setting color mode UYVY: " << ((IS_SUCCESS == retCode) ? "succeeded" : "failed") << std::endl;

                retCode = is_AllocImageMem(capture, WIDTH, HEIGHT, 8*3/2 /*bytes per pixel for UYUV422*/, &imageMemory, &imageMemoryID);
                std::clog << "[opendlv-device-camera-ueye]: Allocating image memory: " << ((IS_SUCCESS == retCode) ? "succeeded" : "failed") << std::endl;

                retCode = is_SetImageMem(capture, imageMemory, imageMemoryID);
                std::clog << "[opendlv-device-camera-ueye]: Setting image memory: " << ((IS_SUCCESS == retCode) ? "succeeded" : "failed") << std::endl;

                retCode = is_SetDisplayMode(capture, IS_SET_DM_DIB);
                std::clog << "[opendlv-device-camera-ueye]: Setting display mode: " << ((IS_SUCCESS == retCode) ? "succeeded" : "failed") << std::endl;

                double newFPS{0};
                retCode = is_SetFrameRate(capture, FREQ, &newFPS);
                std::clog << "[opendlv-device-camera-ueye]: Setting frame rate: " << ((IS_SUCCESS == retCode) ? "succeeded" : "failed") << ", FPS = " << newFPS << std::endl;

                double disable{0};
                double enable{1};
                is_SetAutoParameter(capture, IS_SET_ENABLE_AUTO_FRAMERATE, &disable, 0);
                is_SetAutoParameter(capture, IS_SET_ENABLE_AUTO_GAIN, &enable, 0);
                is_SetAutoParameter(capture, IS_SET_ENABLE_AUTO_SENSOR_GAIN, &enable, 0);
                is_SetAutoParameter(capture, IS_SET_ENABLE_AUTO_SENSOR_SHUTTER, &disable, 0);
                is_SetAutoParameter(capture, IS_SET_ENABLE_AUTO_SENSOR_WHITEBALANCE,&enable,0);
                is_SetAutoParameter(capture, IS_SET_ENABLE_AUTO_SHUTTER, &disable, 0);
                is_SetAutoParameter(capture, IS_SET_ENABLE_AUTO_WHITEBALANCE, &enable, 0);
            }

            // Initialize shared memory.
            std::unique_ptr<cluon::SharedMemory> sharedMemoryI420(new cluon::SharedMemory{NAME_I420, WIDTH * HEIGHT * 3/2});
            if (!sharedMemoryI420 || !sharedMemoryI420->valid()) {
                std::cerr << "[opendlv-device-camera-ueye]: Failed to create shared memory '" << NAME_I420 << "'." << std::endl;
                return retCode = 1;
            }

            std::unique_ptr<cluon::SharedMemory> sharedMemoryARGB(new cluon::SharedMemory{NAME_ARGB, WIDTH * HEIGHT * 4});
            if (!sharedMemoryARGB || !sharedMemoryARGB->valid()) {
                std::cerr << "[opendlv-device-camera-ueye]: Failed to create shared memory '" << NAME_ARGB << "'." << std::endl;
                return retCode = 1;
            }

            if ( (sharedMemoryI420 && sharedMemoryI420->valid()) &&
                 (sharedMemoryARGB && sharedMemoryARGB->valid()) ) {
                std::clog << "[opendlv-device-camera-ueye]: Data from uEye camera available in I420 format in shared memory '" << sharedMemoryI420->name() << "' (" << sharedMemoryI420->size() << ") and in ARGB format in shared memory '" << sharedMemoryARGB->name() << "' (" << sharedMemoryARGB->size() << ")." << std::endl;

                // Accessing the low-level X11 data display.
                Display* display{nullptr};
                Visual* visual{nullptr};
                Window window{0};
                XImage* ximage{nullptr};
                if (VERBOSE) {
                    display = XOpenDisplay(NULL);
                    visual = DefaultVisual(display, 0);
                    window = XCreateSimpleWindow(display, RootWindow(display, 0), 0, 0, WIDTH, HEIGHT, 1, 0, 0);
                    sharedMemoryARGB->lock();
                    {
                        ximage = XCreateImage(display, visual, 24, ZPixmap, 0, sharedMemoryARGB->data(), WIDTH, HEIGHT, 32, 0);
                    }
                    sharedMemoryARGB->unlock();
                    XMapWindow(display, window);
                }

                while (!cluon::TerminateHandler::instance().isTerminated.load()) {
                    if (IS_SUCCESS == is_FreezeVideo(capture, IS_WAIT)) {
                        uint8_t *ueyeImagePtr{nullptr};
                        if (IS_SUCCESS == is_GetImageMem(capture, (void**)&(ueyeImagePtr))) {
                            cluon::data::TimeStamp ts{cluon::time::now()};
                            // Transform data as I420 in sharedMemoryI420.
                            sharedMemoryI420->lock();
                            sharedMemoryI420->setTimeStamp(ts);
                            {
                                libyuv::UYVYToI420(ueyeImagePtr, WIDTH * 2 /* 2*WIDTH for YUYV 422*/,
                                                   reinterpret_cast<uint8_t*>(sharedMemoryI420->data()), WIDTH,
                                                   reinterpret_cast<uint8_t*>(sharedMemoryI420->data()+(WIDTH * HEIGHT)), WIDTH/2,
                                                   reinterpret_cast<uint8_t*>(sharedMemoryI420->data()+(WIDTH * HEIGHT + ((WIDTH * HEIGHT) >> 2))), WIDTH/2,
                                                   WIDTH, HEIGHT);
                            }
                            sharedMemoryI420->unlock();

                            sharedMemoryARGB->lock();
                            sharedMemoryARGB->setTimeStamp(ts);
                            {
                                libyuv::I420ToARGB(reinterpret_cast<uint8_t*>(sharedMemoryI420->data()), WIDTH,
                                                   reinterpret_cast<uint8_t*>(sharedMemoryI420->data()+(WIDTH * HEIGHT)), WIDTH/2,
                                                   reinterpret_cast<uint8_t*>(sharedMemoryI420->data()+(WIDTH * HEIGHT + ((WIDTH * HEIGHT) >> 2))), WIDTH/2,
                                                   reinterpret_cast<uint8_t*>(sharedMemoryARGB->data()), WIDTH * 4, WIDTH, HEIGHT);

                                if (VERBOSE) {
                                    XPutImage(display, window, DefaultGC(display, 0), ximage, 0, 0, 0, 0, WIDTH, HEIGHT);
                                }
                            }
                            sharedMemoryARGB->unlock();

                            sharedMemoryI420->notifyAll();
                            sharedMemoryARGB->notifyAll();
                        }
                    }
                }

                if (VERBOSE) {
                    XCloseDisplay(display);
                }
            }

            is_FreeImageMem(capture, imageMemory, imageMemoryID);
            is_ExitCamera(capture);
        }
    }
    return retCode;
}

