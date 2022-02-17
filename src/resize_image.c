#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    // TODO Fill in
    return get_pixel(im, x + 0.5, y + 0.5, c);
}

image nn_resize(image im, int w, int h)
{
    // TODO Fill in (also fix that first line)
    int i, j, k;
    float a, b, ar, br, m, n;

    image new = make_image(w, h, im.c);
    a = im.w / (float)w;
    b = im.h / (float)h;
    ar = -0.5 + a / 2;
    br = -0.5 + b / 2;
    for (i = 0; i < w; i++) {
        m = a * i + ar;
        for (j = 0; j < h; j ++) {
            n = b * j + br;
            for (k = 0; k < im.c; k++) {
                set_pixel(new, i, j, k, nn_interpolate(im, m, n, k));
            }
        }
    }



    
    return new;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    // TODO
    float d1, d2, d3, d4;
    float q1, q2;
    d1 = x - floorf(x);
    d2 = ceilf(x) - x;
    d3 = y - floorf(y);
    d4 = ceilf(y) - y;

    q1 = d2 * d4 * get_pixel(im, floorf(x), floorf(y), c) + d2 * d3 * get_pixel(im, floorf(x), ceilf(y), c);
    q2 = d1 * d4 * get_pixel(im, ceilf(x), floorf(y), c) + d1 * d3 * get_pixel(im, ceilf(x), ceilf(y), c);

    return q1 + q2;
}

image bilinear_resize(image im, int w, int h)
{
    // TODO
    int i, j, k;
    float a, b, ar, br, m, n;

    image new = make_image(w, h, im.c);
    a = im.w / (float)w;
    b = im.h / (float)h;
    ar = -0.5 + a / 2;
    br = -0.5 + b / 2;
    for (i = 0; i < w; i++) {
        m = a * i + ar;
        for (j = 0; j < h; j ++ ) {
            n = b * j + br;
            for (k = 0; k < im.c; k++) {
                set_pixel(new, i, j, k, bilinear_interpolate(im, m, n, k));
            }
        }
    }
    return new;
}

