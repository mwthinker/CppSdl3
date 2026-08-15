---
name: update-imgui
description: Update the pinned Dear ImGui release in ImGui/CMakeLists.txt. Use when checking for or applying an ImGui version update; only released numeric versions with a matching docking tag are allowed.
---

# Update ImGui

Update only `ImGui/CMakeLists.txt`.

1. Read its current numeric `VERSION`, release comment, and `GIT_TAG`. The comment is the full release version when a suffix is present.
2. Query official `ocornut/imgui` GitHub releases and tags. Never use `master`, `docking`, or an untagged commit.
3. Accept only published, non-prerelease tags matching `vMAJOR.MINOR.PATCH` or `vMAJOR.MINOR.PATCH<lowercase-letter>`, with a matching `-docking` tag. Ignore WIP and other suffixes.
4. Compare versions by numeric components, then suffix order: no suffix, `a`, `b`, `c`, etc. If no eligible release is newer, make no edits and report that no update is available.
5. For the newest eligible release, set `VERSION` to its numeric part only and add/update the same-line comment as `# FULL_VERSION docking branch`; set `GIT_TAG` to the matching docking tag's full 40-character commit SHA.
6. Verify the SHA and release/tag metadata, then review `git diff -- ImGui/CMakeLists.txt`. Preserve unrelated worktree changes.

Do not change anything else or create a commit unless explicitly requested. Report the old/new versions, docking tag, and SHA.
