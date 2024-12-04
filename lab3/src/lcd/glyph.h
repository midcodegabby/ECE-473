#include <stdint.h>

#define GLYPH_WIDTH 5
#define GLYPH_HEIGHT 8

typedef struct {
  uint8_t const cols[GLYPH_WIDTH];
} glyph_t;

/* Converts an ASCII character to a glyph reprentation
 * suitable for display */
extern glyph_t const *ascii_to_glyph(char c);
