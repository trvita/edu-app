#include <QtTest>

#include "ContentRepository.h"

// Fixture: tests/data/content (module A: two lessons, module B: one lesson with a missing file).
// The real app content in content/ is checked too, so broken edits fail the tests.
class TestContentRepository : public QObject
{
    Q_OBJECT

private slots:
    void loadsModulesInManifestOrder()
    {
        const ContentRepository repo(fixtureRoot());
        QCOMPARE(repo.moduleTitles(), QStringList({"Module A", "Module B"}));
        QCOMPARE(repo.moduleTitle(1), QString("Module B"));
    }

    void loadsLessonTitlesAndText()
    {
        const ContentRepository repo(fixtureRoot());
        QCOMPARE(repo.lessonTitles(0), QStringList({"Lesson A1", "Lesson A2"}));
        QCOMPARE(repo.lessonTitle(0, 1), QString("Lesson A2"));
        QCOMPARE(repo.lessonText(0, 0), QString("Text of **A1**.\n"));
    }

    void readsLessonTextAsUtf8()
    {
        const ContentRepository repo(fixtureRoot());
        QCOMPARE(repo.lessonText(0, 1), QString::fromUtf8("Текст урока A2.\n"));
    }

    void missingLessonFileGivesEmptyText()
    {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Cannot open .*missing\\.md"));
        const ContentRepository repo(fixtureRoot());
        QCOMPARE(repo.lessonTitle(1, 0), QString("Lesson B1"));
        QVERIFY(repo.lessonText(1, 0).isEmpty());
    }

    void missingManifestGivesNoContent()
    {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Cannot open .*manifest\\.json"));
        ContentRepository repo(QStringLiteral("/nonexistent"));
        QVERIFY(repo.moduleTitles().isEmpty());

        QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Cannot open .*manifest\\.json"));
        QVERIFY(!repo.load(QStringLiteral("/nonexistent")));
    }

    void hasNextLessonStopsAtLastLesson()
    {
        const ContentRepository repo(fixtureRoot());
        QVERIFY(repo.hasNextLesson(0, 0));
        QVERIFY(!repo.hasNextLesson(0, 1));
    }

    void invalidIndicesReturnEmptyValues()
    {
        const ContentRepository repo(fixtureRoot());
        QVERIFY(repo.moduleTitle(-1).isEmpty());
        QVERIFY(repo.moduleTitle(99).isEmpty());
        QVERIFY(repo.lessonTitles(99).isEmpty());
        QVERIFY(repo.lessonText(0, 99).isEmpty());
        QVERIFY(!repo.hasNextLesson(99, 0));
    }

    void appContentIsComplete()
    {
        const ContentRepository repo(QFINDTESTDATA("../content"));
        QCOMPARE(repo.moduleTitles().size(), 3);
        for (int m = 0; m < repo.moduleTitles().size(); ++m) {
            QVERIFY2(!repo.moduleTitle(m).isEmpty(), qPrintable(QString("module %1").arg(m)));
            const QStringList lessons = repo.lessonTitles(m);
            QVERIFY2(!lessons.isEmpty(), qPrintable(QString("module %1 has no lessons").arg(m)));
            for (int l = 0; l < lessons.size(); ++l) {
                QVERIFY2(!repo.lessonText(m, l).isEmpty(),
                         qPrintable(QString("module %1 lesson %2 has no text").arg(m).arg(l)));
            }
        }
    }

private:
    static QString fixtureRoot() { return QFINDTESTDATA("data/content"); }
};

QTEST_APPLESS_MAIN(TestContentRepository)
#include "tst_contentrepository.moc"
