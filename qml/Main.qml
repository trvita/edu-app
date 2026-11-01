import QtQuick
import QtQuick.Controls
import EduApp

ApplicationWindow {
    width: 360
    height: 640
    visible: true
    title: "EduApp"

            Shortcut {
        sequences: ["Back", "Esc"]
        enabled: stack.depth > 1
        onActivated: stack.pop()
    }

    StackView {
        id: stack
        anchors.fill: parent

        initialItem: BasePage {
            title: "EduApp"
            showBack: false

            Button {
                anchors.centerIn: parent
                text: "Start learning!"
                onClicked: stack.push(modulesPage)
            }
        }
    }

    Component {
        id: modulesPage

        ListPage {
            title: "Modules"
            model: Content.modules
            onItemChosen: index => stack.push(lessonsPage, { moduleIndex: index })
        }
    }

    Component {
        id: lessonsPage

        ListPage {
            property int moduleIndex

            title: Content.modules[moduleIndex].title
            model: Content.modules[moduleIndex].lessons
            onItemChosen: index => stack.push(lessonPage, { moduleIndex: moduleIndex, lessonIndex: index })
        }
    }

    Component {
        id: lessonPage

        LessonPage {
            property int moduleIndex
            property int lessonIndex
            readonly property var lessons: Content.modules[moduleIndex].lessons

            title: lessons[lessonIndex].title
            text: lessons[lessonIndex].text

                                    onNextRequested: {
                if (lessonIndex + 1 < lessons.length)
                    stack.replace(lessonPage, { moduleIndex: moduleIndex, lessonIndex: lessonIndex + 1 })
                else
                    stack.pop()
            }
        }
    }
}
