#include "ProgressTracker.h"

#include <QJsonArray>

#include <algorithm>

ProgressTracker::ProgressTracker(const QList<QStringList> &lessonIds, QObject *parent)
    : QObject(parent)
    , m_lessonIds(lessonIds)
{ }

bool ProgressTracker::isModuleUnlocked(int moduleIndex) const
{
    if (moduleIndex < 0 || moduleIndex >= m_lessonIds.size()) {
        return false;
    }
    return moduleIndex == 0 || isModuleCompleted(moduleIndex - 1);
}

bool ProgressTracker::isLessonUnlocked(int moduleIndex, int lessonIndex) const
{
    if (!isModuleUnlocked(moduleIndex) || lessonIndex < 0
        || lessonIndex >= m_lessonIds[moduleIndex].size()) {
        return false;
    }
    return lessonIndex == 0 || isLessonCompleted(moduleIndex, lessonIndex - 1);
}

bool ProgressTracker::isLessonCompleted(int moduleIndex, int lessonIndex) const
{
    const QString id = lessonId(moduleIndex, lessonIndex);
    return !id.isEmpty() && m_completed.contains(id);
}

QVariantList ProgressTracker::unlockedModules() const
{
    QVariantList result;
    for (int m = 0; m < m_lessonIds.size(); ++m) {
        result.append(isModuleUnlocked(m));
    }
    return result;
}

QVariantList ProgressTracker::unlockedLessons() const
{
    QVariantList result;
    for (int m = 0; m < m_lessonIds.size(); ++m) {
        QVariantList lessons;
        for (int l = 0; l < m_lessonIds[m].size(); ++l) {
            lessons.append(isLessonUnlocked(m, l));
        }
        result.append(QVariant(lessons));
    }
    return result;
}

void ProgressTracker::completeLesson(int moduleIndex, int lessonIndex)
{
    if (!isLessonUnlocked(moduleIndex, lessonIndex)
        || isLessonCompleted(moduleIndex, lessonIndex)) {
        return;
    }
    m_completed.insert(lessonId(moduleIndex, lessonIndex));
    emit progressChanged();
}

bool ProgressTracker::hasAttempts(const QString &lessonId) const
{
    return std::any_of(m_attempts.cbegin(), m_attempts.cend(), [&lessonId](const Attempt &attempt) {
        return attempt.lessonId == lessonId;
    });
}

void ProgressTracker::recordAttempt(const Attempt &attempt)
{
    m_attempts.append(attempt);
}

QJsonObject ProgressTracker::toJson() const
{
    QStringList completed(m_completed.cbegin(), m_completed.cend());
    completed.sort();

    QJsonArray attempts;
    for (const Attempt &attempt : m_attempts) {
        attempts.append(QJsonObject{
            {QStringLiteral("lessonId"), attempt.lessonId},
            {QStringLiteral("score"), attempt.score},
            {QStringLiteral("percent"), attempt.percent},
            {QStringLiteral("passed"), attempt.passed},
            {QStringLiteral("submittedAt"), attempt.submittedAt.toString(Qt::ISODate)},
        });
    }

    return {
        {QStringLiteral("completedLessons"), QJsonArray::fromStringList(completed)},
        {QStringLiteral("attempts"), attempts},
    };
}

void ProgressTracker::fromJson(const QJsonObject &json)
{
    m_completed.clear();
    const QJsonArray completed = json.value(QStringLiteral("completedLessons")).toArray();
    for (const auto &id : completed) {
        m_completed.insert(id.toString());
    }

    m_attempts.clear();
    const QJsonArray attempts = json.value(QStringLiteral("attempts")).toArray();
    for (const auto &value : attempts) {
        const QJsonObject object = value.toObject();
        m_attempts.append({
            object.value(QStringLiteral("lessonId")).toString(),
            object.value(QStringLiteral("score")).toInt(),
            object.value(QStringLiteral("percent")).toInt(),
            object.value(QStringLiteral("passed")).toBool(),
            QDateTime::fromString(object.value(QStringLiteral("submittedAt")).toString(),
                                  Qt::ISODate),
        });
    }
    emit progressChanged();
}

bool ProgressTracker::isModuleCompleted(int moduleIndex) const
{
    const QStringList &ids = m_lessonIds[moduleIndex];
    return std::all_of(ids.cbegin(), ids.cend(),
                       [this](const QString &id) { return m_completed.contains(id); });
}

QString ProgressTracker::lessonId(int moduleIndex, int lessonIndex) const
{
    if (moduleIndex < 0 || moduleIndex >= m_lessonIds.size() || lessonIndex < 0
        || lessonIndex >= m_lessonIds[moduleIndex].size()) {
        return {};
    }
    return m_lessonIds[moduleIndex][lessonIndex];
}
