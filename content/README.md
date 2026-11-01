# Learning content

All lesson texts live here, not in code. The app reads this folder as-is.

## Structure

```
content/
├── manifest.json        list and order of modules and lessons
└── module-1/
    ├── lesson-1.md      lesson text
    ├── lesson-1.quiz.json  quiz after the lesson
    └── ...
```

## manifest.json

```json
{
  "formatVersion": 1,
  "modules": [
    {
      "id": "m1",
      "title": "Module 1",
      "lessons": [
        { "id": "m1-l1", "title": "Lesson 1", "file": "module-1/lesson-1.md" }
      ]
    }
  ]
}
```

- Modules and lessons appear in the app in the order listed here.
- `title` is shown in the lists and page headers.
- `file` is the lesson text, relative to this folder.
- `id` must be unique and must never change once published: learning progress will be
  stored by id, so renaming or reordering lessons keeps users' progress.

## Lesson files

Plain Markdown (UTF-8): paragraphs, `## headings`, `**bold**`, `*italic*`, lists, links.
Separate paragraphs with an empty line.

## Quiz files

Each lesson's quiz is a JSON file next to the lesson, linked from the manifest with
`"quiz": "module-1/lesson-1.quiz.json"`:

```json
{
  "questions": [
    {
      "type": "single",
      "text": "What is 2 + 2?",
      "options": [
        { "text": "3" },
        { "text": "4", "correct": true }
      ]
    },
    {
      "type": "multiple",
      "text": "Which of these numbers are even?",
      "options": [
        { "text": "1" },
        { "text": "2", "correct": true },
        { "text": "4", "correct": true }
      ]
    }
  ]
}
```

- `type`: `single` (exactly one correct option) or `multiple` (one or more correct options).
- Every question needs at least 2 options. Options are shown in the listed order.
- An invalid quiz is skipped (the lesson then has no quiz) and the unit tests fail.

Scoring: single choice gives 1 point for the correct option. Multiple choice gives +0.5 for
each selected correct option, -0.5 for each selected incorrect option and -0.5 for each missed
correct option; a question never goes below 0. The quiz total is rounded up.

## Adding a lesson

1. Create the `.md` file (and the `.quiz.json` file) in the module's folder.
2. Add an entry with a new unique `id` to that module's `lessons` in `manifest.json`.
3. Rebuild the app (content is bundled into it for now) and run the unit tests:
   they check that every lesson in the manifest loads.
