import QtQuick
import QtQuick.Controls
import EduApp

// Back arrow used in page headers.
ToolButton {
    id: control

    Accessible.name: qsTr("Back")

    contentItem: AppText {
        text: "←"
        color: Theme.textOnPrimary
        font.pixelSize: Theme.fontSizeTitle
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        implicitWidth: Theme.headerHeight
        implicitHeight: Theme.headerHeight
        radius: width / 2
        color: control.down ? Qt.rgba(1, 1, 1, 0.2) : "transparent"
    }
}
