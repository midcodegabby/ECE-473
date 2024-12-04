  .globl delay_cycles
  .type delay_cycles, @function
delay_cycles:
  sbiw r24, 0
  brne loop
  ret

  loop:
  nop
  nop
  nop
  nop
  sbiw r24, 1
  brne loop
  ret

  .size delay_cycles, .-delay_cycles
