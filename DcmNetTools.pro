QT += core gui widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    $$PWD/DcmNetTools/DcmNetTools.cpp  \
    $$PWD/DcmNetTools/EchoStoreSCP.cpp \
    $$PWD/DcmNetTools/EchoStoreSCU.cpp \
    $$PWD/DcmNetTools/main.cpp \
    $$PWD/DcmNetTools/Global.cpp \
    $$PWD/DcmNetTools/Terminal.cpp

HEADERS += \
    $$PWD/DcmNetTools/DcmNetTools.h  \
    $$PWD/DcmNetTools/EchoStoreSCP.h \
    $$PWD/DcmNetTools/EchoStoreSCU.h \
    $$PWD/DcmNetTools/Global.h \
    $$PWD/DcmNetTools/Terminal.h

FORMS += \
    $$PWD/DcmNetTools/DcmNetTools.ui  \
    $$PWD/DcmNetTools/EchoStoreSCP.ui \
    $$PWD/DcmNetTools/EchoStoreSCU.ui \
    $$PWD/DcmNetTools/Terminal.ui

RESOURCES += \
    $$PWD/DcmNetTools/DcmNetTools.qrc
