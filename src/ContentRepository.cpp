#include "ContentRepository.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>

#include <optional>

namespace {

Q_LOGGING_CATEGORY(lcContent, "eduapp.content")

const QString kManifestFileName = QStringLiteral("manifest.json");

std::optional<QByteArray> readFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qCWarning(lcContent) << "Cannot open" << path << ":" << file.errorString();
        return std::nullopt;
    }
    return file.readAll();
}

// Quiz file format: see content/README.md. Returns an empty quiz if the file is invalid.
quiz::Quiz readQuiz(const QString &path)
{
    const std::optional<QByteArray> data = readFile(path);
    if (!data) {
        return {};
    }

    QJsonParseError parseError{};
    const QJsonDocument document = QJsonDocument::fromJson(*data, &parseError);
    if (!document.isObject()) {
        qCWarning(lcContent) << "Invalid quiz" << path << ":" << parseError.errorString();
        return {};
    }

    quiz::Quiz result;
    const QJsonArray questions = document.object().value(QStringLiteral("questions")).toArray();
    for (const auto &questionValue : questions) {
        const QJsonObject questionObject = questionValue.toObject();
        quiz::Question question;
        question.text = questionObject.value(QStringLiteral("text")).toString();
        question.type =
            questionObject.value(QStringLiteral("type")).toString() == QStringLiteral("multiple")
            ? quiz::QuestionType::Multiple
            : quiz::QuestionType::Single;

        const QJsonArray options = questionObject.value(QStringLiteral("options")).toArray();
        for (const auto &optionValue : options) {
            const QJsonObject optionObject = optionValue.toObject();
            if (optionObject.value(QStringLiteral("correct")).toBool()) {
                question.correctOptions.append(static_cast<int>(question.options.size()));
            }
            question.options.append(optionObject.value(QStringLiteral("text")).toString());
        }
        result.questions.append(question);
    }

    const QString error = quiz::validate(result);
    if (!error.isEmpty()) {
        qCWarning(lcContent) << "Invalid quiz" << path << ":" << error;
        return {};
    }
    return result;
}

QStringList optionTexts(const quiz::Question &question, const QList<int> &indices)
{
    QStringList texts;
    for (const int index : indices) {
        if (index >= 0 && index < question.options.size()) {
            texts << question.options[index];
        }
    }
    return texts;
}

} // namespace

ContentRepository::ContentRepository(QObject *parent)
    : ContentRepository(kBundledContentRoot, parent)
{ }

ContentRepository::ContentRepository(const QString &contentRoot, QObject *parent)
    : QObject(parent)
{
    load(contentRoot);
}

bool ContentRepository::load(const QString &contentRoot)
{
    m_modules.clear();

    const QDir root(contentRoot);
    const std::optional<QByteArray> manifestData = readFile(root.filePath(kManifestFileName));
    if (!manifestData) {
        return false;
    }

    QJsonParseError parseError{};
    const QJsonDocument manifest = QJsonDocument::fromJson(*manifestData, &parseError);
    if (!manifest.isObject()) {
        qCWarning(lcContent) << "Invalid" << kManifestFileName << "in" << contentRoot << ":"
                             << parseError.errorString();
        return false;
    }

    const QJsonArray modules = manifest.object().value(QStringLiteral("modules")).toArray();
    for (const auto &moduleValue : modules) {
        const QJsonObject moduleObject = moduleValue.toObject();
        Module module{moduleObject.value(QStringLiteral("title")).toString(), {}};

        const QJsonArray lessons = moduleObject.value(QStringLiteral("lessons")).toArray();
        for (const auto &lessonValue : lessons) {
            const QJsonObject lessonObject = lessonValue.toObject();
            const QString file = lessonObject.value(QStringLiteral("file")).toString();
            const QString text =
                QString::fromUtf8(readFile(root.filePath(file)).value_or(QByteArray()));
            const QString quizFile = lessonObject.value(QStringLiteral("quiz")).toString();
            module.lessons.append(
                {lessonObject.value(QStringLiteral("title")).toString(), text,
                 quizFile.isEmpty() ? quiz::Quiz() : readQuiz(root.filePath(quizFile))});
        }
        m_modules.append(module);
    }
    return true;
}

