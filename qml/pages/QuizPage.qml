import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import EduApp

// All quiz questions on one page. Answers can be changed until "Submit", which is enabled
// only when every question has an answer.
BasePage {
    id: page

    // [{ text, multiple, options: [string] }]
    property var questions: []
    // Selected option indices per question, e.g. [[1], [0, 2]].
    property var answers: questions.map(() => [])
    readonly property bool allAnswered: answers.length > 0 && answers.every(a => a.length > 0)

    signal submitted(var answers)

    function setOption(questionIndex, optionIndex, selected, multiple) {
        const updated = answers.slice()
        const current = multiple ? updated[questionIndex].filter(i => i !== optionIndex) : []
        if (selected)
            current.push(optionIndex)
        updated[questionIndex] = current.sort((a, b) => a - b)
        answers = updated
    }

    Flickable {
        id: flick
        anchors.fill: parent
        contentHeight: column.implicitHeight
        clip: true

        ScrollBar.vertical: ScrollBar {}

        ColumnLayout {
            id: column
            width: flick.width
            spacing: Theme.spacing

            Repeater {
                model: page.questions

                QuestionCard {
                    required property int index
                    required property var modelData

                    number: index + 1
                    text: modelData.text
                    multiple: modelData.multiple
                    options: modelData.options
                    Layout.fillWidth: true
                    Layout.leftMargin: Theme.margin
                    Layout.rightMargin: Theme.margin
                    Layout.topMargin: index === 0 ? Theme.margin : 0
                    onOptionToggled: (optionIndex, selected) =>
                        page.setOption(index, optionIndex, selected, modelData.multiple)
                }
            }

            PrimaryButton {
                text: qsTr("Submit")
                enabled: page.allAnswered
                Layout.alignment: Qt.AlignHCenter
                Layout.bottomMargin: Theme.margin
                onClicked: page.submitted(page.answers)
            }
        }
    }
}
