pragma Singleton
import QtQuick

QtObject {
    readonly property var modules: [1, 2, 3].map(m => ({
        title: "Module " + m,
        lessons: [1, 2, 3].map(l => ({
            title: "Lesson " + l,
            text: placeholderText(m, l)
        }))
    }))

    function placeholderText(module, lesson) {
        const paragraph = "This is the text of lesson " + lesson + " in module " + module + ". "
            + "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
            + "incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud "
            + "exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat."
        return Array(8).fill(paragraph).join("\n\n")
    }
}
