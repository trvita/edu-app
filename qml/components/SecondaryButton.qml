import QtQuick
import QtQuick.Controls
import EduApp

// Less prominent button next to a PrimaryButton ("Retake test").
Button {
    id: control

    leftPadding: Theme.margin * 2
    rightPadding: Theme.margin * 2

    contentItem: AppText {
        text: control.text
        color: Theme.primary
        font.pixelSize: Theme.fontSizeButton
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        implicitHeight: Theme.buttonHeight
        radius: Theme.radius
        color: control.down ? Theme.surfacePressed : Theme.surface
        border.color: Theme.primary
        border.width: 2
    }
}
