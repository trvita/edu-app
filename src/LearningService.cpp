#include "LearningService.h"

#include "ContentRepository.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QQmlEngine>
#include <QSaveFile>
#include <QStandardPaths>

#include <utility>

namespace {

Q_LOGGING_CATEGORY(lcState, "eduapp.state")

constexpr int kStateFormatVersion = 1;

QString quizRewardKey(const QString &lessonId)
{
    return QStringLiteral("quiz:") + lessonId;
}

QString perfectQuizRewardKey(const QString &lessonId)
{
    return QStringLiteral("quiz-perfect:") + lessonId;
}

QString dailyRewardKey(const QDate &day)
{
    return QStringLiteral("daily-login:") + day.toString(Qt::ISODate);
}

} // namespace

LearningService::LearningService(const ContentRepository *content, QString statePath,
                                 QObject *parent)
    : QObject(parent)
    , m_content(content)
    , m_statePath(std::move(statePath))
    , m_progress(new ProgressTracker(content->lessonIdsPerModule(), this))
    , m_wallet(new Wallet(this))
{
    load();
}

LearningService *LearningService::create(QQmlEngine *qmlEngine, QJSEngine * /*jsEngine*/)
{
    const auto *content =
        qmlEngine->singletonInstance<ContentRepository *>("EduApp", "ContentRepository");
    auto *service = new LearningService(content, defaultStatePath());
    service->claimDailyReward();
    return service;
}

QString LearningService::defaultStatePath()
{
    return QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))
        .filePath(QStringLiteral("state.json"));
}

int LearningService::claimDailyReward(const QDate &today)
{
    if (!m_wallet->credit(dailyRewardKey(today), kDailyLoginReward,
                          QStringLiteral("daily-login"))) {
        return 0;
    }
    save();
    return kDailyLoginReward;
}

QVariantMap LearningService::submitQuiz(int moduleIndex, int lessonIndex,
                                        const QVariantList &answers)
{
    QVariantMap result = m_content->evaluateQuiz(moduleIndex, lessonIndex, answers);
    const QString lessonId = m_content->lessonId(moduleIndex, lessonIndex);
    if (result.isEmpty() || lessonId.isEmpty()) {
        return result;
    }

    const int score = result.value(QStringLiteral("score")).toInt();
    const int percent = result.value(QStringLiteral("percent")).toInt();
    const bool passed = result.value(QStringLiteral("passed")).toBool();
    const bool firstAttempt = !m_progress->hasAttempts(lessonId);

    m_progress->recordAttempt({lessonId, score, percent, passed, QDateTime::currentDateTimeUtc()});
    if (passed) {
        m_progress->completeLesson(moduleIndex, lessonIndex);
    }

    int coinsEarned = 0;
    if (firstAttempt) {
        const int quizCoins = score * kCoinsPerPoint;
        if (m_wallet->credit(quizRewardKey(lessonId), quizCoins, QStringLiteral("quiz"))) {
            coinsEarned += quizCoins;
        }
        if (percent == 100
            && m_wallet->credit(perfectQuizRewardKey(lessonId), kPerfectQuizBonus,
                                QStringLiteral("quiz-perfect"))) {
            coinsEarned += kPerfectQuizBonus;
        }
    }
    save();

    result.insert(QStringLiteral("firstAttempt"), firstAttempt);
    result.insert(QStringLiteral("coinsEarned"), coinsEarned);
    return result;
}

void LearningService::completeLessonWithoutQuiz(int moduleIndex, int lessonIndex)
{
    if (m_content->hasQuiz(moduleIndex, lessonIndex)) {
        return;
    }
    m_progress->completeLesson(moduleIndex, lessonIndex);
    save();
}

void LearningService::resetProgress()
{
    m_progress->reset();
    m_wallet->reset();
    save();
}

void LearningService::load()
{
    QFile file(m_statePath);
    if (!file.exists()) {
        return; // first start
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qCWarning(lcState) << "Cannot open" << m_statePath << ":" << file.errorString();
        return;
    }

    QJsonParseError parseError{};
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();
    if (!document.isObject()) {
        // Keep the broken file for investigation instead of overwriting it on the next save.
        const QString backupPath = m_statePath + QStringLiteral(".corrupt");
        QFile::remove(backupPath);
        QFile::rename(m_statePath, backupPath);
        qCWarning(lcState) << "Invalid state file, moved to" << backupPath << ":"
                           << parseError.errorString();
        return;
    }

    const QJsonObject state = document.object();
    m_progress->fromJson(state.value(QStringLiteral("progress")).toObject());
    m_wallet->fromJson(state.value(QStringLiteral("wallet")).toObject());
}

void LearningService::save() const
{
    QDir().mkpath(QFileInfo(m_statePath).absolutePath());

    const QJsonObject state{
        {QStringLiteral("version"), kStateFormatVersion},
        {QStringLiteral("progress"), m_progress->toJson()},
        {QStringLiteral("wallet"), m_wallet->toJson()},
    };

    // QSaveFile writes to a temporary file and renames it, so a crash never leaves a
    // half-written state file.
    QSaveFile file(m_statePath);
    if (!file.open(QIODevice::WriteOnly)
        || file.write(QJsonDocument(state).toJson(QJsonDocument::Indented)) < 0 || !file.commit()) {
        qCWarning(lcState) << "Cannot save" << m_statePath << ":" << file.errorString();
    }
}
