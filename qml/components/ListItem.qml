import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import EduApp

// One row of a list (a module or a lesson), drawn as a rounded card.
ItemDelegate {
    id: control

    padding: Theme.margin

    contentItem: AppText {
        text: control.text
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        implicitHeight: Theme.listItemHeight
        radius: Theme.radius
        color: control.down ? Theme.surfacePressed : Theme.surface

        layer.enabled: Theme.shadowEnabled
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: Theme.shadowColor
            shadowBlur: Theme.shadowBlur
            shadowVerticalOffset: Theme.shadowOffset
        }
    }
}
