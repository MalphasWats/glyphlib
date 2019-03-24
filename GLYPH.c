#include "GLYPH.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "ASCII.h"

volatile uint32_t _millis = 0;

const __memx Tune *current_tune;
uint8_t current_note;
bool playing = FALSE;

uint8_t rngSEED = 5;
uint8_t rng( void )
{
    rngSEED = (rngSEED*rngA +rngC) % rngM;
    return rngSEED;
}

void delay_ms( uint16_t ms )
{
    ms += _millis;
    if (ms<_millis)
    {
        while(_millis > 0);
        while(_millis < ms);
    }
    else
        while(_millis < ms);
}

void initialise( void )
{
    /* Configure Outputs */
    //DDRA
    DDRB = (1<<CS) | (1<<DC) | (1<<RST) | (1<<SCK);
    
    PORTC = 0xFF; // Set pull-up resistors
    
    DDRD = (1 << SNDA) | (1 << SNDB) | (1<<MOSI) | (1 << LED_1_PIN) | (1 << LED_2_PIN);
    PORTD = 0x00;
    
    /* Initialise "millis" Timer */
    TCCR0A = 0x02;          // OC0A disconnected, CTC Mode.
    TCCR0B = 0x03;          // 1/64 CLK Prescale.
    
                 //                     F_CPU   Prescale  Timer frequency (1 ms)
    OCR0A = 125; // Set compare value (8000000Hz / 64) / 1000Hz
    //OCR0A = 250; // Set compare value (16000000Hz / 64) / 1000Hz
    
    TIMSK0 |= 0x02;         // Enable OCR0A Compare Interrupt
    
    /* Configure sound timers */
    //TODO: Use Proper bit names
    TCCR1A = 0b01000001;    // phase correct pwm mode
    TCCR1B = 0b00010010;    // 1/8 Prescale
    
    OCR1A = 0;

    
    TCCR3A = 0b00000000;
    TCCR3B = 0b00001101;    // CTC Mode, 1/1024 prescale
    
    TIMSK3 = 0x02;
    
    OCR3A = 0;
    
    
    /* Configure Harware SPI
       USCZ01 = UDORD0 = 0 (MSBFIRST)
       UCSZ00 = UCPHA0 = 0 (SPI MODE 0)
       UCPOL0 =          0 (SPI MODE 0) */
    UBRR0 = 0; // MAXIMUM BAUD RATE
    UCSR0C = (1<<UMSEL01) | (1<<UMSEL00) | (0<<UCSZ01) | (0<<UCSZ00) | (0<<UCPOL0);
    UCSR0B = (0<<RXEN0) | (1<<TXEN0);
    
    UBRR0 = 0; // MAXIMUM BAUD RATE
    
    sei();                  // Enable interrupts
    
    play_tune(&STARTUP_CHIME);
    
    /* Setup Display */
    initialise_oled();
    
    draw_image(&LOGO, 16, 2);
    
    draw();
    
    delay_ms(SPLASH_DELAY);
}

ISR(TIMER0_COMPA_vect)
{
    _millis += 1;
}

ISR(TIMER3_COMPA_vect)
{
    if (playing)
    {
        if (current_note < current_tune->length)
        {
            uint8_t note_index = current_tune->score[current_note] & 0x0f;
            uint8_t beat_index = current_tune->score[current_note] >> 4;
            OCR1A = NOTES[note_index];
            TCNT3 = 0;
            OCR3A = BEATS[beat_index] * BEAT_ATOM;
            current_note += 1;
            return;
        }
        else
        {
            playing = FALSE;
        }
    }
    
    OCR3A = 0;
    OCR1A = 0;
}

void note(uint8_t note, uint16_t duration)
{
    if (!playing)
    {
        OCR1A = NOTES[note];
        TCNT3 = 0;
        OCR3A = duration * NOTE_DURATION_MULTIPLIER;
    }
}

void play_tune(const __memx Tune *t)
{
    if (!playing)
    {
        current_note = 0;
        current_tune = t;
        playing = TRUE;
        TCNT3 = 0;
        OCR3A = 1;
    }
}

void stop_tune()
{
    if (playing)
    {
        OCR3A = 0;
        OCR1A = 0;
        playing = FALSE;
    }
}

uint32_t millis( void )
{
    return _millis;
}

/* OLED Functions */
void shift_out_byte(uint8_t b)
{
    while( !( UCSR0A & (1<<UDRE0) ) );
    UDR0 = b;
}

