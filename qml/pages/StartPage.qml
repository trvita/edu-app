import QtQuick
import EduApp

BasePage {
    id: page

    signal startRequested()

    title: qsTr("EduApp")
    showBack: false

    PrimaryButton {
        anchors.centerIn: parent
        text: qsTr("Start learning!")
        onClicked: page.startRequested()
    }
}
