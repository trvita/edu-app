#include "Quiz.h"

#include <QSet>

#include <algorithm>

namespace quiz {

namespace {

constexpr int kHalvesPerPoint = 2;

int roundUpHalves(int halfPoints)
{
    return (halfPoints + kHalvesPerPoint - 1) / kHalvesPerPoint;
}

} // namespace

int Result::score() const
{
    return roundUpHalves(halfPoints);
}

int Result::maxScore() const
{
    return roundUpHalves(maxHalfPoints);
}

int Result::percent() const
{
    if (maxHalfPoints <= 0) {
        return 0;
    }
    // round(100 * halfPoints / maxHalfPoints) in integers, halves rounded up
    return (200 * halfPoints + maxHalfPoints) / (2 * maxHalfPoints);
}

bool Result::passed() const
{
    return percent() >= kPassPercent;
}

int questionHalfPoints(const Question &question, const Answer &answer)
{
    const QSet<int> selected(answer.cbegin(), answer.cend());

    if (question.type == QuestionType::Single) {
        const bool isCorrect =
            selected.size() == 1 && question.correctOptions.contains(*selected.cbegin());
        return isCorrect ? kHalvesPerPoint : 0;
    }

    int halves = 0;
    for (int option = 0; option < question.options.size(); ++option) {
        const bool isCorrect = question.correctOptions.contains(option);
        const bool isSelected = selected.contains(option);
        if (isCorrect && isSelected) {
            ++halves;
        } else if (isCorrect != isSelected) {
            --halves;
        }
    }
    return std::max(0, halves);
}

int questionMaxHalfPoints(const Question &question)
{
    return question.type == QuestionType::Single ? kHalvesPerPoint
                                                 : static_cast<int>(question.correctOptions.size());
}

Result evaluate(const Quiz &quiz, const QList<Answer> &answers)
{
    Result result;
    for (int i = 0; i < quiz.questions.size(); ++i) {
        const Question &question = quiz.questions[i];
        const int halves = questionHalfPoints(question, answers.value(i));
        const int maxHalves = questionMaxHalfPoints(question);
        result.halfPoints += halves;
        result.maxHalfPoints += maxHalves;
        if (halves < maxHalves) {
            result.incorrectQuestions.append(i);
        }
    }
    return result;
}

QString validate(const Quiz &quiz)
{
    if (quiz.questions.isEmpty()) {
        return QStringLiteral("quiz has no questions");
    }
    for (int i = 0; i < quiz.questions.size(); ++i) {
        const Question &question = quiz.questions[i];
        const QString where = QStringLiteral("question %1: ").arg(i + 1);
        if (question.text.isEmpty()) {
            return where + QStringLiteral("empty text");
        }
        if (question.options.size() < 2) {
            return where + QStringLiteral("needs at least 2 options");
        }
        if (question.type == QuestionType::Single && question.correctOptions.size() != 1) {
            return where + QStringLiteral("single choice needs exactly 1 correct option");
        }
        if (question.type == QuestionType::Multiple && question.correctOptions.isEmpty()) {
            return where + QStringLiteral("multiple choice needs at least 1 correct option");
        }
    }
    return {};
}

} // namespace quiz
