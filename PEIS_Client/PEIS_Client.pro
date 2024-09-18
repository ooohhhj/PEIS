QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 c++14

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    captchagenerator.cpp \
    clientsocket.cpp \
    logindialog.cpp \
    main.cpp \
    passwordrecoverydialog.cpp \
    protocol.cpp \
    registerdialog.cpp \
    usermainwindow.cpp

HEADERS += \
    captchagenerator.h \
    clientsocket.h \
    logindialog.h \
    passwordrecoverydialog.h \
    protocol.h \
    registerdialog.h \
    usermainwindow.h

FORMS += \
    logindialog.ui \
    passwordrecoverydialog.ui \
    registerdialog.ui \
    usermainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ../Resource/icom.qrc
