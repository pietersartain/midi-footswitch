;==============================================================================
; Cyan Technology Ltd
;
; FILE
;  cstartup.asm - Assembler startup for C programs.
;
; DESCRIPTION
;  Defines C segments. Contains initial code called before C is started
;==============================================================================

                  MODULE   cstartup
                  .ALL

;
; C reserves DATA address 0 for the NULL pointer. The value H'DEAD is put in
; here so that it is easier to spot the effect of a NULL pointer during
; debugging. User memory for constants grows upwards from H'0001. The first
; address is given the equate symbol $??LO_ADDR.
;
                  .SEG     C_RESERVED1
                  ORG      0
                  dc       H'DEAD
$??LO_ADDR        EQU      $

;
; DATA addresses H'EFE0-H'EFFF are used for scratchpad RAM in interrupt mode.
; DATA addresses H'EFC0-H'EFDF are used for scratchpad RAM in user mode.
; DATA addresses H'EFB8-H'EFBF are used for register storage in interrupt mode.
; Then follows the interrupt stack, user stack and user heap.
; User memory for variables grows downwards from the end of the user stack.
; This version of cstartup only contains one area of scratchpad RAM
; which constrains users not to write re-entrant re-interruptable
; code.
; The interrupt stack must start at IY-38 to be compatible with the C compiler.
;
                  .SEG     C_RESERVED2
                  ORG      H'EFB8

$??HI_ADDR        DEQU     $
                  ds       8           ; Interrupt register storage

                  ds       32          ; User Scratchpad
IY_SCRATCH        DEQU     $

$?irq_scratchpad? DEQU     $
                  ds       32          ; Interrupt Scratchpad

;
; The registers that control the functional blocks of the eCOG1 are located
; at addresses H'FEA0 to H'FFCF. The C header file <ecog1.h> declares an
; external structure that descibes the registers. This variable is defined
; below.
;
                  .SEG     REGISTERS
                  ORG      H'FEA0
$fd:
$rg:

;
; C requires the following segments:
;     CONST  - Constants in ROM. For example:
;                 const char c = 'c' ;
;                 printf( "Hello World!" ) ;
;     VAR    - Variables in RAM. These are set to zero by the cstartup code.
;              For example:
;                 int i ;              (in file scope)
;                 static int i ;       (in function scope)
;     INIT   - Initialisd variables in RAM. For example:
;                 int i = 9 ;          (in file scope)
;                 static int i = 9 ;   (in function scope)
;     INITC  - Initialisation data for the INIT segment
;     HEAP   - The heap. Required if malloc() etc. are used.
;     STACK  - The stack. Always required.
;
; The memory allocated to each segment is defined by the value of
; $??<segment_name>_SIZE as set below. These sizes can be set manually or, if
; the appropriate line is tagged with !PACK and the -pack option is specified
; to ECOGCL, ECOGCL will write in the size actually required for the segment.
; The sizes of the STACK and HEAP segments must be set by the user.
;
$??ISTACK_SIZE    =        H'0040
$??STACK_SIZE     =        H'0100
$??HEAP_SIZE      =        H'0080


; -- Locate DATA segments in memory --
;
; Segments are allocated sequentially by the ??ALLOCATE macro. They may be
; set at fixed addresses by setting ADDR prior to calling ??ALLOCATE.
;
??ALLOCATE        MACRO    seg
                  .SEG     &seg
                  ORG      ADDR
$??&seg!_LO       = ADDR
ADDR              = ADDR + $??&seg!_SIZE
$??&seg!_HI       = ADDR-1
                  ENDMAC

; Allocate DATA ROM
ADDR              = $??LO_ADDR
                  ??ALLOCATE INITC
                  ??ALLOCATE CONST


; Allocate DATA RAM
ADDR              = $??HI_ADDR
                  IFDEF $??VAR_SIZE
ADDR              = ADDR - $??VAR_SIZE - $??INIT_SIZE
                  ENDIF
ADDR              = ADDR - $??ISTACK_SIZE - $??STACK_SIZE - $??HEAP_SIZE
                  ??ALLOCATE INIT
                  ??ALLOCATE VAR
                  ??ALLOCATE HEAP
                  ??ALLOCATE STACK
                  ??ALLOCATE ISTACK

; -- Memory initialisation macros --
;
; Segments may be initialised by filling with a constant value using the
; ??SEGFILL macro.  Two symbols are passed, the segment name and the value to
; fill with.  A third symbol (the size) is assumed.
;
??SEGFILL         MACRO    seg, value
                  LOCAL    fill_loop
                  IF       $??&seg!_SIZE
                  ld       x, #$??&seg
                  ld       al, #$??&seg!_SIZE
                  ld       ah, &value
