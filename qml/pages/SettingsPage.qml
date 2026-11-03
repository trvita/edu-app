import QtQuick
import QtQuick.Layouts
import EduApp

// App settings. "Reset progress" asks for confirmation first and only then
// emits resetConfirmed().
BasePage {
    id: page

    signal resetConfirmed()

    title: qsTr("Settings")

    ColumnLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: Theme.margin
        spacing: Theme.spacing

        Card {
            Layout.fillWidth: true

            AppText {
                text: qsTr("Progress")
                heading: true
            }

            AppText {
                text: qsTr("Erases your coins and locks all modules and lessons again.")
                color: Theme.textSecondary
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            SecondaryButton {
                text: qsTr("Reset progress")
                accent: Theme.error
                Layout.fillWidth: true
                Layout.topMargin: Theme.spacingSmall
                onClicked: confirmReset.open()
            }
        }
    }

    ConfirmDialog {
        id: confirmReset
        title: qsTr("Reset progress?")
        message: qsTr("Are you sure you want to reset your progress? All coins and unlocked modules will be lost.")
        confirmText: qsTr("Reset")
        destructive: true
        onAccepted: page.resetConfirmed()
    }
}
