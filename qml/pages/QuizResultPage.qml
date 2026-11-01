import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import EduApp

// Quiz result as a percentage of the maximum points, and the correct answers for every
// question answered incorrectly.
BasePage {
    id: page

    property int percent
    // [{ question, yourAnswers: [string], correctAnswers: [string] }]
    property var mistakes: []

    signal continueRequested()

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
                font.pixelSize: Theme.fontSizeScore
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
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
                text: qsTr("Continue")
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: Theme.margin
                Layout.bottomMargin: Theme.margin
                onClicked: page.continueRequested()
            }
        }
    }
}
