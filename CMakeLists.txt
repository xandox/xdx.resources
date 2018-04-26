cmake_minimum_required(VERSION 3.10)

include($ENV{XDX_CMAKE_ROOT}/xdx.cmake)

include(deps.cmake)

xdx_umbrella(
    NAME xdx.resources
    SUBDIRS
        library
        compiler
        test
)