QStringList ContentRepository::moduleTitles() const
{
    QStringList titles;
    for (const Module &module : m_modules) {
        titles << module.title;
    }
    return titles;
}

QString ContentRepository::moduleTitle(int moduleIndex) const
{
    const Module *m = module(moduleIndex);
    return m ? m->title : QString();
}

QStringList ContentRepository::lessonTitles(int moduleIndex) const
{
    QStringList titles;
    if (const Module *m = module(moduleIndex)) {
        for (const Lesson &lesson : m->lessons) {
            titles << lesson.title;
        }
    }
    return titles;
}

QString ContentRepository::lessonTitle(int moduleIndex, int lessonIndex) const
{
    const Lesson *l = lesson(moduleIndex, lessonIndex);
    return l ? l->title : QString();
}

QString ContentRepository::lessonText(int moduleIndex, int lessonIndex) const
{
    const Lesson *l = lesson(moduleIndex, lessonIndex);
    return l ? l->text : QString();
}

bool ContentRepository::hasQuiz(int moduleIndex, int lessonIndex) const
{
    const Lesson *l = lesson(moduleIndex, lessonIndex);
    return l != nullptr && !l->quiz.questions.isEmpty();
}

QVariantList ContentRepository::quizQuestions(int moduleIndex, int lessonIndex) const
{
    QVariantList questions;
    if (const Lesson *l = lesson(moduleIndex, lessonIndex)) {
        for (const quiz::Question &question : l->quiz.questions) {
            questions.append(QVariantMap{
                {QStringLiteral("text"), question.text},
                {QStringLiteral("multiple"), question.type == quiz::QuestionType::Multiple},
                {QStringLiteral("options"), question.options},
            });
        }
    }
    return questions;
}

QVariantMap ContentRepository::evaluateQuiz(int moduleIndex, int lessonIndex,
                                            const QVariantList &answers) const
{
    const Lesson *l = lesson(moduleIndex, lessonIndex);
    if (!l) {
        return {};
    }

    QList<quiz::Answer> parsedAnswers;
    for (const QVariant &answer : answers) {
        quiz::Answer selected;
        const QVariantList options = answer.toList();
        for (const QVariant &option : options) {
            selected.append(option.toInt());
        }
        parsedAnswers.append(selected);
    }

    const quiz::Result result = quiz::evaluate(l->quiz, parsedAnswers);

    QVariantList mistakes;
    for (const int index : result.incorrectQuestions) {
        const quiz::Question &question = l->quiz.questions[index];
        mistakes.append(QVariantMap{
            {QStringLiteral("question"), question.text},
            {QStringLiteral("yourAnswers"), optionTexts(question, parsedAnswers.value(index))},
            {QStringLiteral("correctAnswers"), optionTexts(question, question.correctOptions)},
        });
    }

    return {
        {QStringLiteral("score"), result.score()},
        {QStringLiteral("maxScore"), result.maxScore()},
        {QStringLiteral("points"), result.halfPoints / 2.0},
        {QStringLiteral("maxPoints"), result.maxHalfPoints / 2.0},
        {QStringLiteral("percent"), result.percent()},
        {QStringLiteral("mistakes"), mistakes},
    };
}

bool ContentRepository::hasNextLesson(int moduleIndex, int lessonIndex) const
{
    return lesson(moduleIndex, lessonIndex + 1) != nullptr;
}

const ContentRepository::Module *ContentRepository::module(int moduleIndex) const
{
    if (moduleIndex < 0 || moduleIndex >= m_modules.size()) {
        return nullptr;
    }
    return &m_modules[moduleIndex];
}

const ContentRepository::Lesson *ContentRepository::lesson(int moduleIndex, int lessonIndex) const
{
    const Module *m = module(moduleIndex);
    if (!m || lessonIndex < 0 || lessonIndex >= m->lessons.size()) {
        return nullptr;
    }
    return &m->lessons[lessonIndex];
}
