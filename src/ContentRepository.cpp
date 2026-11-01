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
            module.lessons.append({lessonObject.value(QStringLiteral("title")).toString(), text});
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
