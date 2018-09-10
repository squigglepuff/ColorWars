#-------------------------------------------------
#
# Project created by QtCreator 2018-08-27T09:57:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Enable C++14
CONFIG(!c++14): CONFIG += c++14

TARGET = ColorWars
TEMPLATE = app

# Versioning (use UNIX-style versioning)
VERSION = 1.0.0
BUILD_VER = 0001

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        src/honeycomb.cpp \
        src/main.cpp \
        src/mainwindow.cpp \
    src/board.cpp

HEADERS += \
        include/honeycomb.h \
        include/mainwindow.h \
    include/globals.h \
    include/board.h

# Specify Build settings.

CONFIG(release, debug|release) {
    message(Building release target)
    DESTDIR = $$_PRO_FILE_PWD_/build/release/bin
    OBJECTS_DIR = $$_PRO_FILE_PWD_/build/release/.obj
    MOC_DIR = $$_PRO_FILE_PWD_/build/release/.moc
    RCC_DIR = $$_PRO_FILE_PWD_/build/release/.rcc
    UI_DIR = $$_PRO_FILE_PWD_/build/release/.ui
}

CONFIG(debug, debug|release) {
    message(Building debug target)
    DESTDIR = $$_PRO_FILE_PWD_/build/debug/bin
    OBJECTS_DIR = $$_PRO_FILE_PWD_/build/debug/.obj
    MOC_DIR = $$_PRO_FILE_PWD_/build/debug/.moc
    RCC_DIR = $$_PRO_FILE_PWD_/build/debug/.rcc
    UI_DIR = $$_PRO_FILE_PWD_/build/debug/.ui
}


# System Information.
message($$TARGET ($$VERSION Build: $$BUILD_VER))
message(Build OS: $$QMAKE_HOST.os $$QMAKE_HOST.version_string ($$QMAKE_HOST.version))
message(Build Arch: $$QMAKE_HOST.arch)
message(Build CPUs: $$QMAKE_HOST.cpu_count)
message(Build Hostname: $$QMAKE_HOST.name)
message(Qt Version: $$QT_VERSION)
message(QMake path: $$QMAKE_QMAKE)
message(C compiler: $$QMAKE_CC)
message(C++ compiler: $$QMAKE_CXX)
message(Current output directory: $$OUT_PWD)
message(Build Directory: $$DESTDIR)
