QT += core gui network widgets websockets

TARGET = softrx-client
TEMPLATE = app

SOURCES += main.cpp \
	radiowindow.cpp \
	settingsdialog.cpp \
	mapclass.cpp \
	helpers.cpp \

HEADERS  += defines.h \
	helpers.hpp \
	radiowindow.h \
	settingsdialog.h \
	vector3.hpp \
	mapclass.hpp \

FORMS    += radio.ui \
	settingsdialog.ui \

#unix {
#  QT += pdf
#}

#win32-g++ {
#    QT += xml
#    CONFIG += link_pkgconfig
#    PKGCONFIG += poppler-qt5 libopenjp2
#}

#CONFIG+=c++2a

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3 -fno-math-errno
