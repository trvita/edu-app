import QtQuick
import QtQuick.Controls
import EduApp

// Page with a header: optional back arrow and the title.
// Only reports backRequested; Main.qml decides what "back" does.
Page {
    id: page

    property bool showBack: true
    // App-wide coin balance, provided by the window (Main.qml) so pages stay free of logic.
    readonly property int coins: ApplicationWindow.window?.coins ?? 0

    signal backRequested()

    // Shows a short message on top of the page.
    function showMessage(message) {
        toast.show(message)
    }

    Toast {
        id: toast
    }

    background: Rectangle {
        color: Theme.background
    }

    header: PageHeader {
        title: page.title
        showBack: page.showBack
        coins: page.coins
        onBackRequested: page.backRequested()
    }
}
