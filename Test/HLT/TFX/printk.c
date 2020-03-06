/*
 * printk.c - print out a message to console.
 *
 * Copyright(c) 2009 MDS Technology Co.,Ltd.
 * All rights reserved.
 *
 * This software contains confidential information of MDS Technology Co.,Ltd.
 * and unauthorized distribution of this software, or any portion of it, are
 * prohibited.
 */

#include <NEOS178STypes.h>
#include <stdarg.h>

extern void *MemoryCopy(void *dest, const void *src, unsigned long size);

#define NIOV                8
#define PADSIZE             16      /* pad chunk size */
#define BUF                 500

typedef struct kbuf {
    unsigned char *_p;
} KBUF;

#define PRINT(ptr, len)                 \
        MemoryCopy(fp->_p, ptr, len);       \
        fp->_p += len;                  \

#define PAD(howmany, with)              \
    do {                                \
        if ((n = (howmany)) > 0) {      \
            while (n > PADSIZE) {       \
                PRINT(with, PADSIZE);   \
                n -= PADSIZE;           \
            }                           \
            PRINT(with, n);             \
        }                               \
    } while (0)

/*
 * To extend shorts properly, we need both signed and unsigned
 * argument extraction methods.
 */
#define SARG()                                          \
    (flags&QUADINT ? va_arg(ap, int64_t) :              \
     flags&LONGINT ? va_arg(ap, long) :                 \
     flags&SHORTINT ? (long)(short)va_arg(ap, int) :    \
     (int)va_arg(ap, int))

#define UARG()                                          \
    (flags&QUADINT ? va_arg(ap, uint64_t) :             \
     flags&LONGINT ? va_arg(ap, u_long) :               \
     flags&SHORTINT ? (u_long)(u_short)va_arg(ap, int) :\
     (unsigned int)va_arg(ap, u_int))


/*
 * Macros for converting digits to letters and vice versa
 */
#define to_digit(c)      ((c) - '0')
#define is_digit(c)      ((unsigned)to_digit(c) <= 9)
#define to_char(n)       ((n) + '0')


/*
 * Flags used during conversion.
 */
#define ALT             0x001       /* alternate form */
#define HEXPREFIX       0x002       /* add 0x or 0X prefix */
#define LADJUST         0x004       /* left adjustment */
#define LONGDBL         0x008       /* long double; unimplemented */
#define LONGINT         0x010       /* long integer */
#define QUADINT         0x020       /* quad integer */
#define SHORTINT        0x040       /* short integer */
#define ZEROPAD         0x080       /* zero (as opposed to blank) pad */
#define FPT             0x100       /* Floating point number */

