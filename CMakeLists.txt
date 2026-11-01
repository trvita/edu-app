cmake_minimum_required(VERSION 3.22)

project(EduApp VERSION 0.1 LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick QuickControls2)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appEduApp
    src/main.cpp
)

set_source_files_properties(qml/Content.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(appEduApp
    URI EduApp
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/Content.qml
        qml/BasePage.qml
        qml/ListPage.qml
        qml/LessonPage.qml
)

target_link_libraries(appEduApp PRIVATE Qt6::Quick Qt6::QuickControls2)

set_target_properties(appEduApp PROPERTIES
    QT_ANDROID_PACKAGE_NAME "org.example.eduapp"
)
