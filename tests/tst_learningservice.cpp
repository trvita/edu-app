#include <QTemporaryDir>
#include <QtTest>

#include "ContentRepository.h"
#include "LearningService.h"

// Uses tests/data/content. Lesson A1 ("a-1") has a quiz with 2 questions (max 2 points):
// [[1], [0, 2]] is 100%, [[1], [0, 1]] is 50% (1 point), [[0], [1]] is 0%.
class TestLearningService : public QObject
{
    Q_OBJECT

private slots:
    void init()
    {
        QVERIFY(m_dir.isValid());
        QFile::remove(statePath());
        QFile::remove(statePath() + ".corrupt");
    }

    void perfectFirstAttemptGivesPointsAndBonus()
    {
        LearningService service(&m_content, statePath());
        const QVariantMap result = service.submitQuiz(0, 0, perfectAnswers());
        QCOMPARE(result.value("percent").toInt(), 100);
        QCOMPARE(result.value("firstAttempt").toBool(), true);
        QCOMPARE(result.value("coinsEarned").toInt(), 2 + LearningService::kPerfectQuizBonus);
        QCOMPARE(service.wallet()->balance(), 7);
        QVERIFY(service.progress()->isLessonCompleted(0, 0));
    }

    void failedFirstAttemptGivesPointsButDoesNotComplete()
    {
        LearningService service(&m_content, statePath());
        const QVariantMap result = service.submitQuiz(0, 0, halfAnswers());
        QCOMPARE(result.value("passed").toBool(), false);
        QCOMPARE(result.value("coinsEarned").toInt(), 1);
        QCOMPARE(service.wallet()->balance(), 1);
        QVERIFY(!service.progress()->isLessonCompleted(0, 0));
    }

    void retakeGivesNoCoinsButCanComplete()
    {
        LearningService service(&m_content, statePath());
        service.submitQuiz(0, 0, halfAnswers());
        const QVariantMap retake = service.submitQuiz(0, 0, perfectAnswers());
        QCOMPARE(retake.value("firstAttempt").toBool(), false);
        QCOMPARE(retake.value("coinsEarned").toInt(), 0);
        QCOMPARE(service.wallet()->balance(), 1);
        QVERIFY(service.progress()->isLessonCompleted(0, 0));
        QCOMPARE(service.progress()->attempts().size(), 2);
    }

    void zeroScoreFirstAttemptGivesNothing()
    {
        LearningService service(&m_content, statePath());
        const QVariantMap result = service.submitQuiz(0, 0, {QVariantList{0}, QVariantList{1}});
        QCOMPARE(result.value("percent").toInt(), 0);
        QCOMPARE(result.value("coinsEarned").toInt(), 0);
        QCOMPARE(service.wallet()->balance(), 0);
    }

    void stateSurvivesRestart()
    {
        {
            LearningService service(&m_content, statePath());
            service.claimDailyReward(QDate(2026, 11, 2));
            service.submitQuiz(0, 0, perfectAnswers());
        }
        const LearningService restarted(&m_content, statePath());
        QCOMPARE(restarted.wallet()->balance(), 1 + 7);
        QVERIFY(restarted.progress()->isLessonCompleted(0, 0));
        QVERIFY(restarted.progress()->isLessonUnlocked(0, 1));
        QCOMPARE(restarted.progress()->attempts().size(), 1);
    }

    void firstAttemptRuleSurvivesRestart()
    {
        {
            LearningService service(&m_content, statePath());
            service.submitQuiz(0, 0, halfAnswers());
        }
        LearningService restarted(&m_content, statePath());
        const QVariantMap retake = restarted.submitQuiz(0, 0, perfectAnswers());
        QCOMPARE(retake.value("coinsEarned").toInt(), 0);
        QCOMPARE(restarted.wallet()->balance(), 1);
    }

    void dailyRewardOncePerDay()
    {
        LearningService service(&m_content, statePath());
        QCOMPARE(service.claimDailyReward(QDate(2026, 11, 2)), 1);
        QCOMPARE(service.claimDailyReward(QDate(2026, 11, 2)), 0);
        QCOMPARE(service.claimDailyReward(QDate(2026, 11, 3)), 1);
        QCOMPARE(service.wallet()->balance(), 2);
    }

    void resetErasesProgressAndCoinsAndIsSaved()
    {
        {
            LearningService service(&m_content, statePath());
            service.claimDailyReward(QDate(2026, 11, 2));
            service.submitQuiz(0, 0, perfectAnswers());
            service.resetProgress();
            QCOMPARE(service.wallet()->balance(), 0);
            QVERIFY(!service.progress()->isLessonUnlocked(0, 1));
        }
        LearningService restarted(&m_content, statePath());
        QCOMPARE(restarted.wallet()->balance(), 0);
        QVERIFY(!restarted.progress()->isLessonCompleted(0, 0));
        QCOMPARE(restarted.progress()->attempts().size(), 0);

        // After a reset the next attempt is a first attempt again.
        const QVariantMap result = restarted.submitQuiz(0, 0, perfectAnswers());
        QCOMPARE(result.value("firstAttempt").toBool(), true);
        QCOMPARE(restarted.wallet()->balance(), 7);
    }

    void corruptStateFileStartsFreshAndKeepsBackup()
    {
        {
            QFile file(statePath());
            QVERIFY(file.open(QIODevice::WriteOnly));
            file.write("{ not json");
        }
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Invalid state file"));
        const LearningService service(&m_content, statePath());
        QCOMPARE(service.wallet()->balance(), 0);
        QVERIFY(QFile::exists(statePath() + ".corrupt"));
    }

private:
    QString statePath() const { return m_dir.filePath("state.json"); }
    static QVariantList perfectAnswers() { return {QVariantList{1}, QVariantList{0, 2}}; }
    static QVariantList halfAnswers() { return {QVariantList{1}, QVariantList{0, 1}}; }

    QTemporaryDir m_dir;
    // The fixture's invalid quiz and missing file are expected; their warnings are irrelevant.
    const ContentRepository m_content{QFINDTESTDATA("data/content")};
};

QTEST_GUILESS_MAIN(TestLearningService)
#include "tst_learningservice.moc"
