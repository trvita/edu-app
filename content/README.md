# Learning content

All lesson texts live here, not in code. The app reads this folder as-is.

## Structure

```
content/
├── manifest.json        list and order of modules and lessons
└── module-1/
    ├── lesson-1.md      lesson text
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

## Adding a lesson

1. Create the `.md` file in the module's folder.
2. Add an entry with a new unique `id` to that module's `lessons` in `manifest.json`.
3. Rebuild the app (content is bundled into it for now) and run the unit tests:
   they check that every lesson in the manifest loads.
