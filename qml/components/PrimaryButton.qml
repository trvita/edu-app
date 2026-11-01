import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import EduApp

// Main call-to-action button ("Start learning!", "Next").
Button {
    id: control

    leftPadding: Theme.margin * 2
    rightPadding: Theme.margin * 2

    contentItem: AppText {
        text: control.text
        color: Theme.textOnPrimary
        font.pixelSize: Theme.fontSizeButton
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        implicitHeight: Theme.buttonHeight
        radius: Theme.radius
        color: !control.enabled ? Theme.disabled
             : control.down ? Theme.primaryPressed
             : Theme.primary

        layer.enabled: Theme.shadowEnabled
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: Theme.shadowColor
            shadowBlur: Theme.shadowBlur
            shadowVerticalOffset: Theme.shadowOffset
        }
    }
}
