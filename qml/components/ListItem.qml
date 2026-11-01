import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Layouts
import EduApp

// One row of a list (a module or a lesson), drawn as a rounded card.
// A locked row stays clickable but looks different (color and lock sign).
ItemDelegate {
    id: control

    property bool locked: false

    padding: Theme.margin

    contentItem: RowLayout {
        spacing: Theme.spacingSmall

        AppText {
            text: control.text
            color: control.locked ? Theme.textLocked : Theme.textPrimary
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            Layout.fillWidth: true
        }

        AppText {
            visible: control.locked
            text: "🔒"
            Accessible.name: qsTr("Locked")
        }
    }

    background: Rectangle {
        implicitHeight: Theme.listItemHeight
        radius: Theme.radius
        color: control.locked ? Theme.locked
             : control.down ? Theme.surfacePressed
             : Theme.surface

        layer.enabled: Theme.shadowEnabled && !control.locked
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: Theme.shadowColor
            shadowBlur: Theme.shadowBlur
            shadowVerticalOffset: Theme.shadowOffset
        }
    }
}
