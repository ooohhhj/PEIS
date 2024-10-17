QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 c++14

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    adminmainwindow.cpp \
    appointmentmanagement.cpp \
    appointmentsmanager_nurse.cpp \
    cancel_appointmentmanger.cpp \
    captchagenerator.cpp \
    checkuprecord.cpp \
    checkupreport.cpp \
    clientsocket.cpp \
    editmedicalexaminationreport.cpp \
    electronicmedicalrecord.cpp \
    healthexaminationreport.cpp \
    hepdetails.cpp \
    inputmedicaexaminationdata.cpp \
    logindialog.cpp \
    main.cpp \
    nurse.cpp \
    passwordrecoverydialog.cpp \
    patientinformation.cpp \
    protocol.cpp \
    registerdialog.cpp \
    schedulecheckup.cpp \
    staffmainwindow.cpp \
    userinfo.cpp \
    usermainwindow.cpp

HEADERS += \
    adminmainwindow.h \
    appointmentmanagement.h \
    appointmentsmanager_nurse.h \
    cancel_appointmentmanger.h \
    captchagenerator.h \
    checkuprecord.h \
    checkupreport.h \
    clientsocket.h \
    editmedicalexaminationreport.h \
    electronicmedicalrecord.h \
    healthexaminationreport.h \
    hepdetails.h \
    inputmedicaexaminationdata.h \
    logindialog.h \
    nurse.h \
    passwordrecoverydialog.h \
    patientinformation.h \
    protocol.h \
    registerdialog.h \
    schedulecheckup.h \
    staffmainwindow.h \
    userinfo.h \
    usermainwindow.h

FORMS += \
    adminmainwindow.ui \
    appointmentmanagement.ui \
    appointmentsmanager_nurse.ui \
    cancel_appointmentmanger.ui \
    checkuprecord.ui \
    checkupreport.ui \
    editmedicalexaminationreport.ui \
    electronicmedicalrecord.ui \
    healthexaminationreport.ui \
    hepdetails.ui \
    inputmedicaexaminationdata.ui \
    logindialog.ui \
    nurse.ui \
    passwordrecoverydialog.ui \
    patientinformation.ui \
    registerdialog.ui \
    schedulecheckup.ui \
    staffmainwindow.ui \
    userinfo.ui \
    usermainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += H:/msys64/mingw64/include/poppler/qt5
LIBS += -LH:/msys64/mingw64/lib -lpoppler -lpoppler-qt5
LIBS += -LH:/msys64/mingw64/bin             \

RESOURCES += \
    ../Resource/icom.qrc







