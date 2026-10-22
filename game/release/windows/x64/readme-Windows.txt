Cyberia for Windows (x64)
Development version

This is a static recompilation of the game's CYB.EXE (a Watcom program with
the DOS/4GW extender); it replaces CYB.EXE and DOS4GW.EXE, which are not
needed to run it.

The game needs its own data: the files of the disc (CYB.EXE, the .G93/.Z94
scenes and sounds, the .DEF and .C93 files) and an image of the disc for the
CD drive.  The port reads the data from the game's directory and the CD from
the image named by the CYB_CD_IMAGE environment variable (a .cue file); a
copy of the disc's files in the game's directory works without the image,
when the directory holds the _CD_ marker.

The port writes to the directory named by CYB_WRITE_ROOT (the game's
directory by default) and reads the settings from the environment:
CYB_DATA_DIR, CYB_CD_IMAGE, CYB_WRITE_ROOT, CYB_TRACE_INT, CYB_TRACE_CD,
CYB_TRACE_MOUSE, CYB_TRACE_SOUND, CYB_TRACE_TIMER.

Libraries
---------

The game requires following 64-bit libraries: SDL2, SDL2_mixer
On debian based distributions these libraries are in following packages: libsdl2-2.0-0 libsdl2-mixer-2.0-0

Status
------

The port starts, finds the disc, reads the data files of the game, sets the
video mode 13h, drives the Sound Blaster and runs the game's own timer; the
intro plays.  The interrupt of the sound card does not reach the game yet
(the handler it installs is 16-bit code, which the port does not run), so the
sound repeats one block.
