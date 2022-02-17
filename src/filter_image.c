#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    // TODO
    int i;
    float s = 0;
    for (i = 0; i < im.w * im.h * im.c; i++) { s += im.data[i]; }
    if (s != 0) {
        for (i < im.w* im.h* im.c; i++ ;) { im.data[i] /= s; }
    }
}

image make_box_filter(int w)
{
    // TODO
    image box = make_image(w, w, 1);
    int i;
    for (i = 0; i < w * w; i++) { box.data[i] = 1; }
    l1_normalize(box);
    return box;
}

image convolve_image(image im, image filter, int preserve)
{
    // TODO
    image r; 
    int i, j, k, l, t, n, i1, j1, k1, k2;
    float v = 0;
    if (preserve == 1) {
        r = make_image(im.w, im.h, im.c);
        n = filter.c >> 1;
    }
    else {
        r = make_image(im.w, im.h, 1);
        n = 0;
    }
    l = filter.w >> 1;
    t = filter.h >> 1;

    for (k = 0; k< (preserve == 1 ? im.c : 1); k++) {
        for (i = 0; i < im.w; i++) {
            for (j = 0; j < im.h; j++) {
                for(k1 = 0; k1 < filter.c; k1++)for (i1 = 0; i1 < filter.c; i1++)for (j1 = 0; j1 < filter.c; j1++)
                if (filter.c == 1 && preserve != 1) {

                        for (k2 = 0; k2 < im.c; k2++) { v += get_pixel(im, i + i1 - l, j + j1 - t, k + k2 + k1 - n) * get_pixel(filter, i1, j1, k1); }
                }
                else { v += get_pixel(im, i + i1 - l, j + j1 - t, k + k1 - n) * get_pixel(filter, i1, j1, k1); }

            }
        }
        set_pixel(r, i, j, k, v);
    }

    return r;
}

image make_highpass_filter()
{
    // TODO
    image kernel = make_image(3, 3, 1);
    kernel.data[0] = 0.0f;
    kernel.data[1] = -1.0f;
    kernel.data[2] = 0.0f;
    kernel.data[3] = -1.0f;
    kernel.data[4] = 4.0f;
    kernel.data[5] = -1.0f;
    kernel.data[6] = 0.0f;
    kernel.data[7] = -1.0f;
    kernel.data[8] = 0.0f;
    l1_normalize(kernel);

    return kernel;
}

image make_sharpen_filter()
{
    // TODO
    image kernel = make_image(3, 3, 1);
    kernel.data[0] = 0.0f;
    kernel.data[1] = -1.0f;
    kernel.data[2] = 0.0f;
    kernel.data[3] = -1.0f;
    kernel.data[4] = 5.0f;
    kernel.data[5] = -1.0f;
    kernel.data[6] = 0.0f;
    kernel.data[7] = -1.0f;
    kernel.data[8] = 0.0f;
    l1_normalize(kernel);

    return kernel;
}

