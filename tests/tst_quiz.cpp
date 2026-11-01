#include <QtTest>

#include "Quiz.h"

using namespace quiz;

namespace {

// Options A, B, C; B is correct.
Question singleQuestion()
{
    return {QStringLiteral("single"), QuestionType::Single, {"A", "B", "C"}, {1}};
}

// Options A..E; A, B, C are correct (max 1.5 points).
Question multipleQuestion()
{
    return {
        QStringLiteral("multiple"), QuestionType::Multiple, {"A", "B", "C", "D", "E"}, {0, 1, 2}};
}

} // namespace

class TestQuiz : public QObject
{
    Q_OBJECT

private slots:
    void singleChoice_data()
    {
        QTest::addColumn<Answer>("answer");
        QTest::addColumn<int>("halfPoints");

        QTest::newRow("correct") << Answer{1} << 2;
        QTest::newRow("incorrect") << Answer{0} << 0;
        QTest::newRow("unanswered") << Answer{} << 0;
        QTest::newRow("two options selected") << Answer{0, 1} << 0;
        QTest::newRow("out of range") << Answer{7} << 0;
    }

    void singleChoice()
    {
        QFETCH(Answer, answer);
        QFETCH(int, halfPoints);
        QCOMPARE(questionHalfPoints(singleQuestion(), answer), halfPoints);
    }

    void multipleChoice_data()
    {
        QTest::addColumn<Answer>("answer");
        QTest::addColumn<int>("halfPoints");

        QTest::newRow("all correct") << Answer{0, 1, 2} << 3;
        QTest::newRow("one correct missed") << Answer{0, 1} << 1;
        QTest::newRow("all correct + one incorrect") << Answer{0, 1, 2, 3} << 2;
        QTest::newRow("all options") << Answer{0, 1, 2, 3, 4} << 1;
        QTest::newRow("one correct only") << Answer{0} << 0;
        QTest::newRow("below zero is zero") << Answer{3, 4} << 0;
        QTest::newRow("unanswered") << Answer{} << 0;
        QTest::newRow("order and duplicates ignored") << Answer{2, 0, 1, 0} << 3;
    }

    void multipleChoice()
    {
        QFETCH(Answer, answer);
        QFETCH(int, halfPoints);
        QCOMPARE(questionHalfPoints(multipleQuestion(), answer), halfPoints);
    }

    void maxPoints()
    {
        QCOMPARE(questionMaxHalfPoints(singleQuestion()), 2);
        QCOMPARE(questionMaxHalfPoints(multipleQuestion()), 3);
    }

    void totalIsRoundedUp_data()
    {
        QTest::addColumn<int>("halfPoints");
        QTest::addColumn<int>("score");

        QTest::newRow("0") << 0 << 0;
        QTest::newRow("0.5") << 1 << 1;
        QTest::newRow("1") << 2 << 1;
        QTest::newRow("1.5") << 3 << 2;
        QTest::newRow("2.5") << 5 << 3;
    }

    void totalIsRoundedUp()
    {
        QFETCH(int, halfPoints);
        QFETCH(int, score);
        Result result;
        result.halfPoints = halfPoints;
        QCOMPARE(result.score(), score);
    }

    void percentOfMaxPoints_data()
    {
        QTest::addColumn<int>("halfPoints");
        QTest::addColumn<int>("maxHalfPoints");
        QTest::addColumn<int>("percent");

        QTest::newRow("0 of 1") << 0 << 2 << 0;
        QTest::newRow("1 of 1") << 2 << 2 << 100;
        QTest::newRow("1.5 of 2 (not rounded up first)") << 3 << 4 << 75;
        QTest::newRow("2 of 3") << 4 << 6 << 67;
        QTest::newRow("1 of 3") << 2 << 6 << 33;
        QTest::newRow("0.5 of 1.5") << 1 << 3 << 33;
        QTest::newRow("1 of 1.5") << 2 << 3 << 67;
        QTest::newRow("empty quiz") << 0 << 0 << 0;
    }

    void percentOfMaxPoints()
    {
        QFETCH(int, halfPoints);
        QFETCH(int, maxHalfPoints);
        QFETCH(int, percent);
        Result result;
        result.halfPoints = halfPoints;
        result.maxHalfPoints = maxHalfPoints;
        QCOMPARE(result.percent(), percent);
    }

    void passThreshold_data()
    {
        QTest::addColumn<int>("halfPoints");
        QTest::addColumn<int>("maxHalfPoints");
        QTest::addColumn<bool>("passed");

        QTest::newRow("2 of 3 (66.7% shown as 67%) passes") << 4 << 6 << true;
        QTest::newRow("1 of 1.5 (66.7%) passes") << 2 << 3 << true;
        QTest::newRow("1.5 of 2.5 (60%) fails") << 3 << 5 << false;
        QTest::newRow("all correct passes") << 6 << 6 << true;
        QTest::newRow("nothing correct fails") << 0 << 6 << false;
    }

    void passThreshold()
    {
        QFETCH(int, halfPoints);
        QFETCH(int, maxHalfPoints);
        QFETCH(bool, passed);
        Result result;
        result.halfPoints = halfPoints;
        result.maxHalfPoints = maxHalfPoints;
        QCOMPARE(result.passed(), passed);
    }

    void evaluateSumsQuestionsAndListsMistakes()
    {
        const Quiz quiz{{singleQuestion(), multipleQuestion(), singleQuestion()}};
        // single correct (1) + multiple with one missed (0.5) + single wrong (0) = 1.5 -> 2
        const Result result = evaluate(quiz, {{1}, {0, 1}, {2}});
        QCOMPARE(result.halfPoints, 3);
        QCOMPARE(result.maxHalfPoints, 7);
        QCOMPARE(result.score(), 2);
        QCOMPARE(result.maxScore(), 4);
        QCOMPARE(result.incorrectQuestions, QList<int>({1, 2}));
    }

    void evaluateTreatsMissingAnswersAsUnanswered()
    {
        const Quiz quiz{{singleQuestion(), singleQuestion()}};
        const Result result = evaluate(quiz, {{1}});
        QCOMPARE(result.score(), 1);
        QCOMPARE(result.incorrectQuestions, QList<int>({1}));
    }

    void validate_data()
    {
        QTest::addColumn<Quiz>("quiz");
        QTest::addColumn<bool>("valid");

        Question twoCorrectSingle = singleQuestion();
        twoCorrectSingle.correctOptions = {0, 1};
        Question noCorrectMultiple = multipleQuestion();
        noCorrectMultiple.correctOptions = {};
        Question oneOption = singleQuestion();
        oneOption.options = {"A"};
        oneOption.correctOptions = {0};

        QTest::newRow("valid") << Quiz{{singleQuestion(), multipleQuestion()}} << true;
        QTest::newRow("no questions") << Quiz{} << false;
        QTest::newRow("single with two correct") << Quiz{{twoCorrectSingle}} << false;
        QTest::newRow("multiple without correct") << Quiz{{noCorrectMultiple}} << false;
        QTest::newRow("one option") << Quiz{{oneOption}} << false;
    }

    void validate()
    {
        QFETCH(Quiz, quiz);
        QFETCH(bool, valid);
        QCOMPARE(quiz::validate(quiz).isEmpty(), valid);
    }
};

QTEST_APPLESS_MAIN(TestQuiz)
#include "tst_quiz.moc"
