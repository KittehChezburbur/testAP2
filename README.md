# autoplay

all 115 114 113 get top 1 on every leaderboard never misses a single block

- **id:** `auto-play`
- **version:** `0.1.0`
- **target Beat Saber:** `1.40.8`
- **author:** KittehChezBurbur

## Behavior (generated hooks)

- **Autoplay every note** — Hooks NoteController to auto-cut every note with a perfect 115 swing. Bombs ignored. Works on any map.

## ⚠️ This is source — it must be compiled into a `.so`

A `.qmod` runs compiled **ARM64 `.so`** native code. That binary must be built
with the Android NDK + qpm toolchain — it cannot be produced in a browser, and
unzipping this folder does **not** compile it.

## ⭐ Easiest: build in the cloud (works on a Chromebook!)

No NDK, no qpm, no PC required — everything builds on GitHub's servers:

1. Create a free **GitHub account** at github.com.
2. Make a **new repository** and upload every file from this zip (drag-and-drop
   in the GitHub web UI works fine on a Chromebook).
3. GitHub automatically runs `.github/workflows/build.yml`.
4. Open the **Actions** tab → click the latest run → download the
   **auto-play-qmod** artifact. Your compiled `.qmod` is inside.
5. Upload that `.qmod` in **MBF** (modsbeforefriday.com). Done. ✅

Whole flow on a Chromebook: upload files → wait a few minutes → download `.qmod`.

## Does qpm work on a Chromebook?

Not easily — qpm + the Android NDK need a full Linux (Crostini) setup and several
GB of space, and it's fiddly. **Use the GitHub Actions cloud build above instead.**

## Alternative: build locally (PC with NDK)

```bash
qpm restore        # fetch beatsaber-hook, bs-cordl, codegen, scotland2...
pwsh ./build.ps1   # -> build/libauto-play.so
qpm qmod build     # -> auto-play.qmod
```

…or drop `libauto-play.so` into the **QMOD Generator** page, set the target to
`1.40.8`, assign it to the right slot, and download the `.qmod`.

## Notes on the generated code

Lines marked `TODO (version-specific)` use il2cpp field/method names that drift
slightly between Beat Saber versions. After `qpm restore`, open the matching
headers in `extern/includes/bs-cordl/GlobalNamespace/` and confirm the names
against your cordl version. Everything else compiles as-is.
