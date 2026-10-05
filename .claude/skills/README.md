# Project Skills

Claude Code skills scoped to this repository. Each subfolder holds one skill
(`SKILL.md` + optional `references/`), loaded automatically at session start.

## Third-party skills

The ten `ue-*` skills come from [quodsoler/unreal-engine-skills](https://github.com/quodsoler/unreal-engine-skills)
(MIT — see `LICENSE-unreal-engine-skills`). The upstream repo ships 27; only the
ten relevant to this project were vendored:

| Skill | Covers |
|---|---|
| `ue-cpp-foundations` | UE macros, containers, delegates, GC, subsystems |
| `ue-data-assets-tables` | DataAsset, DataTable, soft refs, async loading |
| `ue-module-build-system` | Build.cs, .uplugin, module types, build errors |
| `ue-editor-tools` | Asset editors, detail customizations, UToolMenus |
| `ue-actor-component-architecture` | Actor/component lifecycle, spawning, interfaces |
| `ue-serialization-savegames` | USaveGame, FArchive, versioning, config |
| `ue-ai-navigation` | AIController, behavior trees, NavMesh, EQS |
| `ue-ui-umg-slate` | UMG, Slate, CommonUI, MVVM |
| `ue-testing-debugging` | Automation tests, logging, assertions, profiling |
| `ue-async-threading` | UE::Tasks, FRunnable, ParallelFor, thread safety |

### Local modifications

Vendored content was changed in two places; re-check these when syncing upstream:

1. **Project-context pointer** — upstream instructs the agent to read
   `.agents/ue-project-context.md`, which does not exist here. All ten skills now
   point at `CLAUDE.md` instead.
2. **IWYU flag (UE 5.5)** — upstream uses `bEnforceIWYU = true`, deprecated since
   UE 5.2. Updated to `IWYUSupport = IWYUSupport.Full` in
   `ue-module-build-system` (SKILL.md and both reference files).

Upstream targets UE5 generically, not 5.5 specifically. Treat version-sensitive
API details as a starting point, not gospel — verify against the engine source in
the local installation before relying on an exact signature.
