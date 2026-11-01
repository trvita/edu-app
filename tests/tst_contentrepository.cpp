#include <QtTest>

#include "ContentRepository.h"

class TestContentRepository : public QObject
{
    Q_OBJECT

private slots:
    void hasThreeModules()
    {
        const ContentRepository repo;
        QCOMPARE(repo.moduleTitles().size(), 3);
    }

    void everyModuleHasLessons()
    {
        const ContentRepository repo;
        for (int m = 0; m < repo.moduleTitles().size(); ++m) {
            QVERIFY(!repo.lessonTitles(m).isEmpty());
        }
    }

    void hasNextLessonStopsAtLastLesson()
    {
        const ContentRepository repo;
        const auto last = static_cast<int>(repo.lessonTitles(0).size()) - 1;
        QVERIFY(repo.hasNextLesson(0, 0));
        QVERIFY(!repo.hasNextLesson(0, last));
    }

    void invalidIndicesReturnEmptyValues()
    {
        const ContentRepository repo;
        QVERIFY(repo.moduleTitle(-1).isEmpty());
        QVERIFY(repo.moduleTitle(99).isEmpty());
        QVERIFY(repo.lessonTitles(99).isEmpty());
        QVERIFY(repo.lessonText(0, 99).isEmpty());
        QVERIFY(!repo.hasNextLesson(99, 0));
    }
};

QTEST_APPLESS_MAIN(TestContentRepository)
#include "tst_contentrepository.moc"
