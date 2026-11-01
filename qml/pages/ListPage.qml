import QtQuick
import EduApp

// List of titles (a string list); reports which one was chosen.
BasePage {
    id: page

    property alias model: list.model

    signal itemChosen(int index)

    ListView {
        id: list
        anchors.fill: parent
        anchors.margins: Theme.margin
        spacing: Theme.spacingSmall

        delegate: ListItem {
            required property int index
            required property string modelData

            width: ListView.view.width
            text: modelData
            onClicked: page.itemChosen(index)
        }
    }
}
