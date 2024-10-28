# R2 - число
# R1 - результат

# MAIN
IN
POP R2
PUSH 1
POP R1

CALL :factorial
PUSH R1
OUT

PUSH 13454321
POP [0]
DRAW
HLT


:factorial
PUSH R2
PUSH 1
JLE :ret_factorial

PUSH R1
PUSH R2
MUL
POP R1

PUSH R2
PUSH 1
SUB
POP R2
CALL :factorial

:ret_factorial
RET