import QtQuick
import QtQuick.Controls
import EduApp

// Page with a header: optional back arrow and the title.
// Only reports backRequested; Main.qml decides what "back" does.
Page {
    id: page

    property bool showBack: true

    signal backRequested()

    background: Rectangle {
        color: Theme.background
    }

    header: PageHeader {
        title: page.title
        showBack: page.showBack
        onBackRequested: page.backRequested()
    }
}
