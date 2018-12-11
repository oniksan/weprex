QT += core network quick quickcontrols2 serialport
CONFIG += c++11

RC_FILE = weprex.rc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
    protocols/modbus.c \
    protocols/libdefs.c \
    protocols/wsmodbusparameter.cpp \
    interfaces/wspollinginterface.cpp \
    interfaces/wssocketinterface.cpp \
    wsqmlapplication.cpp \
    protocols/wsabstractrrprotocol.cpp \
    protocols/wsmodbustcpprotocol.cpp \
    interfaces/wspollingrrinterface.cpp \
    protocols/wsdataconverter.cpp \
    timechart/defaultseriesrenderer.cpp \
    timechart/seriesrenderer.cpp \
    timechart/timechart.cpp \
    timechart/timeseries.cpp \
    timechart/timevalue.cpp \
    interfaces/wsserialinterface.cpp \
    protocols/wsmodbusrtuprotocol.cpp \
    utils/wssettings.cpp \
    utils/wsfile.cpp \
    conf.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

HEADERS += \
    protocols/modbus.h \
    protocols/libdefs.h \
    protocols/wsmodbusparameter.h \
    protocols/wsdataconverter.h \
    protocols/wsparametershash.h \
    interfaces/wspollinginterface.h \
    interfaces/wssocketinterface.h \
    wsqmlapplication.h \
    protocols/wsabstractrrprotocol.h \
    protocols/wsmodbustcpprotocol.h \
    interfaces/wspollingrrinterface.h \
    conf.h \
    timechart/defaultseriesrenderer.h \
    timechart/seriesrenderer.h \
    timechart/timechart.h \
    timechart/timeseries.h \
    timechart/timevalue.h \
    interfaces/wsserialinterface.h \
    protocols/wsmodbusrtuprotocol.h \
    utils/wssettings.h \
    utils/wsfile.h
