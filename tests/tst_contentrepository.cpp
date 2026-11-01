#include <QtTest>

#include "ContentRepository.h"

// Fixture: tests/data/content (module A: two lessons, the first with a valid quiz, the second
// with an invalid one; module B: one lesson with a missing file and no quiz).
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

    void loadsQuizWithoutExposingAnswers()
    {
        const ContentRepository repo(fixtureRoot());
        QVERIFY(repo.hasQuiz(0, 0));

        const QVariantList questions = repo.quizQuestions(0, 0);
        QCOMPARE(questions.size(), 2);
        const QVariantMap multiple = questions[1].toMap();
        QCOMPARE(multiple.value("text").toString(), QString("Multiple question"));
        QCOMPARE(multiple.value("multiple").toBool(), true);
        QCOMPARE(multiple.value("options").toStringList(), QStringList({"Yes 1", "No", "Yes 2"}));
        QCOMPARE(multiple.keys(), QStringList({"multiple", "options", "text"}));
    }

    void invalidOrMissingQuizMeansNoQuiz()
    {
        QTest::ignoreMessage(QtWarningMsg,
                             QRegularExpression("Invalid quiz .*invalid\\.quiz\\.json.*exactly 1"));
        const ContentRepository repo(fixtureRoot());
        QVERIFY(!repo.hasQuiz(0, 1)); // invalid file
        QVERIFY(!repo.hasQuiz(1, 0)); // no quiz in manifest
        QVERIFY(repo.quizQuestions(1, 0).isEmpty());
    }

    void evaluateQuizReturnsScoreAndMistakes()
    {
        const ContentRepository repo(fixtureRoot());
        // single: correct (1); multiple: "Yes 1" + "No" -> 0.5 - 0.5 - 0.5 -> 0. Total 1 of 2.
        const QVariantMap result = repo.evaluateQuiz(0, 0, {QVariantList{1}, QVariantList{0, 1}});
        QCOMPARE(result.value("score").toInt(), 1);
        QCOMPARE(result.value("maxScore").toInt(), 2);
        QCOMPARE(result.value("points").toDouble(), 1.0);
        QCOMPARE(result.value("maxPoints").toDouble(), 2.0);
        QCOMPARE(result.value("percent").toInt(), 50);
        QCOMPARE(result.value("passed").toBool(), false);

        const QVariantList mistakes = result.value("mistakes").toList();
        QCOMPARE(mistakes.size(), 1);
        const QVariantMap mistake = mistakes[0].toMap();
        QCOMPARE(mistake.value("question").toString(), QString("Multiple question"));
        QCOMPARE(mistake.value("yourAnswers").toStringList(), QStringList({"Yes 1", "No"}));
        QCOMPARE(mistake.value("correctAnswers").toStringList(), QStringList({"Yes 1", "Yes 2"}));
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
                QVERIFY2(
                    repo.hasQuiz(m, l),
                    qPrintable(QString("module %1 lesson %2 has no valid quiz").arg(m).arg(l)));
            }
        }
    }

    void appContentLessonIdsAreUniqueAndSet()
    {
        const ContentRepository repo(QFINDTESTDATA("../content"));
        QSet<QString> seen;
        const QList<QStringList> ids = repo.lessonIdsPerModule();
        for (const QStringList &moduleIds : ids) {
            for (const QString &id : moduleIds) {
                QVERIFY2(!id.isEmpty(), "a lesson has no id");
                QVERIFY2(!seen.contains(id), qPrintable(QString("duplicate lesson id %1").arg(id)));
                seen.insert(id);
            }
        }
    }

private:
    static QString fixtureRoot() { return QFINDTESTDATA("data/content"); }
};

QTEST_APPLESS_MAIN(TestContentRepository)
#include "tst_contentrepository.moc"
