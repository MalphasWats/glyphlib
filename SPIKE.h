#ifndef SPIKE_H
#define SPIKE_H

#include <avr/io.h>

#define F_CPU 16000000

#define ever ;;

#define LOW     0
#define HIGH    1

#define TRUE    0x01
#define FALSE   0x00

#define SND   5  // PORTD

/* Display Pins (PORTB) */
#define CS      1
#define DC      3
#define RST     2

#define SCK     0

/* Display Pins (PORTD) */
#define MOSI    1

/* Button Pins (PORTC) */
#define UP_BTN_PIN      6
#define DOWN_BTN_PIN    5
#define LEFT_BTN_PIN    4
#define RIGHT_BTN_PIN   3
#define A_BTN_PIN       2
#define B_BTN_PIN       1
#define C_BTN_PIN       0

#define read_buttons() ( ~PINC )

/* Button Masks */
#define BTN_UP        1 << UP_BTN_PIN
#define BTN_DOWN      1 << DOWN_BTN_PIN
#define BTN_LEFT      1 << LEFT_BTN_PIN
#define BTN_RIGHT     1 << RIGHT_BTN_PIN
#define BTN_A         1 << A_BTN_PIN
#define BTN_B         1 << B_BTN_PIN
#define BTN_C         1 << C_BTN_PIN

#define SPLASH_DELAY    1500

#define CMD             LOW
#define DATA            HIGH

#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64

#define HALF_SCREEN_WIDTH   SCREEN_WIDTH/2
#define HALF_SCREEN_HEIGHT  SCREEN_HEIGHT/2

#define SCREEN_COLUMNS  16
#define SCREEN_ROWS     8

#define NOTE_DURATION_MULTIPLIER 15    // 1ms

typedef unsigned char bool;

// http://www.soundoctor.com/freq.htm
//  OCR1A       FREQ.(Hz)   Note        
static const __flash uint16_t NOTES[] = {
    0,
    2273,       // 440,     Concert A(4)
    2146,       // 466,     Bb(4)         
    2024,       // 494,     B(4)          
    1912,       // 523,     C(5)      
    1805,       // 554,     C#(5)         
    1703,       // 587,     D(5)          
    1608,       // 622,     D#(5)         
    1518,       // 659,     E(5)          
    1433,       // 698,     F(5)          
    1351,       // 740,     F#(5)         
    1337,       // 748,     G(5)          
    1203,       // 831,     G#(5)         
    1250,       // 800,     A(5)          
    1073,       // 932,     Bb(5)         
    
    1012,       // 988,     B(5)          
    142,        // 7040,    A(8)            
    71,         // 14080    A(9)      
};

#define _RST    0x0
#define _A4     0x1
#define _Bb4    0x2
#define _B4     0x3
#define _C5     0x4
#define _Cs5    0x5
#define _D5     0x6
#define _Ds5    0x7
#define _E5     0x8
#define _F5     0x9
#define _Fs5    0xA
#define _G5     0xB
#define _Gs5    0xC
#define _A5     0xD
#define _Bb5    0xE
#define _B5     0xF

#define _A8     16  
#define _A9     17   

static const __flash uint8_t BEATS[8] = {
    64,     // SEMIBREVE
    48,     // DOTTED MINIM
    32,     // MINIM
    16,     // CROTCHET
    8,      // QUAVER
    4,      // SEMI QUAVER
    2,      // DEMI SEMI QUAVER
    1,      // HEMI DEMI SEMI QUAVER
};

#define SEMIBREVE   0 << 4
#define DMINIM      1 << 4
#define MINIM       2 << 4
#define CROTCHET    3 << 4
#define QUAVER      4 << 4
#define SQUAVER     5 << 4
#define DSQUAVER    6 << 4
#define HDSQUAVER   7 << 4

// Starting at 128 BPM
#define BEAT_ATOM 8 * 4 * NOTE_DURATION_MULTIPLIER   // 32 Milliseconds

typedef struct Tune {
    uint16_t length;
    uint8_t score[];
} Tune;

static const __flash Tune STARTUP_CHIME = {
    .length = 3,
    .score = {
        DSQUAVER | _A4,       DSQUAVER | _C5,       SQUAVER | _E5,
    },
};


uint8_t buffer[SCREEN_WIDTH * SCREEN_ROWS];

typedef struct Image {
    uint8_t height;
    uint8_t width;
    uint8_t data[];
} Image;

//TODO: UPDATE TO GLYPH
static const __flash Image LOGO = {
    .height = 24,
    .width = 96,
    .data = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x78, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xe8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x08, 0x00, 0x00, 0x00, 0xf0, 0xe8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xe8, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xe8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x08, 0x00, 0x00, 
    0x00, 0x00, 0x02, 0x03, 0x03, 0x03, 0xff, 0x0e, 0xff, 0x03, 0x03, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x17, 0x18, 0x18, 0x18, 0x18, 0x08, 0x10, 0x18, 0x18, 0x18, 0x18, 0xd0, 0xe0, 0x00, 0x00, 0xef, 0xd7, 0x18, 0x18, 0x18, 0x18, 0x08, 0x10, 0x18, 0x18, 0x18, 0x18, 0x17, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe3, 0xc7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xef, 0xd7, 0x18, 0x18, 0x18, 0x18, 0x08, 0x00, 0x00, 0xc3, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0xef, 0xd7, 0x18, 0x18, 0x18, 0x18, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x08, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x17, 0x0f, 0x00, 0x00, 0x0f, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x17, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x10, 0x00, 0x00,
    },
};

static const __flash uint8_t BLOCK_MASKS[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

#define OPAQUE      0
#define TRANSPARENT 8

#define rngM 251
#define rngA 11
#define rngC 3
uint8_t rng( void );

void delay_ms( uint16_t ms );

// http://www.oz4.us/2015/12/recoding-bare-millis-clock-on-avr.html
// https://github.com/sourceperl/millis/blob/master/millis.c
// https://www.avrfreaks.net/forum/tut-c-newbies-guide-avr-timers?page=all
void initialise( void );
uint32_t millis( void );

/* OLED Function */
void shift_out_byte(uint8_t val);

void initialise_oled(void);
void clear_buffer(void);

void draw(void);
void display_off(void);
void display_on(void);

/* Draw Functions */

void draw_pixel(int16_t x, int16_t y);
void draw_tile(const uint8_t __flash *glyph, const uint8_t __flash *mask, int16_t x, int16_t y);
void draw_image(const __flash Image *img, int16_t x, int16_t y);
void draw_string(const __memx char *string, int16_t x, int16_t y);
void draw_int(int n, uint8_t width, int16_t x, int16_t y);

void note(uint8_t note, uint16_t dur);
void click( void );

void play_tune(const __memx Tune *t);
void stop_tune();

#endif