import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import EduApp

// Modal question with a confirm and a cancel button. Emits accepted() or rejected();
// the Android back gesture cancels it.
Dialog {
    id: dialog

    property string message
    property string confirmText: qsTr("OK")
    property string cancelText: qsTr("Cancel")
    // Shows the confirm button as a destructive action.
    property bool destructive: false

    parent: Overlay.overlay
    anchors.centerIn: parent
    width: Math.min(parent.width - Theme.margin * 2, 400)
    modal: true
    padding: Theme.margin
    closePolicy: Popup.CloseOnEscape

    background: Rectangle {
        radius: Theme.radius
        color: Theme.surface
    }

    contentItem: ColumnLayout {
        spacing: Theme.spacingSmall

        AppText {
            text: dialog.title
            heading: true
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        AppText {
            text: dialog.message
            color: Theme.textSecondary
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
    }

    footer: RowLayout {
        spacing: Theme.spacingSmall

        SecondaryButton {
            text: dialog.cancelText
            Layout.fillWidth: true
            Layout.leftMargin: Theme.margin
            Layout.bottomMargin: Theme.margin
            onClicked: dialog.reject()
        }

        SecondaryButton {
            text: dialog.confirmText
            accent: dialog.destructive ? Theme.error : Theme.primary
            Layout.fillWidth: true
            Layout.rightMargin: Theme.margin
            Layout.bottomMargin: Theme.margin
            onClicked: dialog.accept()
        }
    }
}
