# Cyberia — a native port by static recompilation

A native port of the DOS game *Cyberia* to Linux, Windows and Android. The original
`CYB.EXE` is statically recompiled with [SR](https://github.com/M-HT/SR): every instruction
of the game becomes native code, and the game runs its own logic as it did in DOS. What DOS,
DOS/4GW and the PC hardware gave it comes from the port's runtime,
[SR-SDK](SR-SDK/README.md), on SDL2.

This port is almost all SR-SDK: the game brings its description (`game/game.cfg`,
`game/Game_hooks.h`, `game/distrib.cfg`), SR's inputs, and one small mod
(`game/CYB-mods.c`).

No game data is included. The build and the game need your own copy of the game's CD.

## The original

| | |
|---|---|
| Game | *Cyberia*, an action adventure with pre-rendered 3D scenes and flight sequences |
| Developer | Xatrix Entertainment |
| Publisher | Interplay Productions |
| Year | 1994 |
| Platform | MS-DOS, CD-ROM; the port builds from the Russian or the English release's disc |
| Executable | `CYB.EXE`, a 32-bit LE program: Watcom C/C++32, the DOS/4GW extender |
| Graphics | VGA mode 13h, 320×200 in 256 colours; scenes and movies streamed from the CD |
| Sound | the game's own Sound Blaster driver (DMA, with its interrupt) |
| Input | keyboard, Microsoft mouse (INT 33h), joystick |

## How the port works

- **Recompilation.** SR translates `CYB.EXE` into llasm, an intermediate language.
  SR-SDK lowers it to LLVM IR, and LLVM compiles it for x86-64 and arm64. The port starts
  at the program's own entry, through the extender's start code; the 16-bit DOS/4GW stub
  is not needed.
- **DOS runtime** (SR-SDK `runtime/dos`): the PSP and the environment (`BLASTER`), the
  DOS file and memory services, the PIT timer with the INT 8 → INT 1Ch chain, the
  keyboard, the mouse, VGA mode 13h, and the CD drive (MSCDEX), from a disc image or from a
  copy of the disc.
- **Sound.** SR-SDK emulates a Sound Blaster 2.0 (`Game_dossb.c`): the game's own driver
  programs the card and its DMA, and the port delivers the card's interrupt to the
  handler the game installed.

## What works

- The game plays: the intro, the scenes, the sound, the mouse.
- Linux (x86-64, arm64), Windows (x86-64; it also runs under Wine) and Android (arm64),
  where on-screen buttons stand in for the keyboard.
- The port's panel (F12): the size of the picture, the window or the full screen, the
  frame rate, the gamepad.
- Gamepads, through SDL's game controller support.

## Mods

Off by default, so the game plays as the original did. Each mod is a key of
`cyberia.cfg` and an item of the port's panel.

| Key | What it does |
|---|---|
| `Uninvert_Crosshair=on` | The crosshair follows the mouse (or a touch) up and down, not mirrored. The original starts with the vertical axis of the crosshair inverted, and its F key toggles that. |
| `Gyro_Mouse=on` | On a phone or a tablet with a gyroscope: turning the device moves the mouse, so it aims the crosshair. A touch still puts the mouse under the finger and clicks, and the turns go on from there. The on-screen FIRE is then the mouse's button, not Space: the game stops reading the mouse while Space or an arrow is held. |
| `Gyro_Mouse_Speed=1..9` | How far a turn moves the mouse: 5 (the default) crosses the screen in a turn of about 38 degrees, each step 0.3 of a screen's width a radian. |

## Building

Requirements: Python 3, SCons, GCC or Clang, LLVM's `opt` and `llc`, SDL2 and SDL2_mixer;
MinGW-w64 for Windows; the Android SDK and NDK for Android (see
[SR-SDK's README](SR-SDK/README.md#requirements)).

The recompiler reads `CYB.EXE` from the CD, so the image is needed for the build as well.
Put it where `image=` of [`game/distrib.cfg`](game/distrib.cfg) points, beside the
repository: `../iso/CYBERIA_RUS.iso`. The build checks the executable against the SHA-256s
in `game/recomp/`.

Two releases of the game build, each from its own disc: the Russian one (the default) and
the English one. Both have `CYB.EXE` V1.86, the same program but for the texts, so they
share SR's inputs. The English build is `make <target> image=<the English disc image>`
(say `make linux image=../iso/Cyberia.bin`, a `.bin` with its `.cue` beside it); the build
keeps that choice until `image=default`.

The port has the two fixes of the publisher's V1.87 update (December 1994) built in, for
both releases (`game/recomp/Game-main/default/llasm/instruction_replacements.sci`):

- the password that is typed in the game no longer cuts the player's name, so the log of
  the saved games (`<name>.log`) stays the same file;
- after the first arcade sequence the scene is reset as after the others.

```sh
git clone --recursive <this repository>
cd cyberia-recomp
make linux      # build/bin/x64-linux/cyberia-x64
make windows    # MinGW-w64 cross build, with the SDL DLLs
make apk        # Android, arm64
make distrib    # the release archives, in build/distrib/
```

## Running

```sh
./run-sdl.sh            # the Linux build
./run-sdl-wine.sh       # the Windows build under Wine
./run-orig-dosbox.sh    # the original under DOSBox, for comparison
```

The launchers run the game on `build/distrib/data`: the game's install (`INSTCYB.EXE`
copies a few files to `C:\CYBERIA` and reads the rest from the disc) with the rest of the
disc beside it, so the directory is also the disc. `make distrib-data` makes it from the
image. The port writes into `build/run`.

## License

The code of this repository is under the MIT license, as its file headers say. SR-SDK
and SR are under the MIT license as well: see `SR-SDK/LICENSE` and `SR-SDK/NOTICE.md`.
SR is by Roman Pauer; this port stands on it.

*Cyberia* itself, its executable and its data belong to their owners and are not part of
this repository.
