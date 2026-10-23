; The instruction replacements of Cyberia: the instructions of the original
; (CYB.EXE) that the port replaces with its own.
;
; The getenv of the extender (4GW) reads the array of the addresses of the
; variables of the program from its own variable in the data segment
; (mov esi,[0x1d20] at 0x2FBA1).  The array was built by the server of the
; extender, which is not in the port: every INT 31h of the game is the
; runtime's.  The runtime builds the array instead (Game_DosEnviron, from the
; environment block it makes behind the PSP) and the code reads it.

loc_2FBA1,6,;mov esi, [loc_41D20]|load esi, Game_DosEnviron, 4