/* Initiasation for SSD1306 OLED Controller */
void initialise_oled(void)
{
    PORTB &= ~(1 << CS);                // LOW (Enabled)
    PORTB &= ~(1 << DC);                // LOW (Command Mode)
    
    PORTB |= 1 << RST;          // HIGH
    delay_ms(10);
    PORTB &= ~(1 << RST);       // LOW
    delay_ms(10);
    PORTB |= 1 << RST;          // HIGH
    delay_ms(10);
    
    shift_out_byte(0xAE);               // DISPLAYOFF
    
    shift_out_byte(0xD5);               // SETDISPLAYCLOCKDIV
    shift_out_byte(0x80);               // the suggested ratio 0x80

    shift_out_byte(0xA8 );              // SSD1306_SETMULTIPLEX
    shift_out_byte(SCREEN_HEIGHT - 1);

    shift_out_byte(0xD3 );              // SETDISPLAYOFFSET
    shift_out_byte(0x0);                // no offset
    shift_out_byte(0x40  | 0x0);        // SETSTARTLINE line #0
  
    shift_out_byte(0x8D);               // CHARGEPUMP
    shift_out_byte(0x14);               // Not External Vcc
  
    shift_out_byte(0x20 );              // MEMORYMODE
    shift_out_byte(0x00);               // 0x0 act like ks0108
    shift_out_byte(0xA0  | 0x1);        // SEGREMAP
    shift_out_byte(0xC8 );              // COMSCANDEC


    shift_out_byte(0xDA);               // SETCOMPINS
    shift_out_byte(0x12);
  
    shift_out_byte(0x81 );              // SETCONTRAST
    shift_out_byte(0xCF);               // Not External Vcc


    shift_out_byte(0xD9 );              // SETPRECHARGE
    shift_out_byte(0xF1);               // Not External Vcc
  
    shift_out_byte(0xDB);               // SETVCOMDETECT
    shift_out_byte(0x40);
  
    shift_out_byte(0xA4 );              // DISPLAYALLON_RESUME
    shift_out_byte(0xA6 );              // NORMALDISPLAY

    //shift_out_byte(0x2E );            // DEACTIVATE_SCROLL

    shift_out_byte(0xAF);               // DISPLAYON
    
    shift_out_byte(0xB0 + 0);           // PAGEADDR (0 = reset)
    shift_out_byte(0 & 0x0F);           // Column start address (0 = reset)
    shift_out_byte(0x10 | (0 >> 4));    // LOW COL ADDR
    
    PORTB |= 1 << CS;                   // HIGH (Disabled)
    PORTB |= 1 << DC;                   // DATA
    delay_ms(1);
    PORTB &= ~(1 << CS);                // LOW (Enabled)
}

void clear_buffer(void)
{
    for (uint16_t i=0 ; i<SCREEN_WIDTH*SCREEN_ROWS ; i++)
        buffer[i] = 0x00;
}

void draw( void )
{
    for (uint16_t i=0 ; i<SCREEN_WIDTH*SCREEN_ROWS ; i++)
        shift_out_byte(buffer[i]);
}

void display_off(void)
{
    PORTB &= ~(1 << DC);        // COMMAND
    shift_out_byte(0xAE);       // DISPLAYOFF
    
    PORTB |= 1 << DC;           // DATA
}

void display_on(void)
{
    PORTB &= ~(1 << DC);        // COMMAND
    shift_out_byte(0xAF);       // DISPLAYON
    
    PORTB |= 1 << DC;           // DATA
}

void draw_pixel(int16_t x, int16_t y)
{
    buffer[ (y>>3) * SCREEN_WIDTH + x ] |= 1 << (y&7);
}

void draw_tile(const uint8_t __flash *tile, const uint8_t __flash *mask, int16_t x, int16_t y)
{
    /* is the tile actually visible */
    if (x < -7 || x >= SCREEN_WIDTH || y < -7 || y >= SCREEN_HEIGHT)
        return;
    
    
    int16_t y_ = y;
    
    if (y < 0)
        y_ = 0-y;
        
    int16_t tile_start = ((y_ >> 3) * SCREEN_WIDTH) + x;
    
    uint8_t y_offset_a = y & 7; // y % 8
    uint8_t y_offset_b = 8-y_offset_a;
    
    uint8_t tile_index = 0;
    uint8_t tile_width = 8;
    if (x < 0)
    {
        tile_start -= x;
        tile_index = 0-x;
        tile_width -= tile_index;
    }
    
    if (x > SCREEN_WIDTH-8)
    {
        tile_width = SCREEN_WIDTH-x;
    }
    
    if (y < 0)
    {
        y_offset_a = 8;
        y_offset_b = 0-y;
        tile_start -= SCREEN_WIDTH;
    }
    
    if (y > SCREEN_HEIGHT-8)
    {
        y_offset_b = 8;
    }
    
    for(uint8_t tile_offset=0 ; tile_offset<tile_width ; tile_offset++, tile_index++)
    {
        if (y_offset_a < 8)
        {
            buffer[tile_start+tile_offset] &= (mask[tile_index] << y_offset_a) | ~(0xff << y_offset_a);
            buffer[tile_start+tile_offset] |= tile[tile_index] << y_offset_a;
        }
        if (y_offset_b < 8)
        {
            buffer[tile_start+SCREEN_WIDTH+tile_offset] &= (mask[tile_index] >> y_offset_b) | ~(0xff >> y_offset_b);
            buffer[tile_start+SCREEN_WIDTH+tile_offset] |= tile[tile_index] >> y_offset_b;
        }
    }
}

void draw_image(const __flash Image *img, int16_t x, int16_t y)
{
    for(uint8_t img_y=0 ; img_y<(img->height>>3) ; img_y++)
        for(uint8_t img_x=0 ; img_x<img->width ; img_x++)
            buffer[(img_y+y)*SCREEN_WIDTH + (img_x+x)] = img->data[img_y*img->width + img_x];
}

void draw_string(const __memx char *string, int16_t x, int16_t y)
{
    for(uint8_t i=0 ; string[i] != '\0' ; i++)
    {
        draw_tile(&ASCII[(string[i]-32)*8], &BLOCK_MASKS[OPAQUE], x+(i*8), y);
    }
}

void draw_int(int16_t n, uint8_t width, int16_t x, int16_t y)
{
    //TODO: Negative numbers
    int32_t n_;
    for (uint8_t i=0 ; i<width ; i++)
    {
        n_ = (6554*(int32_t)n)>>16;
        draw_tile(&DIGITS[(n - (n_*10))*8], &BLOCK_MASKS[OPAQUE], x+((width-i-1)*8), y);
        n = (int16_t)n_;
    }
}

/* Sound Functions */

void click( void )
{
    note(_C5, 15);
}