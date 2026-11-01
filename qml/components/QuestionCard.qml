import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import EduApp

// One quiz question: radio buttons for single choice, check boxes for multiple choice.
// Reports every change of an option; the page keeps the answers.
Card {
    id: card

    property int number
    property string text
    property bool multiple
    property var options: []

    signal optionToggled(int optionIndex, bool selected)

    AppText {
        text: card.number + ". " + card.text
        font.bold: true
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
    }

    AppText {
        visible: card.multiple
        text: qsTr("Select all correct answers")
        color: Theme.textSecondary
        font.pixelSize: Theme.fontSizeSmall
    }

    Repeater {
        model: card.multiple ? [] : card.options

        RadioButton {
            required property int index
            required property string modelData

            text: modelData
            font.pixelSize: Theme.fontSizeBody
            Layout.fillWidth: true
            onToggled: card.optionToggled(index, checked)
        }
    }

    Repeater {
        model: card.multiple ? card.options : []

        CheckBox {
            required property int index
            required property string modelData

            text: modelData
            font.pixelSize: Theme.fontSizeBody
            Layout.fillWidth: true
            onToggled: card.optionToggled(index, checked)
        }
    }
}
