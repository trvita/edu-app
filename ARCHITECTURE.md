# Architecture and file ownership

| Layer | Files | Owner | Rules |
|---|---|---|---|
| Logic | `src/` (except `main.cpp`), `tests/` | logic | Business rules and data in C++, exposed to QML as singletons. Every rule has a unit test. |
| Navigation | `qml/Main.qml` | logic | Connects page signals to logic calls and `StackView` changes. No layout or styling. |
| Pages | `qml/pages/` | design | Data in through properties, user actions out through signals. No `StackView`, no logic singletons. |
| Components | `qml/components/` | design | Reusable controls (buttons, etc.). Same rules as pages. |
| Style | `qml/style/Theme.qml`, `qtquickcontrols2.conf`, `assets/` | design | `Theme` holds all colors, radii, shadows, spacing, sizes and fonts. Controls use the Basic style; components replace their `background`/`contentItem` using `Theme` values. Fonts and images go to `assets/`. |

Learning content (`content/`: `manifest.json` + Markdown lessons) is data, not code; content
authors edit only that folder (see `content/README.md`). `ContentRepository` loads it from the
app resources (`:/content`); it can load any folder with the same layout, e.g. one downloaded
from the server later.

Learner state (progress, quiz attempts, coins) changes only through `LearningService`, which
saves everything to one local file (`state.json` in the app data folder) after each change.
App-wide values that every page shows (the coin balance) are properties of the
`ApplicationWindow` in `Main.qml`; `BasePage` reads them via `ApplicationWindow.window`, so pages
still don't use logic singletons.

The contract between the two owners is each page's public properties and signals.
Changing it touches both layers, so do it in one task, not in parallel.

All user-visible text in QML goes through `qsTr()` and in C++ through `tr()`.

## Build

Android (from this folder):

```bash
~/Qt/6.12.0/android_arm64_v8a/bin/qt-cmake -S . -B build-android \
    -DANDROID_SDK_ROOT=$HOME/Android/Sdk -DANDROID_NDK_ROOT=$HOME/Android/Sdk/ndk/27.2.12479018 \
    -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24 -DCMAKE_BUILD_TYPE=Debug
cmake --build build-android --target apk
adb install -r build-android/android-build/appEduApp.apk
```

Unit tests (host):

```bash
~/Qt/6.12.0/gcc_64/bin/qt-cmake -S . -B build-host -DCMAKE_BUILD_TYPE=Debug
cmake --build build-host --target tst_contentrepository
ctest --test-dir build-host --output-on-failure
```

## C++ code style

`.clang-format` (Qt-like style) and `.clang-tidy` (static analysis) apply to `src/` and `tests/`.
Use the LLVM 18 tools from the NDK; the host build provides `compile_commands.json`.

```bash
LLVM=~/Android/Sdk/ndk/27.2.12479018/toolchains/llvm/prebuilt/linux-x86_64/bin
$LLVM/clang-format -i src/*.cpp src/*.h tests/*.cpp       # format
$LLVM/clang-tidy -p build-host src/*.cpp tests/*.cpp      # analyze
```

`cmake` here is CMake 4.4.4 (`~/Downloads/packages/cmake-4.4.4-linux-x86_64/bin`).
