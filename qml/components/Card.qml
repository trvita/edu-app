import QtQuick
import QtQuick.Layouts
import EduApp

// Rounded surface; children are stacked vertically inside.
Rectangle {
    default property alias content: layout.data

    implicitHeight: layout.implicitHeight + 2 * Theme.margin
    radius: Theme.radius
    color: Theme.surface

    ColumnLayout {
        id: layout
        anchors.fill: parent
        anchors.margins: Theme.margin
        spacing: Theme.spacingSmall
    }
}
