#include <QSignalSpy>
#include <QtTest>

#include "ProgressTracker.h"

// Course used by every test: module 0 has 2 lessons, module 1 has 3, module 2 has 1.
class TestProgressTracker : public QObject
{
    Q_OBJECT

private slots:
    void initiallyOnlyFirstModuleAndLessonAreUnlocked()
    {
        const ProgressTracker progress(course());
        QVERIFY(progress.isModuleUnlocked(0));
        QVERIFY(!progress.isModuleUnlocked(1));
        QVERIFY(!progress.isModuleUnlocked(2));
        QVERIFY(progress.isLessonUnlocked(0, 0));
        QVERIFY(!progress.isLessonUnlocked(0, 1));
        QVERIFY(!progress.isLessonUnlocked(1, 0));
    }

    void completingLessonUnlocksOnlyTheNextOne()
    {
        ProgressTracker progress(course());
        progress.completeLesson(0, 0);
        QVERIFY(progress.isLessonCompleted(0, 0));
        QVERIFY(progress.isLessonUnlocked(0, 1));
        QVERIFY(!progress.isModuleUnlocked(1));
    }

    void completingModuleUnlocksNextModuleAndItsFirstLesson()
    {
        ProgressTracker progress(course());
        progress.completeLesson(0, 0);
        progress.completeLesson(0, 1);
        QVERIFY(progress.isModuleUnlocked(1));
        QVERIFY(progress.isLessonUnlocked(1, 0));
        QVERIFY(!progress.isLessonUnlocked(1, 1));
        QVERIFY(!progress.isModuleUnlocked(2));
    }

    void lockedLessonCannotBeCompleted()
    {
        ProgressTracker progress(course());
        progress.completeLesson(0, 1); // locked: lesson 0 not completed
        progress.completeLesson(1, 0); // locked: module 1 locked
        QVERIFY(!progress.isLessonCompleted(0, 1));
        QVERIFY(!progress.isLessonCompleted(1, 0));
    }

    void invalidIndicesAreLockedAndIgnored()
    {
        ProgressTracker progress(course());
        progress.completeLesson(-1, 0);
        progress.completeLesson(0, 99);
        progress.completeLesson(99, 0);
        QVERIFY(!progress.isModuleUnlocked(-1));
        QVERIFY(!progress.isModuleUnlocked(99));
        QVERIFY(!progress.isLessonUnlocked(0, -1));
        QVERIFY(!progress.isLessonUnlocked(0, 99));
        QVERIFY(!progress.isLessonCompleted(99, 0));
    }

    void progressChangedOnlyOnNewCompletion()
    {
        ProgressTracker progress(course());
        QSignalSpy spy(&progress, &ProgressTracker::progressChanged);
        progress.completeLesson(0, 0);
        progress.completeLesson(0, 0); // already completed
        progress.completeLesson(1, 0); // locked
        QCOMPARE(spy.count(), 1);
    }

    void qmlListsReflectProgress()
    {
        ProgressTracker progress(course());
        progress.completeLesson(0, 0);
        progress.completeLesson(0, 1);
        QCOMPARE(progress.unlockedModules(), QVariantList({true, true, false}));
        const QVariantList lessons = progress.unlockedLessons();
        QCOMPARE(lessons.size(), 3);
        QCOMPARE(lessons[0].toList(), QVariantList({true, true}));
        QCOMPARE(lessons[1].toList(), QVariantList({true, false, false}));
        QCOMPARE(lessons[2].toList(), QVariantList({false}));
    }

    void attemptsAreRecordedPerLesson()
    {
        ProgressTracker progress(course());
        QVERIFY(!progress.hasAttempts("a1"));
        progress.recordAttempt({"a1", 1, 50, false, QDateTime::currentDateTimeUtc()});
        QVERIFY(progress.hasAttempts("a1"));
        QVERIFY(!progress.hasAttempts("a2"));
    }

    void jsonRoundTripKeepsProgressAndAttempts()
    {
        ProgressTracker original(course());
        original.completeLesson(0, 0);
        original.completeLesson(0, 1);
        const QDateTime time(QDate(2026, 11, 2), QTime(10, 0), QTimeZone::UTC);
        original.recordAttempt({"a2", 3, 100, true, time});

        ProgressTracker restored(course());
        QSignalSpy spy(&restored, &ProgressTracker::progressChanged);
        restored.fromJson(original.toJson());
        QCOMPARE(spy.count(), 1);
        QVERIFY(restored.isLessonCompleted(0, 1));
        QVERIFY(restored.isModuleUnlocked(1));
        QCOMPARE(restored.attempts().size(), 1);
        QCOMPARE(restored.attempts()[0].lessonId, QString("a2"));
        QCOMPARE(restored.attempts()[0].percent, 100);
        QCOMPARE(restored.attempts()[0].submittedAt, time);
    }

    void resetLocksEverythingExceptFirstLessonAgain()
    {
        ProgressTracker progress(course());
        progress.completeLesson(0, 0);
        progress.completeLesson(0, 1);
        progress.recordAttempt({"a1", 2, 100, true, QDateTime::currentDateTimeUtc()});
        const QSignalSpy spy(&progress, &ProgressTracker::progressChanged);
        progress.reset();
        QCOMPARE(spy.count(), 1);
        QVERIFY(!progress.isLessonCompleted(0, 0));
        QVERIFY(!progress.isLessonUnlocked(0, 1));
        QVERIFY(!progress.isModuleUnlocked(1));
        QVERIFY(!progress.hasAttempts("a1"));
    }

    void progressFollowsLessonIdsWhenContentIsReordered()
    {
        ProgressTracker original(course());
        original.completeLesson(0, 0); // "a1"

        // Same lessons, module 0 reordered: "a1" is now the second lesson.
        ProgressTracker reordered({{"a2", "a1"}, {"b1", "b2", "b3"}, {"c1"}});
        reordered.fromJson(original.toJson());
        QVERIFY(!reordered.isLessonCompleted(0, 0));
        QVERIFY(reordered.isLessonCompleted(0, 1));
    }

private:
    static QList<QStringList> course() { return {{"a1", "a2"}, {"b1", "b2", "b3"}, {"c1"}}; }
};

QTEST_APPLESS_MAIN(TestProgressTracker)
#include "tst_progresstracker.moc"
