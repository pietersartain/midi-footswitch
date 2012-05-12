                  MODULE   pack

; The pack information below used to reside in cstartup.asm.  This meant
; that the toolchain linked qqpack.asm rather than cstartup.asm.  This
; raises a number of issues when debugging cstartup.asm.  By placing the
; pack information in this file, which does not contain any code, these
; issues are resolved.

; ROM segments
$??INITC_SIZE     =        h'0000  ; !PACK
$??CONST_SIZE     =        h'0000  ; !PACK
        
; RAM segments
$??INIT_SIZE      =        h'0000  ; !PACK
$??VAR_SIZE       =        h'0000  ; !PACK

                  ENDMOD
