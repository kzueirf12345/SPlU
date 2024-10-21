# R2 - число
# R1 - результат

# MAIN
PUSH 6
POP R2
PUSH 1
POP R1

JMP :factorial
:exit_factorial
PUSH R1
OUT
HLT



:factorial
PUSH R2
PUSH 1
JE :exit_factorial

PUSH R1
PUSH R2
MUL
POP R1

PUSH R2
PUSH 1
SUB
POP R2
JMP :factorial
