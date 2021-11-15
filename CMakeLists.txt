cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(qt_qr_code VERSION 0.0.1 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)

option(USE_QT4 "Perform a legacy build with Qt4" OFF)
message("Option USE_QT4 is ${USE_QT4}")

option(USE_C_LIBRARY "Create QR Code with C library")
message("Option USE_C_LIBRARY is ${USE_C_LIBRARY}")


add_library(qrcodegen-cpp STATIC
    ext/qrcodegen.cpp
    ext/qrcodegen.hpp
)

add_library(qrcodegen-c STATIC
    ext/qrcodegen.c
    ext/qrcodegen.h
)


if (USE_QT4)
    find_package(Qt4 COMPONENTS QtCore QtGui QtSvg REQUIRED)
    set(QT_LINK_LIBRARIES  Qt4::QtCore Qt4::QtGui Qt4::QtSvg)
else()
    find_package(Qt5 COMPONENTS Core Gui Svg Widgets REQUIRED)
    set(QT_LINK_LIBRARIES  Qt::Core Qt::Gui Qt::Svg Qt::Widgets)
endif()


add_executable(qt_qr_code
    README.md
    main.cpp
)
target_link_libraries(qt_qr_code ${QT_LINK_LIBRARIES})

if(USE_C_LIBRARY)
    target_compile_definitions(qt_qr_code PRIVATE USE_C_API)
    target_link_libraries(qt_qr_code qrcodegen-c)
else()
    target_link_libraries(qt_qr_code qrcodegen-cpp)
endif()
