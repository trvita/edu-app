#pragma once

#include <QList>
#include <QObject>
#include <QStringList>
#include <QtQml/qqmlregistration.h>

// Read-only learning content: modules and their lessons.
// Placeholder data for now; later loaded from content files.
// Invalid indices return empty values instead of failing.
class ContentRepository : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QStringList moduleTitles READ moduleTitles CONSTANT)

public:
    explicit ContentRepository(QObject *parent = nullptr);

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
