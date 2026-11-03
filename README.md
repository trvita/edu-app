# EduApp

Educational Android app in C++ and QML (Qt 6).

## Requirements

- Qt 6.5 or newer with the Android arm64-v8a kit and the Desktop gcc_64 kit
- CMake 3.22 or newer
- Android SDK with platform-tools (adb)
- Android NDK
- An Android phone (Android 7.0 / API 24 or newer) with USB debugging enabled

## Build and install

```bash
qt-cmake -S . -B build-android \
    -DANDROID_SDK_ROOT=$ANDROID_SDK_HOME \
    -DANDROID_NDK_ROOT=$ANDROID_NDK_HOME \
    -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24 \
    -DCMAKE_BUILD_TYPE=Debug
cmake --build build-android --target apk
adb install -r build-android/android-build/appEduApp.apk
```

## Build and run on the desktop

```bash
qt-cmake -S . -B build-host -DCMAKE_BUILD_TYPE=Debug
cmake --build build-host
./build-host/appEduApp

# run tests on host
ctest --test-dir build-host --output-on-failure
```


## More

- `ARCHITECTURE.md`: project structure and code style tools
- `content/README.md`: how to edit lessons and quizzes
