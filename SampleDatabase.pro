#-------------------------------------------------
#
# Project created by QtCreator 2017-01-10T16:28:07
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SampleDatabase
TEMPLATE = app

RC_ICONS += icon.ico

SOURCES += main.cpp\
        mainwindow.cpp \
    tableeditor.cpp \
    dateformatdelegate.cpp \
    openfiledelegate.cpp \
    constant.cpp

HEADERS  += mainwindow.h \
    constant.h \
    tableeditor.h \
    dateformatdelegate.h \
    openfiledelegate.h

FORMS    += mainwindow.ui

DISTFILES += \
    icon.ico
