import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

BasePage {
    id: page

    property alias text: body.text

    signal nextRequested()

    Flickable {
        id: flick
        anchors.fill: parent
        contentHeight: column.implicitHeight
        clip: true

        ScrollBar.vertical: ScrollBar {}

        ColumnLayout {
            id: column
            width: flick.width
            spacing: 16

            Label {
                id: body
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                Layout.margins: 16
            }

            Button {
                text: "Next"
                Layout.alignment: Qt.AlignHCenter
                Layout.bottomMargin: 16
                onClicked: page.nextRequested()
            }
        }
    }
}
