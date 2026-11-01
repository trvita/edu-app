pragma Singleton
import QtQuick

// Shared design values. Change the look here instead of in pages/components.
QtObject {
    // Colors
    readonly property color primary: "#4F46E5"
    readonly property color primaryPressed: "#4338CA"
    readonly property color textOnPrimary: "#FFFFFF"
    readonly property color background: "#F4F4F8"
    readonly property color surface: "#FFFFFF"
    readonly property color surfacePressed: "#E8E8F0"
    readonly property color textPrimary: "#1F2937"
    readonly property color textSecondary: "#6B7280"
    readonly property color disabled: "#C7C7D1"
    readonly property color locked: "#E2E2EA"
    readonly property color textLocked: "#9CA3AF"
    readonly property color toastBackground: "#E61F2937"
    readonly property color success: "#15803D"
    readonly property color error: "#B91C1C"

    // Shape
    readonly property int radius: 12
    readonly property int radiusSmall: 8

    // Shadows
    readonly property bool shadowEnabled: true
    readonly property color shadowColor: "#40000000"
    readonly property real shadowBlur: 0.5
    readonly property int shadowOffset: 2

    // Spacing
    readonly property int margin: 16
    readonly property int spacing: 16
    readonly property int spacingSmall: 8

    // Sizes
    readonly property int headerHeight: 56
    readonly property int buttonHeight: 48
    readonly property int listItemHeight: 56

    // Fonts. For a custom font: put the .ttf into assets/fonts/, add it to RESOURCES
    // in CMakeLists.txt, and use FontLoader { source: "..." }.font.family here.
    readonly property string fontFamily: Qt.application.font.family
    readonly property int fontSizeBody: 16
    readonly property int fontSizeTitle: 20
    readonly property int fontSizeButton: 16
    readonly property int fontSizeSmall: 14
    readonly property int fontSizeScore: 48
}
