import QtQuick
import EduApp

// Coin amount with the coin sign, e.g. "🟡 12".
AppText {
    property int amount

    text: "🟡 " + amount
    color: Theme.textOnPrimary
    font.bold: true
    Accessible.name: qsTr("%n coin(s)", "", amount)
}
