QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 c++14

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    adminmainwindow.cpp \
    captchagenerator.cpp \
    checkuprecord.cpp \
    checkupreport.cpp \
    clientsocket.cpp \
    hepdetails.cpp \
    logindialog.cpp \
    main.cpp \
    passwordrecoverydialog.cpp \
    protocol.cpp \
    registerdialog.cpp \
    schedulecheckup.cpp \
    staffmainwindow.cpp \
    userinfo.cpp \
    usermainwindow.cpp

HEADERS += \
    adminmainwindow.h \
    captchagenerator.h \
    checkuprecord.h \
    checkupreport.h \
    clientsocket.h \
    hepdetails.h \
    logindialog.h \
    passwordrecoverydialog.h \
    protocol.h \
    registerdialog.h \
    schedulecheckup.h \
    staffmainwindow.h \
    userinfo.h \
    usermainwindow.h

FORMS += \
    adminmainwindow.ui \
    checkuprecord.ui \
    checkupreport.ui \
    hepdetails.ui \
    logindialog.ui \
    passwordrecoverydialog.ui \
    registerdialog.ui \
    schedulecheckup.ui \
    staffmainwindow.ui \
    userinfo.ui \
    usermainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ../Resource/icom.qrc