static char blanks[PADSIZE] =
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
static char zeroes[PADSIZE] =
    {'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};

typedef unsigned int size_t;
typedef signed long long int64_t;
typedef unsigned long long uint64_t;
typedef unsigned long u_long;
typedef unsigned int u_int;
typedef unsigned short u_short;
typedef long long quad_t;
typedef unsigned long long u_quad_t;

static void *
memchr(const void *s, int c, size_t n)
{
    if (n != 0) {
        unsigned char *p = (unsigned char *)s;
		
        do {
            if (*p++ == (unsigned char)c)
                return ((void *)(p - 1));
        } while (--n != 0);
    }
	
    return (NULL);
}

size_t
strlen(const char *str)
{
    char *s;
	
    for (s = (char *)str; *s; ++s)
        ;
	
    return(s - str);
}

/*******************************************************************************
 *
 * kfprintf - write a formated string to a stream.
 */

int
kfprintf(KBUF *fp, const char *fmt0, va_list ap)
{
    char   *fmt;        /* format string */
    int     ch;         /* character from fmt */
    int     n;          /* handy integer (short term usage) */
    char   *cp;         /* handy char pointer (short term usage) */
    int     flags;      /* flags as above */
    int     ret;        /* return value accumulator */
    int     width;      /* width from format (%8d), or 0 */
    int     prec;       /* precision from format (%.3d), or -1 */
    int     base;       /* base for [diouxX] conversion */
    int     dprec;      /* a copy of prec if [diouxX], 0 otherwise */
    int     realsz;     /* field size expanded by dprec, sign, etc */
    int     size;       /* size of converted field or string */
    char    sign;       /* sign prefix (' ', '+', '-', or \0) */
    unsigned long long _uquad;    /* integer arguments %[diouxX] */
    char    *xdigs;     /* digits for [xX] conversion */
    char    buf[BUF];   /* space for %c, %[diouxX], %[eEfgG] */
    char    ox[2];      /* space for 0x hex-prefix */


    fmt   = (char *)fmt0;
    ret   = 0;
    xdigs = NULL;

    /*
     * Scan the format for conversions (`%' character).
     */
    for (;;) {
        for (cp = fmt; (ch = *fmt) != '\0' && ch != '%'; fmt++)
        /* void */;
        if ((n = fmt - cp) != 0) {
            PRINT(cp, n);
            ret += n;
        }
        if (ch == '\0')
            goto done;
        fmt++;      /* skip over '%' */

        flags = 0;
        dprec = 0;
        width = 0;
        prec = -1;
        sign = '\0';

rflag:  ch = *fmt++;
reswitch:   
        switch (ch) {
        case ' ':
            /*
             * ``If the space and + flags both appear, the space
             * flag will be ignored.''
             *  -- ANSI X3J11
             */
            if (!sign)
                sign = ' ';
            goto rflag;
        case '#':
            flags |= ALT;
            goto rflag;
        case '*':
            /*
             * ``A negative field width argument is taken as a
             * - flag followed by a positive field width.''
             *  -- ANSI X3J11
             * They don't exclude field widths read from args.
             */
            width = va_arg(ap, int);
            if (width >= 0)
                goto rflag;
            width = -width;
            /* FALLTHROUGH */
        case '-':
            flags |= LADJUST;
            goto rflag;
        case '+':
            sign = '+';
            goto rflag;
        case '.':
            if ((ch = *fmt++) == '*') {
                n = va_arg(ap, int);
                prec = n < 0 ? -1 : n;
                goto rflag;
            }
            n = 0;
            while (is_digit(ch)) {
                n = 10 * n + to_digit(ch);
                ch = *fmt++;
            }
            prec = n < 0 ? -1 : n;
            goto reswitch;
        case '0':
            /*
             * ``Note that 0 is taken as a flag, not as the
             * beginning of a field width.''
             *  -- ANSI X3J11
             */
            flags |= ZEROPAD;
            goto rflag;
        case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            n = 0;
            do {
                n = 10 * n + to_digit(ch);
                ch = *fmt++;
            } while (is_digit(ch));
            width = n;
            goto reswitch;
        case 'h':
            flags |= SHORTINT;
            goto rflag;
        case 'l':
            if (*fmt == 'l') {
                fmt++;
                flags |= QUADINT;
            } else {
                flags |= LONGINT;
            }
            goto rflag;
        case 'q':
            flags |= QUADINT;
            goto rflag;
        case 'c':
            *(cp = buf) = va_arg(ap, int);
            size = 1;
            sign = '\0';
            break;
        case 'D':
            flags |= LONGINT;
            /*FALLTHROUGH*/
        case 'd':
        case 'i':
            _uquad = SARG();
#ifndef NO_LONGLONG
            if ((quad_t)_uquad < 0)
#else
            if ((long)_uquad < 0)
#endif
            {
                _uquad = -_uquad;
                sign = '-';
            }
            base = 10;
            goto number;
        case 'n':
            if (flags & LONGINT)
                *va_arg(ap, long *) = ret;
            else if (flags & SHORTINT)
                *va_arg(ap, short *) = ret;
            else
                *va_arg(ap, int *) = ret;
            continue;   /* no output */
        case 'O':
            flags |= LONGINT;
            /*FALLTHROUGH*/
        case 'o':
            _uquad = UARG();
            base  = 8;
            goto nosign;
        case 'p':
            /*
             * ``The argument shall be a pointer to void.  The
             * value of the pointer is converted to a sequence
             * of printable characters, in an implementation-
             * defined manner.''
             *  -- ANSI X3J11
             */
            _uquad = (unsigned long)va_arg(ap, void *);
            base   = 16;
            xdigs  = "0123456789abcdef";
            flags |= HEXPREFIX;
            ch     = 'x';
            goto nosign;
        case 's':
            if ((cp = va_arg(ap, char *)) == NULL)
                cp = "(null)";
            if (prec >= 0) {
                /*
                 * can't use strlen; can only look for the
                 * NUL in the first `prec' characters, and
                 * strlen() will go further.
                 */
                char *p = (char*)memchr(cp, 0, (size_t)prec);

                if (p != NULL) {
                    size = p - cp;
                    if (size > prec)
                        size = prec;
                } else
                    size = prec;
            } else
                size = strlen(cp);  
            sign = '\0';
            break;
        case 'U':
            flags |= LONGINT;
            /*FALLTHROUGH*/
        case 'u':
            _uquad = UARG();
            base = 10;
            goto nosign;
        case 'X':
            xdigs = "0123456789ABCDEF";
            goto hex;
        case 'x':
            xdigs = "0123456789abcdef";
hex:        _uquad = UARG();
            base = 16;
            /* leading 0x/X only if non-zero */
            if ((flags & ALT) && (_uquad != 0))
                flags |= HEXPREFIX;

            /* unsigned conversions */
nosign:     sign = '\0';
            /*  
             * ``... diouXx conversions ... if a precision is
             * specified, the 0 flag will be ignored.''
             *  -- ANSI X3J11
             */
number:     if ((dprec = prec) >= 0)
                flags &= ~ZEROPAD;

            /*
             * ``The result of converting a zero value with an
             * explicit precision of zero is no characters.''
             * -- ANSI X3J11
             */
            cp = buf + BUF;
            if (_uquad != 0 || prec != 0) {
                /* Unsigned mod is hard, and unsigned mod
                 * by a constant is easier than that by 
                 * a variable; hance this switch.
                 */
                switch(base) {
                case 8: 
                    do {
                        *--cp = to_char(_uquad & 7);
                        _uquad >>= 3;
                    } while (_uquad);
                    /* handle octal leading 0 */
                    if (flags & ALT && *cp != '0')
                        *--cp = '0';
                    break;
                case 10: 
                    if (!(flags & QUADINT)) {
                        /* many numbers are 1 digit */
                        unsigned long v = (unsigned long)_uquad;
                        while (v >= 10) {
                            unsigned long next = v / 10;
                            *--cp = to_char(v - (next *10));
                            v = next;
                        }
                        *--cp = to_char(v);
                    } else {
                        while (_uquad >= 10) {
                            u_quad_t next = _uquad / 10;
                            *--cp = to_char(_uquad - (next * 10));
                            _uquad = next;
                        }
                        *--cp = to_char(_uquad);
                    }
                    break;
                case 16:
                    do {
                        *--cp = xdigs[_uquad & 15];
                        _uquad >>= 4;
                    } while (_uquad);
                    break;
                default:
                    cp = "bug in vfprintf: bad base";
                    size = strlen(cp);
                    goto skipsize;
                }
            }
                size = buf + BUF - cp;
skipsize:
            break;
        default: /* "%?" prints ?, unless ? is NUL */
            if (ch == '\0')
                goto done;
            /* pretend it was %c with argument ch */
            cp = buf;
            *cp = ch;
            size = 1;
            sign = '\0';
            break;
        }

        realsz = dprec > size ? dprec : size;
        if (sign)
            realsz++;
        else if (flags & HEXPREFIX)
            realsz += 2;

        /* right-adjusting blank padding */
        if ((flags & (LADJUST|ZEROPAD)) == 0)
            PAD(width - realsz, blanks);

        /* prefix */
        if (sign) {
            PRINT(&sign, 1);
        } else if (flags & HEXPREFIX) {
            ox[0] = '0';
            ox[1] = ch;
            PRINT(ox, 2);
        }

        /* right-adjusting zero padding */
        if ((flags & (LADJUST|ZEROPAD)) == ZEROPAD)
            PAD(width - realsz, zeroes);

        /* leading zeroes from decimal precision */
        PAD(dprec - size, zeroes);

        /* the string or number proper */
        PRINT(cp, size);

        /* left-adjusting padding (always blank) */
        if (flags & LADJUST)
            PAD(width - realsz, blanks);

        /* finally, adjust ret */
        ret += width > realsz ? width : realsz;
    }
done:
    return (ret);
    /* NOTREACHED */
}


int
ksprintf(char *str, const char *fmt, va_list ap)
{
    KBUF f;
    int ret;

    f._p = (unsigned char *)str;
    ret = kfprintf(&f, fmt, ap);
    *f._p = 0;
    return (ret);
}


extern void DebugPuts(char *);

int
printk(char *fmt, ...)
{
    char buf[BUF+1];
    va_list ap;

    va_start(ap, fmt);
    ksprintf(buf, fmt, ap);
    DebugPuts(buf);
    va_end(ap);
    return(0);
}
