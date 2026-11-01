import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import EduApp

// Scrollable lesson text with a "Next" button after it.
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
            spacing: Theme.spacing

            AppText {
                id: body
                textFormat: Text.MarkdownText
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                Layout.margins: Theme.margin
            }

            PrimaryButton {
                text: qsTr("Next")
                Layout.alignment: Qt.AlignHCenter
                Layout.bottomMargin: Theme.margin
                onClicked: page.nextRequested()
            }
        }
    }
}
