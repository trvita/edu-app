import QtQuick
import QtQuick.Controls
import EduApp

// Navigation only: connects page signals to ContentRepository and page changes.
// No layout or styling here; that belongs in pages/, components/ and style/.
ApplicationWindow {
    width: 360
    height: 640
    visible: true
    title: qsTr("EduApp")

    function goBack() {
        stack.pop()
    }

    // Replaces the current page (lesson, quiz or result) with the next lesson.
    // After the last lesson, returns to the modules list.
    function goToNextLesson(moduleIndex, lessonIndex) {
        if (ContentRepository.hasNextLesson(moduleIndex, lessonIndex))
            stack.replace(lessonPage, { moduleIndex: moduleIndex, lessonIndex: lessonIndex + 1 })
        else
            stack.pop(stack.get(1))
    }

    // Android back button/gesture arrives as the Back key.
    // On the start page the shortcut is disabled, so the system closes the app as usual.
    Shortcut {
        sequences: ["Back", "Esc"]
        enabled: stack.depth > 1
        onActivated: goBack()
    }

    StackView {
        id: stack
        anchors.fill: parent

        initialItem: StartPage {
            onStartRequested: stack.push(modulesPage)
        }
    }

    Component {
        id: modulesPage

        ListPage {
            title: qsTr("Modules")
            model: ContentRepository.moduleTitles
            onBackRequested: goBack()
            onItemChosen: index => stack.push(lessonsPage, { moduleIndex: index })
        }
    }

    Component {
        id: lessonsPage

        ListPage {
            property int moduleIndex

            title: ContentRepository.moduleTitle(moduleIndex)
            model: ContentRepository.lessonTitles(moduleIndex)
            onBackRequested: goBack()
            onItemChosen: index => stack.push(lessonPage, { moduleIndex: moduleIndex, lessonIndex: index })
        }
    }

    Component {
        id: lessonPage

        LessonPage {
            property int moduleIndex
            property int lessonIndex

            title: ContentRepository.lessonTitle(moduleIndex, lessonIndex)
            text: ContentRepository.lessonText(moduleIndex, lessonIndex)
            onBackRequested: goBack()

            // Lesson, quiz and result replace each other, so "back" always returns to the lesson list.
            onNextRequested: {
                if (ContentRepository.hasQuiz(moduleIndex, lessonIndex))
                    stack.replace(quizPage, { moduleIndex: moduleIndex, lessonIndex: lessonIndex })
                else
                    goToNextLesson(moduleIndex, lessonIndex)
            }
        }
    }

    Component {
        id: quizPage

        QuizPage {
            property int moduleIndex
            property int lessonIndex

            title: qsTr("Quiz: %1").arg(ContentRepository.lessonTitle(moduleIndex, lessonIndex))
            questions: ContentRepository.quizQuestions(moduleIndex, lessonIndex)
            onBackRequested: goBack()
            onSubmitted: answers => {
                const result = ContentRepository.evaluateQuiz(moduleIndex, lessonIndex, answers)
                stack.replace(quizResultPage, {
                    moduleIndex: moduleIndex,
                    lessonIndex: lessonIndex,
                    percent: result.percent,
                    mistakes: result.mistakes
                })
            }
        }
    }

    Component {
        id: quizResultPage

        QuizResultPage {
            property int moduleIndex
            property int lessonIndex

            title: qsTr("Result: %1").arg(ContentRepository.lessonTitle(moduleIndex, lessonIndex))
            onBackRequested: goBack()
            onContinueRequested: goToNextLesson(moduleIndex, lessonIndex)
        }
    }
}
