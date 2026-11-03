cmake_minimum_required(VERSION 3.22)

project(EduApp VERSION 0.1 LANGUAGES CXX)

# compile_commands.json for clang-tidy and IDE code navigation.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick QuickControls2)

qt_standard_project_setup(REQUIRES 6.5)

# QML files live in subfolders; let each subfolder see the whole EduApp module (Qt >= 6.8).
if(QT_KNOWN_POLICY_QTP0004)
    qt_policy(SET QTP0004 NEW)
endif()

qt_add_executable(appEduApp
    src/main.cpp
)

set_source_files_properties(qml/style/Theme.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(appEduApp
    URI EduApp
    VERSION 1.0
    SOURCES
        src/ContentRepository.h
        src/ContentRepository.cpp
        src/LearningService.h
        src/LearningService.cpp
        src/ProgressTracker.h
        src/ProgressTracker.cpp
        src/Quiz.h
        src/Quiz.cpp
        src/Wallet.h
        src/Wallet.cpp
    QML_FILES
        qml/Main.qml
        qml/style/Theme.qml
        qml/components/AppText.qml
        qml/components/BackButton.qml
        qml/components/Card.qml
        qml/components/CoinBalance.qml
        qml/components/ConfirmDialog.qml
        qml/components/ListItem.qml
        qml/components/PageHeader.qml
        qml/components/PrimaryButton.qml
        qml/components/QuestionCard.qml
        qml/components/SecondaryButton.qml
        qml/components/Toast.qml
        qml/pages/BasePage.qml
        qml/pages/StartPage.qml
        qml/pages/ListPage.qml
        qml/pages/LessonPage.qml
        qml/pages/QuizPage.qml
        qml/pages/QuizResultPage.qml
        qml/pages/SettingsPage.qml
)

# Selects the Qt Quick Controls style at startup (must be at :/qtquickcontrols2.conf).
qt_add_resources(appEduApp "controls_conf"
    PREFIX "/"
    FILES qtquickcontrols2.conf
)

# Learning content (manifest + lesson texts), available in the app at :/content.
file(GLOB_RECURSE CONTENT_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    CONFIGURE_DEPENDS
    content/*.json
    content/*/*.md
)
qt_add_resources(appEduApp "content"
    PREFIX "/"
    FILES ${CONTENT_FILES}
)

target_include_directories(appEduApp PRIVATE src)
target_link_libraries(appEduApp PRIVATE Qt6::Quick Qt6::QuickControls2)

set_target_properties(appEduApp PROPERTIES
    QT_ANDROID_PACKAGE_NAME "org.example.eduapp"
)

# Unit tests for C++ logic run on the host only.
if(NOT ANDROID)
    find_package(Qt6 REQUIRED COMPONENTS Test)
    enable_testing()

    qt_add_executable(tst_contentrepository
        tests/tst_contentrepository.cpp
        src/ContentRepository.cpp
        src/Quiz.cpp
    )
    target_include_directories(tst_contentrepository PRIVATE src)
    target_link_libraries(tst_contentrepository PRIVATE Qt6::Qml Qt6::Test)
    add_test(NAME tst_contentrepository COMMAND tst_contentrepository)

    qt_add_executable(tst_quiz
        tests/tst_quiz.cpp
        src/Quiz.cpp
    )
    target_include_directories(tst_quiz PRIVATE src)
    target_link_libraries(tst_quiz PRIVATE Qt6::Core Qt6::Test)
    add_test(NAME tst_quiz COMMAND tst_quiz)

    qt_add_executable(tst_progresstracker
        tests/tst_progresstracker.cpp
        src/ProgressTracker.cpp
    )
    target_include_directories(tst_progresstracker PRIVATE src)
    target_link_libraries(tst_progresstracker PRIVATE Qt6::Qml Qt6::Test)
    add_test(NAME tst_progresstracker COMMAND tst_progresstracker)

    qt_add_executable(tst_wallet
        tests/tst_wallet.cpp
        src/Wallet.cpp
    )
    target_include_directories(tst_wallet PRIVATE src)
    target_link_libraries(tst_wallet PRIVATE Qt6::Qml Qt6::Test)
    add_test(NAME tst_wallet COMMAND tst_wallet)

    qt_add_executable(tst_learningservice
        tests/tst_learningservice.cpp
        src/LearningService.cpp
        src/ContentRepository.cpp
        src/ProgressTracker.cpp
        src/Quiz.cpp
        src/Wallet.cpp
    )
    target_include_directories(tst_learningservice PRIVATE src)
    target_link_libraries(tst_learningservice PRIVATE Qt6::Qml Qt6::Test)
    add_test(NAME tst_learningservice COMMAND tst_learningservice)
endif()
