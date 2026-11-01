#pragma once

#include <QDateTime>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QSet>
#include <QStringList>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

// Learning progress and unlocking rules (Q-EDU-01/02):
// - a lesson is completed when its quiz is passed (or it has no quiz and was read);
// - the first lesson of an unlocked module is unlocked, every next one needs the previous
//   lesson completed;
// - the first module is unlocked, every next one needs all lessons of the previous module
//   completed.
// Completion is never undone. Progress is stored by lesson id, so it survives content
// reordering. Also keeps the history of quiz attempts.
class ProgressTracker : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Owned by LearningService")

    // [bool] per module.
    Q_PROPERTY(QVariantList unlockedModules READ unlockedModules NOTIFY progressChanged)
    // [[bool]] per module, per lesson.
    Q_PROPERTY(QVariantList unlockedLessons READ unlockedLessons NOTIFY progressChanged)

public:
    struct Attempt
    {
        QString lessonId;
        int score = 0;
        int percent = 0;
        bool passed = false;
        QDateTime submittedAt;
    };

    // lessonIds[m][l] is the id of lesson l in module m.
    explicit ProgressTracker(const QList<QStringList> &lessonIds, QObject *parent = nullptr);

    bool isModuleUnlocked(int moduleIndex) const;
    bool isLessonUnlocked(int moduleIndex, int lessonIndex) const;
    bool isLessonCompleted(int moduleIndex, int lessonIndex) const;

    QVariantList unlockedModules() const;
    QVariantList unlockedLessons() const;

    // Ignored for locked or nonexistent lessons.
    void completeLesson(int moduleIndex, int lessonIndex);

    bool hasAttempts(const QString &lessonId) const;
    void recordAttempt(const Attempt &attempt);
    QList<Attempt> attempts() const { return m_attempts; }

    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

signals:
    void progressChanged();

private:
    bool isModuleCompleted(int moduleIndex) const;
    QString lessonId(int moduleIndex, int lessonIndex) const;

    QList<QStringList> m_lessonIds;
    QSet<QString> m_completed;
    QList<Attempt> m_attempts;
};
