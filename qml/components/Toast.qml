import QtQuick
import QtQuick.Controls
import EduApp

// Short message at the bottom of the screen that hides itself.
Popup {
    id: toast

    property int duration: 2500

    function show(message) {
        label.text = message
        open()
        hideTimer.restart()
    }

    x: (parent.width - width) / 2
    y: parent.height - height - Theme.margin * 2
    width: Math.min(implicitWidth, parent.width - Theme.margin * 2)
    padding: Theme.margin
    closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape

    contentItem: AppText {
        id: label
        color: Theme.textOnPrimary
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
    }

    background: Rectangle {
        radius: Theme.radius
        color: Theme.toastBackground
    }

    Timer {
        id: hideTimer
        interval: toast.duration
        onTriggered: toast.close()
    }
}