&fill_loop:       st       ah, @(0,x)
                  add      x, #1
                  sub      al, #1
                  bne      &fill_loop
                  ENDIF
                  ENDMAC

;
; Segments may be initialised by copying an initialisation segment with
; the ??SEGCOPY macro. Two symbols are passed, the source and destination
; segment names.
;
??SEGCOPY         MACRO    src, dest
                  IF       $??&src!_SIZE
                  ld       x, #$??&src
                  ld       y, #$??&dest
                  ld       al, #$??&src!_SIZE
                  bc
                  ENDIF
                  ENDMAC

;
; Fills a block of memory with a value. Three values are passed, the start
; address for the block, the number of addresses to write to and the value
; to be written.
;
??MEMFILL         MACRO    start, length, value
                  LOCAL    fill_loop
                  ld       x, &start
                  ld       al, &length
                  ld       ah, &value
&fill_loop:       st       ah, @(0,x)
                  add      x, #1
                  sub      al, #1
                  bne      &fill_loop
                  ENDMAC

;
; Input argument for main().
;
                  .SEG     CONST
argv              dc       0,0         ; NULL as two-word byte address 


;
; Start of Code.
;
                  .CODE
                  ORG      H'40

$?cstart_code:
		bra $ecog1ConfigMMU	; configure MMU and Cache Banks

$ecog1ConfigContinue:
;
; Initialise segments. The HEAP and STACK are filled with H'9999 and H'aaaa
; respectively so that their maximum runtime extents can be checked. The
; INIT segment is set from the ROM initialisers in the INITC segment. The non
; initialised RAM segment VAR is set to zero (compiler puts 0 initialised
; variables in these segments as well as uninitialised ones,x).
;
                  ??SEGFILL HEAP, #h'9999
                  ??SEGFILL STACK, #h'AAAA
                  ??SEGFILL ISTACK, #h'BBBB
                  ??SEGCOPY INITC, INIT
                  ??SEGFILL VAR, #h'0

; Set interrupt stack pointer.
                  ld y, #IY_SCRATCH

; Set user mode flag to allow interupts.
                  st flags, @(-1,y)
                  ld al, @(-1,y)
                  or al, #h'10
                  st al, @(-1,y)
                  ld flags, @(-1,y)

; Set usermode stack pointer
                  ld y, #$??STACK_HI

; Call ecog1Config to setup eCOG1 peripherals
; Defined in module produced by configuration compiler
                  bsr $ecog1Config
                
; Call main, setting argc and argv[0] to 0.
                  ld ah, #argv
                  ld al, #0
                  bsr $main

;
; Main may exit by returning or by explicitly calling $exit. In either case
; exit code will be in AL.
;
$exit:
				  brk				; Alert the user if in debug mode
                  bra 0             ; Restart


;
; This is the minimal interrupt routine. The contents of FLAGS is restored
; as the program counter is restored using rti.
;
$minimal_handler:
                  st flags,@(-33,y)    ; Store Flags
                  st al, @(-34,y)      ; Store AL

                  ld al, @(-33,y)      ; Put Flags into AL
                  or al, #h'0010       ; Set usermode
                  st al, @(-33,y)      ; Store the value to be restored to Flags

				  brk				; Alert the user if in debug mode

                  ld al, @(-34,y)      ; Restore AL
                  rti @(-33,y)         ; Restore PC and Flags

;
; The address exception can happen often during development. A handler
; is put here to catch the exception.
;
$address_error:
        		st flags,@(-33,y)			; Store Flags
				st al, @(-34,y)	; Store AL

				ld al, @(-33,y)	; Put Flags into AL
				or al, #h'0010	; Set usermode
				st al, @(-33,y)	; Store the value to be restored to Flags

				brk				; Alert the user if in debug mode

				ld al, #h'a
				st al, @h'ff69	; Clear status in mmu.address_exception

				ld al, #h'200
				st al, @h'ff7a	; Clear status in emi.ctrl_sts

				ld al, @(-34,y)	; Restore AL
				rti @(-33,y)		; Restore PC and Flags
				
; End of startup code
$??CSTARTUP_END	  EQU      $

; Reserve storage for stacks
    .SEG HEAP
    DS $??HEAP_SIZE

    .SEG STACK
    DS $??STACK_SIZE

    .SEG ISTACK
    DS $??ISTACK_SIZE

                  ENDMOD
