;******************************************************************************;
;   HEADER FILES
;******************************************************************************;


;******************************************************************************;
;   CONSTANTS
;******************************************************************************;  
.equ FOSC = 1000000
.equ BAUD = 4800
.equ OVERHEAD_ADJUST = 1

.equ ADC0 = PINA0
.equ SYS_RESET_PIN = PINB1

.equ IO_SIGNATURE = $D8

#define HEARTBEAT


;******************************************************************************;
;   REGISTER DEFINITIONS
;******************************************************************************;


;******************************************************************************;
;   MACROS
;******************************************************************************;
.listmac

.macro SAVE_SREG    
    in r16, SREG            ;read status register
    push r16                ;save status register to stack
.endmacro


.macro RESTORE_SREG  
    pop r16                  ;get previous flag register from stack
    out SREG, r16            ;restore status register
.endmacro


.macro TOGGLE_BIT ;@bit, @pin, @port
    sbic @1, @0	                ;if bit clear, skip next line
    cbi @2, @0                  ;set bit low

    sbis @1, @0	                ;if bit high, skip next line
    sbi @2, @0                  ;set bit high
.endmacro
