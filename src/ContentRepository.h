#pragma once

#include <QList>
#include <QObject>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

#include "Quiz.h"

// Read-only learning content: modules, lessons and lesson quizzes, loaded from a content
// folder (manifest.json + Markdown lessons + quiz JSON files, see content/README.md).
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
    // manifest the repository is left empty; a missing lesson file gives an empty text;
    // a missing or invalid quiz file gives a lesson without a quiz.
    bool load(const QString &contentRoot);

    QStringList moduleTitles() const;

    Q_INVOKABLE QString moduleTitle(int moduleIndex) const;
    Q_INVOKABLE QStringList lessonTitles(int moduleIndex) const;
    Q_INVOKABLE QString lessonTitle(int moduleIndex, int lessonIndex) const;
    Q_INVOKABLE QString lessonText(int moduleIndex, int lessonIndex) const;
    Q_INVOKABLE bool hasNextLesson(int moduleIndex, int lessonIndex) const;

    // Stable lesson ids from the manifest; progress is stored by them.
    QString lessonId(int moduleIndex, int lessonIndex) const;
    QList<QStringList> lessonIdsPerModule() const;

    Q_INVOKABLE bool hasQuiz(int moduleIndex, int lessonIndex) const;
    // Questions without correct answers: [{ text, multiple, options: [string] }].
    Q_INVOKABLE QVariantList quizQuestions(int moduleIndex, int lessonIndex) const;
    // answers: selected option indices per question, e.g. [[1], [0, 2]].
    // Returns { score, maxScore, points, maxPoints, percent, passed,
    //           mistakes: [{ question, yourAnswers: [string], correctAnswers: [string] }] }.
    // Not for QML: quizzes are submitted through LearningService, which applies rewards.
    QVariantMap evaluateQuiz(int moduleIndex, int lessonIndex, const QVariantList &answers) const;

private:
    struct Lesson
    {
        QString id;
        QString title;
        QString text;
        quiz::Quiz quiz; // empty if the lesson has no quiz
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
