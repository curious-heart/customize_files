QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    common_tools/common_tool_func.cpp \
    filescustomizer.cpp \
    logger/logger.cpp \
    main.cpp \
    main_dialog.cpp \
    version_def/version_def.cpp

HEADERS += \
    common_tools/common_tool_func.h \
    filescustomizer.h \
    logger/logger.h \
    main_dialog.h \
    version_def/version_def.h

FORMS += \
    main_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

msvc:QMAKE_CXXFLAGS += -execution-charset:utf-8
