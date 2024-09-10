
#ifndef LIB_UTILS
#define LIB_UTILS 1

void hue_to_rgb(uint8_t hue, uint8_t *r, uint8_t *g, uint8_t *b) {
  if (hue < 85) {
    *r = hue * 3;
    *g = 255 - hue * 3;
    *b = 0;
  } else if (hue < 170) {
    hue -= 85;
    *r = 255 - hue * 3;
    *g = 0;
    *b = hue * 3;
  } else {
    hue -= 170;
    *r = 0;
    *g = hue * 3;
    *b = 255 - hue * 3;
  }
}

#define util_clamp(x, a, b) ((x) > (b) ? (b) : ((x) < (a) ? (a) : (x)))

#define linlin(x, xmin, xmax, ymin, ymax)                                 \
  util_clamp((ymin + (x - xmin) * (ymax - ymin) / (xmax - xmin)), (ymin), \
             (ymax))

#endif