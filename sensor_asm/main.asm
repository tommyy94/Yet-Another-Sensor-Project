;******************************************************************************;
;   HEADER FILES
;******************************************************************************;
.nolist
.include "main.inc"
.list


;******************************************************************************;
;   RESET AND INT VECTORS
;******************************************************************************;
; Interrupts located at start of the flash
.cseg
.org $0000

rjmp RESET              ; Reset Handler
rjmp INT0_vect          ; IRQ0 Handler
reti                    ; PCINT0 Handler
reti                    ; PCINT1 Handler
reti                    ; Timer/Counter0 Capture Handler
reti                    ; Timer/Counter0 Overflow Handler
reti                    ; Timer/Counter0 Compare Match A Handler
reti                    ; Timer/Counter0 Compare Match B Handler  
reti                    ; Analog Comparator Handler     
reti                    ; Watchdog Timer Handler
reti                    ; Voltage Level Monitor Handler
rjmp ADC_vect           ; ADC Conversion Handler
reti                    ; USART0 Rx Start Handler
reti                    ; USART0 Rx Complete Handler
reti                    ; USART0 Data Register Empty Handler
reti                    ; USART0 Tx Complete Handler


RESET:

stack_init:            
    ldi r16, RAMEND
    out SPL, r16
         
CLK_init:  
    ldi r16, IO_SIGNATURE
    out CCP, r16

    ; 1 MHz Clock frequency
    ldi r16, (1 << CLKPS1) | (1 << CLKPS0)
    out CLKPSR, r16   

WDT_init:
    ; Enable Watchdog interrupt, 128K WDT Oscillator (Cycles) = ~1.0s oscillator             
    ldi r16, (1 << WDIE) | (1 << WDP2) | (1 << WDP1) 
    out WDTCSR, r16

PRR_init:
    ; Disable TIMER0
    ldi r16, (1 << PRTIM0)
    out PRR, r16     

GPIO_init:
    ; Switch
    ldi r16, (1 << SYS_RESET_PIN)
    out PUEB, r16 
    ldi r16, (1 << SYS_RESET_PIN)
    out PORTB, r16

    ; INT0
    ldi r16, (1 << ISC01)
    out EICRA, r16
    ldi r16, (1 << INT0)   
    out EIMSK, r16   
        
    ; LED
    ldi r16, (1 << DDRA5)
    out DDRA, r16
    ldi r16, ~(1 << PINA5)
    out PORTA, r16

ADC_init:
    ; Select ADC channel
    ldi r16, (1 << REFS0) | ADC0
    out ADMUX, r16
    ; Enable ADC, interrupt on success
    ldi r16, (1 << ADEN) | (1 << ADIE) | (1 << ADPS1) | (1 << ADPS0)
    out ADCSRA, r16
    ; Disable digital input buffer
    ldi r16, (1 << ADC0)
    out DIDR0, r16
                         
USART_init: 
    ; Set baud rate               
    ldi r17, HIGH(((FOSC/16)/BAUD) - OVERHEAD_ADJUST) 
    ldi r16, LOW(((FOSC/16)/BAUD) - OVERHEAD_ADJUST)  
    out UBRRH, r17
    out UBRRL, r16
    ; Enable transmission
    ldi r16, (1 << TXEN)
    out UCSRB, r16  
                
    sei ; Enable global interrupts
     

;******************************************************************************;
;   MAIN PROGRAM
;******************************************************************************;
main:      
    rcall ADC_start_conversion

    mov r20, r25
    rcall USART_transmit
    mov r20, r24    
    rcall USART_transmit

    rcall delay    
    rjmp main   


delay:
    in r16, WDTCSR
    ori r16,  (1 << WDIE)
    out WDTCSR, r16

    ; Enable sleep - Power-down mode
    ldi r16, (1 << SM1) | (1 << SE)
    out SMCR, r16
    
    sleep

    ; Disable sleep
    ldi r16, ~$FF
    out SMCR, r16
                 
    in r16, WDTCSR
    andi r16,  ~(1 << WDIE)
    out WDTCSR, r16

    ret


; Sleeps 10 ms at 1 MHz
wait_until_tx_done:
    ldi  r18, 13
    ldi  r19, 250
tx_done_loop:
    dec  r19
    brne tx_done_loop
    dec  r18
    brne tx_done_loop
    ret


USART_transmit:

wait_for_empty_tx_buff1:
    in r16, UCSRA
    sbrs r16, UDRE 
    rjmp wait_for_empty_tx_buff1
             
    out UDR, r20

    rcall wait_until_tx_done            
    ret


ADC_start_conversion:  
    in r16, PRR
    andi r16, ~(1 << PRADC)
    out PRR, r16

    ldi r16, (1 << SM0) | (1 << SE)
    out SMCR, r16

    sleep
             
    ldi r16, ~$FF
    out SMCR, r16  

    in r16, PRR
    ori r16, (1 << PRADC)
    out PRR, r16

    ret
    

software_reset:
    ldi r16, (1 << WDE)
    out WDTCSR, r16
    ret


;******************************************************************************;
;   INTERRUPT SERVICE ROUTINES
;******************************************************************************;
ADC_vect:  
    SAVE_SREG
    ; Move ADC result to register pair r25:r24 
    in r24, ADCL   
    in r25, ADCH  
    RESTORE_SREG   
     
    #ifdef HEARTBEAT
    TOGGLE_BIT PINA5, PINA, PORTA
    #endif
    reti
     


INT0_vect:   
    SAVE_SREG
    rcall software_reset  
    RESTORE_SREG   
    reti


.exit
