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


; ================CIRCLE================
; R1 - radius
; R2 - width
; R3 - height

;MAIN
    IN
    POP R1
    IN
    POP R2
    IN
    POP R3

    CALL :print_circle
HLT


:print_circle
    ; R4 - counter out
    PUSH 0
    POP R4
    :start_circle
    PUSH R4
    PUSH 1
    ADD
    POP R4
    ;
    PUSH R4
    PUSH R3
    PUSH R2
    MUL
    JG :end_circle
    ;
        ; PUSH R4
        ; OUT

        ; X
        PUSH R4
        PUSH R2
        MOD

        PUSH R2
        PUSH 2
        DIV
        SUB

        POP R0
        PUSH R0
        PUSH R0
        MUL

        PUSH 3
        DIV

        ; Y
        PUSH R4
        PUSH R2
        DIV

        PUSH R3
        PUSH 2
        DIV
        SUB

        POP R0
        PUSH R0
        PUSH R0
        MUL

        ADD
        PUSH R1
        PUSH R1
        MUL
        JGE :not_circle
            PUSH 48
            POP [R4]
        :not_circle

        JMP :start_circle
    :end_circle

    DRAW
RET
    
