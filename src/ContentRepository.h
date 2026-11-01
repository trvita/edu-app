#pragma once

#include <QList>
#include <QObject>
#include <QStringList>
#include <QtQml/qqmlregistration.h>

// Read-only learning content: modules and their lessons, loaded from a content folder
// (manifest.json + Markdown lesson files, see content/README.md).
// Invalid indices return empty values instead of failing.
class ContentRepository : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QStringList moduleTitles READ moduleTitles CONSTANT)

public:
    // Content bundled into the app's resources.
    static inline const QString kBundledContentRoot = QStringLiteral(":/content");

    explicit ContentRepository(QObject *parent = nullptr);
    explicit ContentRepository(const QString &contentRoot, QObject *parent = nullptr);

    // Replaces the current content with the one in contentRoot. On a missing or invalid
    // manifest the repository is left empty; a missing lesson file gives an empty text.
    bool load(const QString &contentRoot);

    QStringList moduleTitles() const;

    Q_INVOKABLE QString moduleTitle(int moduleIndex) const;
    Q_INVOKABLE QStringList lessonTitles(int moduleIndex) const;
    Q_INVOKABLE QString lessonTitle(int moduleIndex, int lessonIndex) const;
    Q_INVOKABLE QString lessonText(int moduleIndex, int lessonIndex) const;
    Q_INVOKABLE bool hasNextLesson(int moduleIndex, int lessonIndex) const;

private:
    struct Lesson
    {
        QString title;
        QString text;
    };

    struct Module
    {
        QString title;
        QList<Lesson> lessons;
    };

    const Module *module(int moduleIndex) const;
    const Lesson *lesson(int moduleIndex, int lessonIndex) const;

    QList<Module> m_modules;
};
