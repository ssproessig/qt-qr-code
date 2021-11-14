cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(qt_qr_code VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)


add_library(qrcodegen STATIC
    ext/qrcodegen.cpp
    ext/qrcodegen.hpp
)
