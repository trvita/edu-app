import QtQuick
import QtQuick.Controls
import EduApp

// Less prominent button next to a PrimaryButton ("Retake test").
Button {
    id: control

    // Text and border color; Theme.error for destructive actions ("Reset progress").
    property color accent: Theme.primary

    leftPadding: Theme.margin * 2
    rightPadding: Theme.margin * 2

    contentItem: AppText {
        text: control.text
        color: control.accent
        font.pixelSize: Theme.fontSizeButton
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        implicitHeight: Theme.buttonHeight
        radius: Theme.radius
        color: control.down ? Theme.surfacePressed : Theme.surface
        border.color: control.accent
        border.width: 2
    }
}
