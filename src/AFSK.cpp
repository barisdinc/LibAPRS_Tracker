#include <string.h>
#include "AFSK.h"
#include "Arduino.h"

extern unsigned long custom_preamble;
extern unsigned long custom_tail;

bool hw_afsk_dac_isr = false;
Afsk *AFSK_modem;

// Forward declerations
void afsk_putchar(char c);
void AFSK_hw_init(void) {
    // Set up ADC
    TCCR1A = 0;                                    
    TCCR1B = _BV(CS10) | _BV(WGM13) | _BV(WGM12);
    ICR1 = (((CPU_FREQ+FREQUENCY_CORRECTION)) / 9600) - 1;

    ADMUX = 0;
/*
    ADC_DDR  &= ~_BV(0);
    ADC_PORT &= ~_BV(0);
    DIDR0 |= _BV(0);
  */  
    ADCSRB =    _BV(ADTS2) |
                _BV(ADTS1) |
                _BV(ADTS0);  
    ADCSRA =    _BV(ADEN) |
                _BV(ADSC) |
                _BV(ADATE)|
                _BV(ADIE) |
                _BV(ADPS2);

    AFSK_DAC_INIT();
    LED_TX_INIT();
}

void AFSK_init(Afsk *afsk) {
    // Allocate modem struct memory
    memset(afsk, 0, sizeof(*afsk));
    AFSK_modem = afsk;
    // Set phase increment
    afsk->phaseInc = MARK_INC;
    fifo_init(&afsk->txFifo, afsk->txBuf, sizeof(afsk->txBuf));
    AFSK_hw_init();
}

static void AFSK_txStart(Afsk *afsk) {
    if (!afsk->sending) {
        afsk->phaseInc = MARK_INC;
        afsk->phaseAcc = 0;
        afsk->bitstuffCount = 0;
        afsk->sending = true;
        LED_TX_ON();
        afsk->preambleLength = DIV_ROUND(custom_preamble * BITRATE, 8000);
        AFSK_DAC_IRQ_START();
    }
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      afsk->tailLength = DIV_ROUND(custom_tail * BITRATE, 8000);
    }
}

void afsk_putchar(char c) {
    AFSK_txStart(AFSK_modem);
    while(fifo_isfull_locked(&AFSK_modem->txFifo)) { /* Wait */ }
    fifo_push_locked(&AFSK_modem->txFifo, c);
}

void AFSK_transmit(char *buffer, size_t size) {
    fifo_flush(&AFSK_modem->txFifo);
    int i = 0;
    while (size--) {
        afsk_putchar(buffer[i++]);
    }
}

uint8_t AFSK_dac_isr(Afsk *afsk) {
    if (afsk->sampleIndex == 0) {
        if (afsk->txBit == 0) {
            if (fifo_isempty(&afsk->txFifo) && afsk->tailLength == 0) {
                AFSK_DAC_IRQ_STOP();
                afsk->sending = false;
                LED_TX_OFF();
                return 0;
            } else {
                if (!afsk->bitStuff) afsk->bitstuffCount = 0;
                afsk->bitStuff = true;
                if (afsk->preambleLength == 0) {
                    if (fifo_isempty(&afsk->txFifo)) {
                        afsk->tailLength--;
                        afsk->currentOutputByte = HDLC_FLAG;
                    } else {
                        afsk->currentOutputByte = fifo_pop(&afsk->txFifo);
                    }
                } else {
                    afsk->preambleLength--;
                    afsk->currentOutputByte = HDLC_FLAG;
                }
                if (afsk->currentOutputByte == AX25_ESC) {
                    if (fifo_isempty(&afsk->txFifo)) {
                        AFSK_DAC_IRQ_STOP();
                        afsk->sending = false;
                        LED_TX_OFF();
                        return 0;
                    } else {
                        afsk->currentOutputByte = fifo_pop(&afsk->txFifo);
                    }
                } else if (afsk->currentOutputByte == HDLC_FLAG || afsk->currentOutputByte == HDLC_RESET) {
                    afsk->bitStuff = false;
                }
            }
            afsk->txBit = 0x01;
        }

        if (afsk->bitStuff && afsk->bitstuffCount >= BIT_STUFF_LEN) {
            afsk->bitstuffCount = 0;
            afsk->phaseInc = SWITCH_TONE(afsk->phaseInc);
        } else {
            if (afsk->currentOutputByte & afsk->txBit) {
                afsk->bitstuffCount++;
            } else {
                afsk->bitstuffCount = 0;
                afsk->phaseInc = SWITCH_TONE(afsk->phaseInc);
            }
            afsk->txBit <<= 1;
        }

        afsk->sampleIndex = SAMPLESPERBIT;
    }
    afsk->phaseAcc += afsk->phaseInc;
    afsk->phaseAcc %= SIN_LEN;
    afsk->sampleIndex--;
    return sinSample(afsk->phaseAcc);
}

//void AFSK_adc_isr(Afsk *afsk, int8_t currentSample) {
//}

ISR(ADC_vect) {
    TIFR1 = _BV(ICF1);
    if (hw_afsk_dac_isr) {
        DAC_PORT = (AFSK_dac_isr(AFSK_modem) & 0xF0) | _BV(3); 
    } else {
        DAC_PORT = 128;
    }
}
