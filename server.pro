# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Tools.
# ------------------------------------------------------

TEMPLATE = app
TARGET = server
QT += core gui network widgets
VERSION = 5.15

# allows use of version variable elsewhere
DEFINES += "VERSION=$$VERSION"

# build folder organization
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR = build

CONFIG -= debug_and_release debug_and_release_target

# enable c++17
CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Ban usage of Qt's built in foreach utility for better code style
DEFINES += QT_NO_FOREACH

include(src/server.pri)
