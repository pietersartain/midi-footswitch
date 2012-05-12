;==============================================================================
; Cyan Technology Ltd
; Example application software for eCOG
;
; FILE - irq.asm
;  Contains the interrupt vectors and the initial branch to the main code.
;
; DESCRIPTION
;==============================================================================

                  MODULE   irq
                  .ALL
;
; Interrupt vectors are sign-extended 24 bit absolute branch addresses. For
; example a vector containing H'1234 will start executing at H'1234 and a
; vector containing H'8765 will start executing at H'ff8765.
;
                  ORG      H'0
                  bra      $?cstart_code

                  ORG      H'4

ex_debug                        DC $minimal_handler
ex_tim_wdog_ufl                 DC $minimal_handler
ex_adr_err                      DC $address_error
reserved                        DC $minimal_handler
ex_tim                          DC $minimal_handler
ex_reserved                     DC $minimal_handler
ex_usart_a                      DC $minimal_handler
ex_usart_b                      DC $minimal_handler
ex_uart_a                       DC $minimal_handler
ex_uart_b                       DC $minimal_handler
int_tim_tmr_ufl                 DC $minimal_handler
int_tim_cnt1_ufl                DC $minimal_handler
int_tim_cnt2_ufl                DC $minimal_handler
int_tim_cnt1_match              DC $minimal_handler
int_tim_cnt2_match              DC $minimal_handler
int_tim_pwm1_ufl                DC $minimal_handler
int_tim_pwm2_ufl                DC $minimal_handler
int_tim_pwm1_match              DC $minimal_handler
int_tim_pwm2_match              DC $minimal_handler
int_tim_cap_ofl                 DC $minimal_handler
int_tim_cap1                    DC $minimal_handler
int_tim_cap2                    DC $minimal_handler
int_tim_cap3                    DC $minimal_handler
int_tim_cap4                    DC $minimal_handler
int_tim_cap5                    DC $minimal_handler
int_tim_cap6                    DC $minimal_handler
int_tim_ltmr_ufl                DC $minimal_handler
int_reserved1                   DC $minimal_handler
int_reserved2                   DC $minimal_handler
int_reserved3                   DC $minimal_handler
int_reserved4                   DC $minimal_handler
int_reserved5                   DC $minimal_handler
int_reserved6                   DC $minimal_handler
int_reserved7                   DC $minimal_handler
int_reserved8                   DC $minimal_handler
int_usart_a_rx                  DC $minimal_handler
int_usart_a_tx                  DC $minimal_handler
int_usart_b_rx                  DC $minimal_handler
int_usart_b_tx                  DC $minimal_handler
int_sci_tx_done                 DC $minimal_handler
int_sci_tx_err                  DC $minimal_handler
int_sci                         DC $minimal_handler
int_ifr_tx_done                 DC $minimal_handler
int_ifr_rx_done                 DC $minimal_handler
int_ifr_rx_err                  DC $minimal_handler
int_ifr_frame_done              DC $minimal_handler
int_uart_a_tx_rdy               DC $minimal_handler
int_uart_a_rx_rdy               DC $minimal_handler
int_uart_b_tx_rdy               DC $minimal_handler
int_uart_b_rx_rdy               DC $minimal_handler
int_ehi                         DC $minimal_handler
int_gpio                        DC $gpio_handler??
int_adc                         DC $minimal_handler

                  ENDMOD
