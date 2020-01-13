// Based on work by Francesco Sacchi

#include "Arduino.h"
#include <string.h>
#include <ctype.h>
#include "AX25.h"
#include "HDLC.h"
#include "CRC-CCIT.h"
#include "AFSK.h"

#define countof(a) sizeof(a)/sizeof(a[0])
#define MIN(a,b) ({ typeof(a) _a = (a); typeof(b) _b = (b); ((typeof(_a))((_a < _b) ? _a : _b)); })

void ax25_init(AX25Ctx *ctx) {
    memset(ctx, 0, sizeof(*ctx));
    ctx->crc_in = ctx->crc_out = CRC_CCIT_INIT_VAL;
}

static void ax25_putchar(AX25Ctx *ctx, uint8_t c)
{
    if (c == HDLC_FLAG || c == HDLC_RESET || c == AX25_ESC) afsk_putchar(AX25_ESC);
    ctx->crc_out = update_crc_ccit(c, ctx->crc_out);
    afsk_putchar(c);
}

void ax25_sendRaw(AX25Ctx *ctx, void *_buf, size_t len) {
    ctx->crc_out = CRC_CCIT_INIT_VAL;
    afsk_putchar(HDLC_FLAG);
    const uint8_t *buf = (const uint8_t *)_buf;
    while (len--) ax25_putchar(ctx, *buf++);
    uint8_t crcl = (ctx->crc_out & 0xff) ^ 0xff;
    uint8_t crch = (ctx->crc_out >> 8) ^ 0xff;
    ax25_putchar(ctx, crcl);
    ax25_putchar(ctx, crch);
    afsk_putchar(HDLC_FLAG);
}

static void ax25_sendCall(AX25Ctx *ctx, const AX25Call *addr, bool last){
    unsigned len = MIN((sizeof(addr->call) - CALL_OVERSPACE), strlen(addr->call));
    for (unsigned i = 0; i < len; i++) {
        uint8_t c = addr->call[i];
        c = toupper(c);
        ax25_putchar(ctx, c << 1);
    }
    if (len < (sizeof(addr->call) - CALL_OVERSPACE)) {
        for (unsigned i = 0; i < (sizeof(addr->call) - CALL_OVERSPACE) - len; i++) {
            ax25_putchar(ctx, ' ' << 1);
        }
    }
    uint8_t ssid = 0x60 | (addr->ssid << 1) | (last ? 0x01 : 0);
    ax25_putchar(ctx, ssid);
}

void ax25_sendVia(AX25Ctx *ctx, const AX25Call *path, size_t path_len, const void *_buf, size_t len) {
    const uint8_t *buf = (const uint8_t *)_buf;
    ctx->crc_out = CRC_CCIT_INIT_VAL;
    afsk_putchar(HDLC_FLAG);
    for (size_t i = 0; i < path_len; i++) {
        ax25_sendCall(ctx, &path[i], (i == path_len - 1));
    }
    ax25_putchar(ctx, AX25_CTRL_UI);
    ax25_putchar(ctx, AX25_PID_NOLAYER3);
    while (len--) {
        ax25_putchar(ctx, *buf++);
    }
    uint8_t crcl = (ctx->crc_out & 0xff) ^ 0xff;
    uint8_t crch = (ctx->crc_out >> 8) ^ 0xff;
    ax25_putchar(ctx, crcl);
    ax25_putchar(ctx, crch);
    afsk_putchar(HDLC_FLAG);
}
