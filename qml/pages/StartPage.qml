import QtQuick
import QtQuick.Layouts
import EduApp

BasePage {
    id: page

    signal startRequested()
    signal settingsRequested()

    title: qsTr("EduApp")
    showBack: false

    ColumnLayout {
        anchors.centerIn: parent
        spacing: Theme.spacing

        PrimaryButton {
            text: qsTr("Start learning!")
            Layout.fillWidth: true
            onClicked: page.startRequested()
        }

        SecondaryButton {
            text: qsTr("Settings")
            Layout.fillWidth: true
            onClicked: page.settingsRequested()
        }
    }
}
