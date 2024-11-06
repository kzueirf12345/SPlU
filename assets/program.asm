; ; ===============FACTORIAL===============
; ; R2 - число
; ; R1 - результат

; ; MAIN
;     IN
;     POP R2
;     PUSH 1
;     POP R1

;     CALL :factorial
;     PUSH R1
;     OUT

;     PUSH 48
;     POP [0]
;     DRAW
; HLT


; :factorial
;     PUSH R2
;     PUSH 1
;     JLE :ret_factorial

;     PUSH R1
;     PUSH R2
;     MUL
;     POP R1

;     PUSH R2
;     PUSH 1
;     SUB
;     POP R2
; CALL :factorial

; :ret_factorial
;     RET


; ; ================CIRCLE================
; ; R1 - radius
; ; R2 - width
; ; R3 - height

; ;MAIN
;     IN
;     POP R1
;     IN
;     POP R2
;     IN
;     POP R3

;     CALL :print_circle
; HLT


; :print_circle
;     ; R4 - counter out
;     PUSH 0
;     POP R4
;     :start_circle
;     PUSH R4
;     PUSH 1
;     ADD
;     POP R4
;     ;
;     PUSH R4
;     PUSH R3
;     PUSH R2
;     MUL
;     JG :end_circle
;     ;
;         ; PUSH R4
;         ; OUT

;         ; X
;         PUSH R4
;         PUSH R2
;         MOD

;         PUSH R2
;         PUSH 2
;         DIV
;         SUB

;         POP R0
;         PUSH R0
;         PUSH R0
;         MUL

;         PUSH 4
;         DIV

;         ; Y
;         PUSH R4
;         PUSH R2
;         DIV

;         PUSH R3
;         PUSH 2
;         DIV
;         SUB

;         POP R0
;         PUSH R0
;         PUSH R0
;         MUL

;         ADD
;         PUSH R1
;         PUSH R1
;         MUL
;         JGE :not_circle
;             PUSH 48
;             POP [R4]
;         :not_circle

;         JMP :start_circle
;     :end_circle

;     DRAW
; RET


; =============Quadratka=============
; R1 - a
; R2 - b
; R3 - c

IN
POP R1
IN
POP R2
IN
POP R3

CALL :quadratka
HLT

:quadratka
    PUSH R1
    PUSH 0
    JNE :a_nzero
        PUSH R2
        PUSH 0
        JNE :ab_nzero
            PUSH R3
            PUSH 0
            JNE :abc_nzero
                PUSH 666
                OUT
                RET
            :abc_nzero
            PUSH 1
            OUT
            PUSH 0
            OUT
            RET
        :ab_nzero
        PUSH 1
        OUT
        PUSH 0
        PUSH R3
        SUB
        PUSH R2
        DIV
        OUT
        RET
    :a_nzero
    PUSH R2
    PUSH 0
    JNE :b_nzero
        PUSH R3
        PUSH 0
        JNE :bc_nzero
            PUSH 1
            OUT
            PUSH 0
            OUT
            RET
        :bc_nzero
        PUSH 0
        PUSH R3
        SUB
        PUSH R1
        DIV
        POP R0
        PUSH R0
        PUSH R0
        PUSH 0
        JGE :sqr_arg_pos
            PUSH 666
            OUT
            RET
        :sqr_arg_pos
        SQR
        PUSH 2
        OUT
        POP R0
        PUSH 0
        PUSH R0
        SUB
        OUT
        PUSH R0
        OUT
        RET
    :b_nzero
    PUSH R3
    PUSH 0
    JNE :c_nzero
        PUSH 2
        OUT
        PUSH 0
        OUT
        PUSH 0
        PUSH R2
        SUB
        PUSH R1
        DIV
        OUT
        RET
    :c_nzero
    ; R4 - discriminant
    PUSH R2
    PUSH R2
    MUL
    PUSH 4
    PUSH R1
    PUSH R3
    MUL
    MUL
    SUB
    POP R4
    CALL :discriminant
    RET

:discriminant
    PUSH R4
    PUSH 0
    JGE :discr_pos0
        PUSH 228
        OUT
        RET
    :discr_pos0
    PUSH R4
    PUSH 0
    JNE :discr_pos
        PUSH 1
        OUT
        PUSH 0
        PUSH R2
        PUSH 2
        PUSH R1
        MUL
        DIV
        SUB
        OUT
        RET
    :discr_pos
    PUSH 2
    OUT

    PUSH R4
    SQR
    POP R4

    PUSH 0
    PUSH R2
    SUB
    POP R0

    PUSH R0
    PUSH R4
    SUB
    PUSH 2
    PUSH R1
    MUL
    DIV
    OUT

    PUSH R0
    PUSH R4
    ADD
    PUSH 2
    PUSH R1
    MUL
    DIV
    OUT
    RET



    
