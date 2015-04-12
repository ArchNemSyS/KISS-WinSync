#-------------------------------------------------
#
# Project created by QtCreator 2015-04-08T11:17:54
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = KISS-WinSync
CONFIG   += console
CONFIG   -= app_bundle

INCLUDEPATH += "C:/Program Files (x86)/Microsoft SDKs/Microsoft Sync Framework/2.1/include/"
LIBS += -lole32
LIBS += "C:/Program Files (x86)/Microsoft SDKs/Microsoft Sync Framework/2.1/lib/x86/WinSync.lib"
LIBS += "C:/Program Files (x86)/Microsoft SDKs/Microsoft Sync Framework/2.1/lib/x86/synchronization.lib"
LIBS += "C:/Program Files (x86)/Microsoft SDKs/Microsoft Sync Framework/2.1/lib/x86/FileSyncProvider.lib"
LIBS += "C:/Program Files (x86)/Microsoft SDKs/Microsoft Sync Framework/2.1/lib/x86/Metastore.lib"

TEMPLATE = app
SOURCES += main.cpp \
    syncer.cpp

HEADERS += \
    syncer.h
