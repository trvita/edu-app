#include "ContentRepository.h"

namespace {

constexpr int kModuleCount = 3;
constexpr int kLessonsPerModule = 3;

QString placeholderText(int module, int lesson)
{
    const QString paragraph =
        QStringLiteral("This is the text of lesson %1 in module %2. "
                       "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod "
                       "tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim "
                       "veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea "
                       "commodo consequat.")
            .arg(lesson)
            .arg(module);

    QStringList paragraphs;
    for (int i = 0; i < 8; ++i)
        paragraphs << paragraph;
    return paragraphs.join(QStringLiteral("\n\n"));
}

} // namespace

ContentRepository::ContentRepository(QObject *parent)
    : QObject(parent)
{
    for (int m = 1; m <= kModuleCount; ++m) {
        Module module{tr("Module %1").arg(m), {}};
        for (int l = 1; l <= kLessonsPerModule; ++l)
            module.lessons.append({tr("Lesson %1").arg(l), placeholderText(m, l)});
        m_modules.append(module);
    }
}

QStringList ContentRepository::moduleTitles() const
{
    QStringList titles;
    for (const Module &module : m_modules)
        titles << module.title;
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
        for (const Lesson &lesson : m->lessons)
            titles << lesson.title;
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
    if (moduleIndex < 0 || moduleIndex >= m_modules.size())
        return nullptr;
    return &m_modules[moduleIndex];
}

const ContentRepository::Lesson *ContentRepository::lesson(int moduleIndex, int lessonIndex) const
{
    const Module *m = module(moduleIndex);
    if (!m || lessonIndex < 0 || lessonIndex >= m->lessons.size())
        return nullptr;
    return &m->lessons[lessonIndex];
}
