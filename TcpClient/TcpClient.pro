QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += $$PWD/libs/libcomponents.a
INCLUDEPATH += $$PWD/libs/Include

LIBS += -L$$PWD/sdk/lib/libvlc.lib

INCLUDEPATH += $$PWD/sdk/include
DEPENDPATH += $$PWD/sdk/include

LIBS += -L$$PWD/sdk/lib/llibvlccore.lib

INCLUDEPATH += $$PWD/sdk/include
DEPENDPATH += $$PWD/sdk/include



SOURCES += \
    fileshareinfowidget.cpp \
    framelessWidget.cpp \
    friendwidget.cpp \
    main.cpp \
    mainwidget.cpp \
    operatewidget.cpp \
    protocol.cpp \
    shareselectwidget.cpp \
    tcpclient.cpp

HEADERS += \
    fileshareinfowidget.h \
    framelessWidget.h \
    friendwidget.h \
    mainwidget.h \
    operatewidget.h \
    protocol.h \
    shareselectwidget.h \
    tcpclient.h

FORMS += \
    fileshareinfowidget.ui \
    framelessWidget.ui \
    friendwidget.ui \
    mainwidget.ui \
    operatewidget.ui \
    shareselectwidget.ui \
    tcpclient.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

