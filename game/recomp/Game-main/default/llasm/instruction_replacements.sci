; The instruction replacements of Cyberia: the instructions of the original
; (CYB.EXE) that the port replaces with its own.
;
; Two fixes of V1.87 (CYB.EXE of 1994-12-08: the update of the publisher "for the
; problems with .log files"; both releases on disc are V1.86).  No comment line
; here may hold two commas: SR would take it for a replacement.
;
; The password of loc_2AB90 (compared with [loc_42604]: EINSTEIN) is typed into
; [ebp-0x24] but its terminating zero went to [eax+loc_457AC]: into the name of
; the player.  The name was cut to the length of the password and the log of the
; saved games (<name>.log of loc_2A900 and loc_2AA13) was another file from then
; on.  V1.87 ends the password instead.
loc_2AD38,7,;mov byte [eax+loc_457AC], 0x0: the end of the password and not of the name (V1.87)|add tmpadr, ebp, -36|add tmpadr, tmpadr, eax|store8 0, tmpadr, 1
; When the arcade sequence 1 (loc_1DCC4 with 1) is won the state of the scene was
; not reset (loc_109C0) as after the other sequences: V1.87 calls it.  It returns
; to loc_122FA: a jmp loc_126F1 as the jmp short loc_1232B that it replaces leads to.
loc_12317,11,;mov word [loc_40032], 0x0; jmp short loc_1232B (to jmp loc_126F1): and reset the scene (V1.87)|store16 0, loc_40032, 2|ACTION_CALL|PUSH loc_122FA|tcall loc_109C0
