## OpenDLV Microservice to interface with IDS uEye cameras

This repository provides source code to interface with IDS uEye cameras
for the OpenDLV software ecosystem. This microservice provides the captured frames
in two separate shared memory areas, one for a picture in [I420 format](https://wiki.videolan.org/YUV/#I420)
and one in ARGB format.

[![Build Status](https://travis-ci.org/chalmers-revere/opendlv-device-camera-ueye.svg?branch=master)](https://travis-ci.org/chalmers-revere/opendlv-device-camera-ueye) [![License: GPLv3](https://img.shields.io/badge/license-GPL--3-blue.svg
)](https://www.gnu.org/licenses/gpl-3.0.txt)


## Table of Contents
* [Dependencies](#dependencies)
* [Usage](#usage)
* [Build from sources on the example of Ubuntu 16.04 LTS](#build-from-sources-on-the-example-of-ubuntu-1604-lts)
* [License](#license)


## Dependencies
You need a C++14-compliant compiler to compile this project. The following
dependency is shipped as part of the source distribution:

* [libcluon](https://github.com/chrberger/libcluon) - [![License: GPLv3](https://img.shields.io/badge/license-GPL--3-blue.svg
)](https://www.gnu.org/licenses/gpl-3.0.txt)
* [libyuv](https://chromium.googlesource.com/libyuv/libyuv/+/master) - [![License: BSD 3-Clause](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause) - [Google Patent License Conditions](https://chromium.googlesource.com/libyuv/libyuv/+/master/PATENTS)

To use this microservice, you need to install the drivers on the host machine from [here](https://www.1stvision.com/cameras/IDS-imaging-software).


## Usage
This microservice is created automatically on changes to this repository via Docker's public registry for:
* [x86_64](https://hub.docker.com/r/chalmersrevere/opendlv-device-camera-ueye-amd64/tags/)
* [armhf](https://hub.docker.com/r/chalmersrevere/opendlv-device-camera-ueye-armhf/tags/)
* [aarch64](https://hub.docker.com/r/chalmersrevere/opendlv-device-camera-ueye-aarch64/tags/)

To run this microservice using our pre-built Docker multi-arch images to open
a supported camera, simply start it as follows:

```
docker run --rm -ti --init --ipc=host -v /tmp:/tmp --pid=host -v /var/run:/var/run -e DISPLAY=$DISPLAY chalmersrevere/opendlv-device-camera-ueye-multi:v0.0.5 --width=752 --height=480 --pixel_clock=10 --freq=20 --verbose
```

## Build from sources on the example of Ubuntu 16.04 LTS
To build this software, you need cmake, C++14 or newer, libx11-dev, and make.
Having these preconditions, just run `cmake` and `make` as follows:

```
mkdir build && cd build
cmake -D CMAKE_BUILD_TYPE=Release ..
make && make test && make install
```


## License

* This project is released under the terms of the GNU GPLv3 License

