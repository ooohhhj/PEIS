QT -= gui
QT += core network sql printsupport

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        clienthandler.cpp \
        databasemanager.cpp \
        main.cpp \
        protocol.cpp \
        serversocket.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    clienthandler.h \
    databasemanager.h \
    protocol.h \
    serversocket.h

FORMS += \
    serversocket.ui
