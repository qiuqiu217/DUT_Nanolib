QT       += core gui bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    #页面模块
    blepage.cpp \
    bluetooth/bluetoothhandler.cpp \
    bluetooth/dataprocess.cpp \
    data/algorithm.cpp \
    imupage.cpp \
    library/mybaseplot.cpp \
    library/myplantarheatmap.cpp \
    library/mywaveform.cpp \
    library/qtrack2d.cpp \
    main.cpp \
    mainwindow.cpp \
    plantarpage.cpp \
    test.cpp \
    trackpage.cpp\
    #蓝牙模块文件
    #自定义组件和库文件
    library/customWidgets/toast.cpp\
    library/plothandler.cpp \
    library/qcustomplot.cpp \
    #数据模块
    data/datacoversionutils.cpp\

HEADERS += \
    blepage.h \
    bluetooth/bluetoothhandler.h \
    bluetooth/dataprocess.h \
    data/algorithm.h \
    imupage.h \
    library/mybaseplot.h \
    library/myplantarheatmap.h \
    library/mywaveform.h \
    library/plothandler.h \
    library/qcustomplot.h \
    library/qtrack2d.h \
    mainwindow.h \
    plantarpage.h \
    test.h \
    trackpage.h\
    #蓝牙模块头文件
    #自定义组件和库文件
    library/customWidgets/toast.h\
    data/datacoversionutils.h\

FORMS += \
    blepage.ui \
    imupage.ui \
    mainwindow.ui \
    plantarpage.ui \
    trackpage.ui\



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon.qrc

QMAKE_CXXFLAGS += /source-charset:utf-8
QMAKE_CXXFLAGS += /execution-charset:utf-8
