; serial.asm
            .cdecls C, LIST, "msp430g2231.h"
            .bss    in_bit_mask, 2      ; Serial in pin
            .bss    out_bit_mask, 2     ; Serial out pin
            .bss    bit_dur, 2          ; Bit duration in cycles
            .bss    half_dur, 2         ; Half bit duration in cycles
                                        ;
            .text                       ;
            .def    serial_setup        ; void serial_setup(unsigned out_mask, unsigned in_mask, unsigned bit_duration);
            .def    putc                ; void putc(unsigned c);
            .def    puts                ; void puts(char *s);
            .def    getc                ; unsigned getc(void);
            .def	Red_On	     		;
            .def	Red_Off 			;
            .def	Green2_On     		;
            .def	Green2_Off 			;            
            .def	All_Off				;
                                        ;
                                        ;
serial_setup                            ; - Setup serial I/O bitmasks and bit duration (32 minimum)
            mov     R12, &out_bit_mask  ; Save serial output bitmask
            mov     R13, &in_bit_mask   ; Save serial input bitmask
            bis     R12, &P1DIR         ; Setup output pin
            bis     R12, &P1OUT         ;
            bic     R13, &P1DIR         ; Setup input pin
            or      R13, R12            ;
            bic     R12, &P1SEL         ; Setup peripheral select
            mov     R14, R12            ;
            sub     #16, R14            ; Adjust count for loop overhead
            rla     R14                 ; Multiply by 2 because NOP is two bytes
            mov     R14, &bit_dur       ; Save bit duration
            sub     #32, R12            ; Adjust count for loop overhead
            mov     R12, &half_dur      ; Save half bit duration
            							; Added by mark to allow for LED fun!
;            jmp		Red_Off             ; all others are inputs by default
;            bis.b   #00000001b,&P1DIR   ; make P1.0 and P1.6 output
            ret                         ; Return
                                        ;
                                        ; - Send a single char
putc                                    ; Char to tx in R12
                                       ; R12, R13, R14, R15 trashed
            mov     &out_bit_mask, R15  ; Serial output bitmask
            mov     &bit_dur, R14       ; Bit duration
            or      #0x0300, R12        ; Stop bit(s)
            jmp     bit_low             ; Send start bit...
                                        ;
tx_bit      mov     R14, R13            ; Get bit duration
tx_delay    nop                         ; 4 cycle loop
            sub     #8, R13             ;
            jc      tx_delay            ;
            subc    R13, PC             ; 0 to 3 cycle delay
            nop                         ; 3
            nop                         ; 2
            nop                         ; 1
                                        ;
            rra     R12                 ; Get bit to tx, test for zero
            jc      bit_high            ; If high...
bit_low     bic.b   R15, &P1OUT         ; Send zero bit
            jmp     tx_bit              ; Next bit...
bit_high    bis.b   R15, &P1OUT         ; Send one bit
            jnz     tx_bit              ; If tx data is not zero, then there are more bits to send...
                                        ;
            ret                         ; Return when all bits sent         
                                        ;
                                        ;
                                        ; - Send a NULL terminated string
puts                                    ; Tx string using putc
            push    R11                 ;
            mov     R12, R11            ; String pointer in R12, copy to R11
putsloop                                ;
            mov.b   @R11+, R12          ; Get a byte, inc pointer
            tst.b   R12                 ; Test if end of string
            jz      putsx               ; Yes, exit...
            call    #putc               ; Call putc
            jmp     putsloop            ;
putsx       pop     R11                 ;
            ret                         ;
                                        ;
getc                                    ; - Get a char
            mov     &bit_dur, R14       ; Bit duration
            mov     &in_bit_mask, R13   ; Input bitmask
            mov     #0x01FF, R12        ; 9 bits - 8 data + stop

rx_start                                ; Wait for start bit
            mov     &P1IN, R15          ; Get serial input         
            and     R13, R15            ; Mask and test bit
            jc      rx_start            ; Wait for low...
                                        ;
            mov     &half_dur, R13      ; Wait for 1/2 bit time
                                        ;
rx_delay    nop                         ; Bit delay
            sub     #8, R13             ;
            jc      rx_delay            ;
            subc    R13, PC             ; 0 to 3 cycle delay
            nop                         ; 3
            nop                         ; 2
            nop                         ; 1
                                        ;
            mov     &P1IN, R15          ; Get serial input         
            and     &in_bit_mask, R15   ;
            rrc     R12                 ; Shift in a bit
                                        ;
            mov     R14, R13            ; Setup bit timer
            jc      rx_delay            ; Next bit...
                                        ;
            rla     R12                 ; Move stop bit to carry
            swpb    R12                 ; Move rx byte to lower byte, start bit in msb
            ret                         ; Return with rx char and start bit in R12, stop bit in carry
                                        ;
                                        ; Added by Mark
Red_On      bis.b   #00000001b,&P1OUT   ; clear P1.0 (red on)
            ret							;
Red_Off     bic.b   #00000001b,&P1OUT   ; clear P1.6 (green off)
			ret
All_Off   	bic.b   #01000001b,&P1OUT   ; clear P1.6 and p1.0            
            ret						;
Green2_On   bis.b   #10000000b,&P2OUT   ; clear P2.7 (green oon)
            ret							;
Green2_Off  bic.b   #10000000b,&P2OUT   ; clear P2.7 (green oon)
            ret							;

            .end                        ;
