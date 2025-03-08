# C++ library for ST Teseo GPS

C++ driver for the ST Teseo-LIV3 GPS module.

[![Open Source](https://badges.frapsoft.com/os/v1/open-source.svg?v=103)](https://opensource.org/)
![GitHub License](https://img.shields.io/github/license/jancumps/pico_gps_teseo?label=licence)
[![run unit tests](https://github.com/jancumps/gps_teseo_lib/actions/workflows/test.yml/badge.svg)](https://github.com/jancumps/gps_teseo_lib/actions/workflows/test.yml)


goals:
- Teseo lib code does not need to know what the target microcontroller is.
- Teseo lib code does not need to know if the project uses I2C or UART
- controller and protocol functionality is provided by the user's project code. It has to plug in a reader and writer function.
- lean, for embedded evelopment

1: [Pico and I2C support](https://community.element14.com/technologies/embedded/b/blog/posts/c-library-for-st-teseo-gps---pt-1-pico-and-i2c-support?CommentId=a0dfd5e9-20a5-4ae6-8b1d-723620f2db3f)  
2: [Dynamic GPS configuration (and some other things) ](https://community.element14.com/technologies/embedded/b/blog/posts/c-library-for-st-teseo-gps---pt-2-dynamic-gps-configuration-and-some-other-things)  


Requires a toolchain that supports C++ modules. Tested with CMake 3.26 and GCC 14.2.1  

depends on module [callbackmanager ](https://github.com/jancumps/callbackmanager)  

cmake example:

```
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules-ts")

add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/teseo/teseo.cpp
)
target_sources(${CMAKE_PROJECT_NAME}
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/callbackmanager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/teseo/teseo_iface.cpp
)
```