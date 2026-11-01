import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import EduApp

// Quiz result as a percentage of the maximum points, and the correct answers for every
// question answered incorrectly. Passed: "Retake test" + "Next";
// not passed: "Retake test" + "Back to the lesson".
BasePage {
    id: page

    property int percent
    property bool passed
    property bool firstAttempt
    property int coinsEarned
    // [{ question, yourAnswers: [string], correctAnswers: [string] }]
    property var mistakes: []

    signal retakeRequested()
    signal nextRequested()
    signal backToLessonRequested()

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

            AppText {
                text: qsTr("Your result")
                color: Theme.textSecondary
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: Theme.margin * 2
            }

            AppText {
                text: qsTr("%1%").arg(page.percent)
                color: page.passed ? Theme.success : Theme.error
                font.pixelSize: Theme.fontSizeScore
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }

            AppText {
                text: page.passed ? qsTr("Test passed, the next lesson is unlocked")
                                  : qsTr("Test not passed: you need at least 67%")
                color: Theme.textSecondary
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
                Layout.leftMargin: Theme.margin
                Layout.rightMargin: Theme.margin
            }

            AppText {
                text: !page.firstAttempt ? qsTr("Coins are given only for the first attempt")
                    : page.coinsEarned > 0 ? qsTr("You earned 🟡 %1").arg(page.coinsEarned)
                    : qsTr("No coins earned this time")
                font.bold: page.firstAttempt && page.coinsEarned > 0
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
                Layout.leftMargin: Theme.margin
                Layout.rightMargin: Theme.margin
            }

            AppText {
                text: page.mistakes.length === 0 ? qsTr("All answers are correct!")
                                                 : qsTr("Correct answers")
                heading: page.mistakes.length > 0
                Layout.alignment: page.mistakes.length === 0 ? Qt.AlignHCenter : Qt.AlignLeft
                Layout.leftMargin: Theme.margin
                Layout.topMargin: Theme.margin
            }

            Repeater {
                model: page.mistakes

                Card {
                    required property var modelData

                    Layout.fillWidth: true
                    Layout.leftMargin: Theme.margin
                    Layout.rightMargin: Theme.margin

                    AppText {
                        text: modelData.question
                        font.bold: true
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }

                    AppText {
                        text: qsTr("Your answer: %1").arg(modelData.yourAnswers.join(", ") || "—")
                        color: Theme.error
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }

                    AppText {
                        text: qsTr("Correct answer: %1").arg(modelData.correctAnswers.join(", "))
                        color: Theme.success
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }
                }
            }

            PrimaryButton {
                text: page.passed ? qsTr("Next") : qsTr("Back to the lesson")
                Layout.fillWidth: true
                Layout.leftMargin: Theme.margin
                Layout.rightMargin: Theme.margin
                Layout.topMargin: Theme.margin
                onClicked: page.passed ? page.nextRequested() : page.backToLessonRequested()
            }

            SecondaryButton {
                text: qsTr("Retake test")
                Layout.fillWidth: true
                Layout.leftMargin: Theme.margin
                Layout.rightMargin: Theme.margin
                Layout.bottomMargin: Theme.margin
                onClicked: page.retakeRequested()
            }
        }
    }
}
