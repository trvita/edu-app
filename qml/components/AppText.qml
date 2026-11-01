import QtQuick
import EduApp

// All text in the app. `heading: true` for titles.
Text {
    property bool heading: false

    color: Theme.textPrimary
    font.family: Theme.fontFamily
    font.pixelSize: heading ? Theme.fontSizeTitle : Theme.fontSizeBody
    font.bold: heading
}
