#pragma once

#include <QList>
#include <QString>
#include <QStringList>

#include <cstdint>

// Quiz content and scoring rules (Q-QUIZ-02):
// - single choice: correct answer +1, otherwise 0;
// - multiple choice: per option, selected correct +0.5, selected incorrect -0.5,
//   missed correct -0.5, unselected incorrect 0; a question never goes below 0;
// - the quiz total is rounded up.
// Points are counted in halves so the arithmetic stays exact.
// A quiz is passed at 67% or more of the rounded percentage, so 2 of 3 passes (Q-EDU-01/02).
namespace quiz {

constexpr int kPassPercent = 67;

enum class QuestionType : std::uint8_t { Single, Multiple };

struct Question
{
    QString text;
    QuestionType type = QuestionType::Single;
    QStringList options;
    QList<int> correctOptions; // option indices
};

struct Quiz
{
    QList<Question> questions;
};

// Selected option indices for one question.
using Answer = QList<int>;

struct Result
{
    int halfPoints = 0;
    int maxHalfPoints = 0;
    QList<int> incorrectQuestions; // indices of questions without full points

    int score() const; // points rounded up
    int maxScore() const; // maximum points rounded up
    int percent() const; // points (before rounding) as % of the maximum, rounded to nearest
    bool passed() const; // percent() >= kPassPercent
};

int questionHalfPoints(const Question &question, const Answer &answer);
int questionMaxHalfPoints(const Question &question);

// answers[i] belongs to questions[i]; missing answers count as unanswered.
Result evaluate(const Quiz &quiz, const QList<Answer> &answers);

// Content rules for authors. Returns an error description, or an empty string if valid.
QString validate(const Quiz &quiz);

} // namespace quiz
