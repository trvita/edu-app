import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Layouts
import EduApp

// Top bar of a page: optional back arrow, the title and the coin balance on the right.
ToolBar {
    id: control

    property string title
    property bool showBack: true
    property int coins

    signal backRequested()

    background: Rectangle {
        implicitHeight: Theme.headerHeight
        color: Theme.primary

        layer.enabled: Theme.shadowEnabled
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: Theme.shadowColor
            shadowBlur: Theme.shadowBlur
            shadowVerticalOffset: Theme.shadowOffset
        }
    }

    RowLayout {
        anchors.fill: parent

        BackButton {
            visible: control.showBack
            onClicked: control.backRequested()
        }

        AppText {
            text: control.title
            heading: true
            color: Theme.textOnPrimary
            elide: Text.ElideRight
            Layout.fillWidth: true
            Layout.leftMargin: control.showBack ? 0 : Theme.margin
        }

        CoinBalance {
            amount: control.coins
            Layout.rightMargin: Theme.margin
        }
    }
}
