import QtQuick
import QtQuick.Controls

BasePage {
    id: page

    property alias model: list.model

    signal itemChosen(int index)

    ListView {
        id: list
        anchors.fill: parent

        delegate: ItemDelegate {
            required property int index
            required property var modelData

            width: ListView.view.width
            text: modelData.title
            onClicked: page.itemChosen(index)
        }
    }
}
