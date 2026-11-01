#pragma once

#include <QDate>
#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

#include "ProgressTracker.h"
#include "Wallet.h"

class ContentRepository;
class QJSEngine;
class QQmlEngine;

// Entry point for everything that changes the learner's state: quiz submissions, lesson
// completion and coin rewards. Saves progress and coins together to one local file after
// every change, so they always stay consistent (Stage 2: single user, local storage).
//
// Reward rules (Q-ECON-01..03, Q-QUIZ-04):
// - first quiz attempt only: 1 coin per point (score rounded up);
// - first attempt with 100%: 5 extra coins;
// - 1 coin for the first app start of each day.
class LearningService : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(ProgressTracker *progress READ progress CONSTANT)
    Q_PROPERTY(Wallet *wallet READ wallet CONSTANT)

public:
    static constexpr int kCoinsPerPoint = 1;
    static constexpr int kPerfectQuizBonus = 5;
    static constexpr int kDailyLoginReward = 1;

    // Loads the saved state from statePath (if any). The content must outlive the service.
    LearningService(const ContentRepository *content, QString statePath, QObject *parent = nullptr);

    // QML singleton factory: uses the ContentRepository singleton and the default state file,
    // and grants today's login reward.
    static LearningService *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);
    static QString defaultStatePath();

    ProgressTracker *progress() const { return m_progress; }
    Wallet *wallet() const { return m_wallet; }

    // Grants the daily login reward once per calendar day. Returns the coins granted.
    int claimDailyReward(const QDate &today = QDate::currentDate());

    // Evaluates the answers, records the attempt, completes the lesson if passed and grants
    // first-attempt rewards. Returns ContentRepository::evaluateQuiz() fields plus
    // { firstAttempt, coinsEarned }.
    Q_INVOKABLE QVariantMap submitQuiz(int moduleIndex, int lessonIndex,
                                       const QVariantList &answers);
    // For lessons without a quiz: completes the lesson after reading.
    Q_INVOKABLE void completeLessonWithoutQuiz(int moduleIndex, int lessonIndex);

private:
    void load();
    void save() const;

    const ContentRepository *m_content;
    QString m_statePath;
    ProgressTracker *m_progress;
    Wallet *m_wallet;
};
