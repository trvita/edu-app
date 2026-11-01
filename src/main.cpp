#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    // Used for the app data folder (saved progress) on desktop builds.
    QGuiApplication::setApplicationName(QStringLiteral("EduApp"));

    QQmlApplicationEngine engine;

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
        []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    engine.loadFromModule("EduApp", "Main");

    return QGuiApplication::exec();
}
