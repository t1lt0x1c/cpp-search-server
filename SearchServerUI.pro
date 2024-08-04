QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Путь к заголовочным файлам

SOURCES += \
    search-server/document.cpp \
    json_libs/json.cpp \
    json_libs/json_reader.cpp \
    json_libs/json_builder.cpp \
    main.cpp \
    mainwindow.cpp \
    search-server/process_queries.cpp \
    search-server/read_input_functions.cpp \
    search-server/remove_duplicates.cpp \
    search-server/request_queue.cpp \
    search-server/search_server.cpp \
    search-server/string_processing.cpp \
    tests/test_example_functions.cpp

HEADERS += \
    search-server/concurrent_map.h \
    search-server/document.h \
    json_libs/json.h \
    json_libs/json_reader.h \
    log_libs/log_duration.h \
    mainwindow.h \
    search-server/paginator.h \
    search-server/process_queries.h \
    log_libs/qlogger.h \
    search-server/read_input_functions.h \
    search-server/remove_duplicates.h \
    search-server/request_queue.h \
    search-server/search_server.h \
    search-server/string_processing.h \
    tests/testGeneratorWord.h \
    tests/test_example_functions.h \
    tests/test_framework.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