image make_emboss_filter()
{
    // TODO
    image kernel = make_image(3, 3, 1);
    kernel.data[0] = -2.0f;
    kernel.data[1] = -1.0f;
    kernel.data[2] = 0.0f;
    kernel.data[3] = -1.0f;
    kernel.data[4] = 1.0f;
    kernel.data[5] = -1.0f;
    kernel.data[6] = 0.0f;
    kernel.data[7] = 1.0f;
    kernel.data[8] = 2.0f;
    l1_normalize(kernel);

    return kernel;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: We should preserve the sharpen filter and the emboss filter. We should not preserve the highpass filter.
//         This is because that we use high pass filters to detect edges.And for other filters, we generate images to see the effect.

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: Each filter needs pre-processing, otherwise the data in the image may exceed the range(underflow or overflow).

image make_gaussian_filter(float sigma)
{
    // TODO
    int hw = roundf(6 * sigma);
    if ((hw % 2) == 0)hw += 1;
    image g = make_image(hw, hw, 1);
    int i, j;
    float sig2 = sigma * sigma;

    for (i = 0; i < hw; i++) {
        for (j = 0; j < hw; j++) {
            float x = hw / 2 - i, y = hw / 2 - j;
            set_pixel(g, i, j, 0, exp(-(x * x + y * y) / (2 * sig2)) / (TWOPI * sig2)); }
    }
    l1_normalize(g);
    return g;
}

image add_image(image a, image b)
{
    // TODO
    image add = copy_image(a);
    int i, j, k;
    for (i = 0; i < a.w; i++)for (j = 0; j < a.h; j++)for (k = 0; k < a.c; k++) set_pixel(add, i, j, k, get_pixel(a, i, j, k) + get_pixel(b, i, j, k));
    return add;
}

image sub_image(image a, image b)
{
    // TODO
    image sub = copy_image(a);
    int i, j, k;
    for (i = 0; i < a.w; i++)for (j = 0; j < a.h; j++)for (k = 0; k < a.c; k++)set_pixel(sub, i, j, k, get_pixel(a, i, j, k) + get_pixel(b, i, j, k));
    return sub;
}

image make_gx_filter()
{
    // TODO
    image kernel = make_image(3, 3, 1);
    kernel.data[0] = -1.0f;
    kernel.data[1] = 0.0f;
    kernel.data[2] = 1.0f;
    kernel.data[3] = -2.0f;
    kernel.data[4] = 0.0f;
    kernel.data[5] = 2.0f;
    kernel.data[6] = -1.0f;
    kernel.data[7] = 0.0f;
    kernel.data[8] = 1.0f;
    l1_normalize(kernel);

    return kernel;
}

image make_gy_filter()
{
    // TODO
    image kernel = make_image(3, 3, 1);
    kernel.data[0] = -1.0f;
    kernel.data[1] = -2.0f;
    kernel.data[2] = -1.0f;
    kernel.data[3] = 0.0f;
    kernel.data[4] = 0.0f;
    kernel.data[5] = 0.0f;
    kernel.data[6] = 1.0f;
    kernel.data[7] = 2.0f;
    kernel.data[8] = 1.0f;
    l1_normalize(kernel);

    return kernel;
}

void feature_normalize(image im)
{
    // TODO
    float mi = im.data[0], ma = im.data[0], delta, tmp;
    int i;
    for (i = 1; i < im.w * im.h * im.c; i++) {
        if (mi > im.data[i])mi = im.data[i];
        if (ma < im.data[i])ma = im.data[i];
    }
    delta = ma - mi;
    if (delta == 0)
    {
        for (i = 0; i < im.w * im.h * im.c; i++)im.data[i] = 0;
    }
    else {
        for (i = 0; i < im.w * im.h * im.c; i++) {
            tmp = im.data[i] - mi;
            im.data[i] = tmp / delta;
        }
    }
}



image *sobel_image(image im)
{
    // TODO
    image gx = make_gx_filter(), gy = make_gy_filter();
    image gx1 = convolve_image(im, gx, 0), gy1 = convolve_image(im, gy, 0);
    image* gt = calloc(2, sizeof(image));
    gt[0] = make_image(im.w, im.h, 1);
    gt[1] = make_image(im.w, im.h, 1);
    int i;
    for (i = 0; i < im.w * im.h; i++) {
        gt[0].data[i] = sqrt(gx1.data[i] * gx1.data[i] + gy1.data[i] * gy1.data[i]);
        gt[1].data[i] = atan2(gy1.data[i], gx1.data[i]);
    }
    free_image(gx);
    free_image(gx1);
    free_image(gy);
    free_image(gy1);
    return gt;
}

image colorize_sobel(image im)
{
    // TODO

    image gx = make_gx_filter(), gy = make_gy_filter();
    image gx1 = convolve_image(im, gx, 0), gy1 = convolve_image(im, gy, 0);
    image gt = make_image(im.w, im.h, im.c);
    int i;
    for (i = 0; i < im.w * im.h; i++) {
        gt.data[i] = atan2(gy1.data[i], gx1.data[i]);
        gt.data[im.w * im.h + i] = gx1.data[i];
        gt.data[im.w * im.h + im.w * im.h + i] = gy1.data[i];
    }
    free_image(gx);
    free_image(gx1);
    free_image(gy);
    free_image(gy1);
    hsv_to_rgb(gt);
    return gt;
}
