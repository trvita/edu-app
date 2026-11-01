import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page

    property bool showBack: true

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                text: "←"
                visible: page.showBack
                onClicked: page.StackView.view.pop()
            }

            Label {
                text: page.title
                font.bold: true
                elide: Label.ElideRight
                Layout.fillWidth: true
                Layout.leftMargin: page.showBack ? 0 : 16
            }
        }
    }
}
