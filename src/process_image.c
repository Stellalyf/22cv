#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    // TODO Fill this in
    if (x < 0)
    {
        x = 0;
    }
    else if (x >= im.w)
    {
        x = im.w - 1;
    }

    if (y < 0)
    {
        y = 0;
    }
    else if (y >= im.h)
    {
        y = im.h - 1;
    }

    if (c < 0)
    {
        c = 0;
    }
    else if (c >= im.c)
    {
        c = im.c - 1;
    }

    return im.data[c * im.h * im.w + y * im.w + x];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    // TODO Fill this in
    if (x < 0 || x >= im.w || y < 0 || y >= im.h || c < 0 || c >= im.c) return;
    im.data[c * im.h * im.w + y * im.w + x] = v;

}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    // TODO Fill this in
    int i = 0;
    for (i; i < im.h * im.w * im.c; i++)
    {
        copy.data[i] = im.data[i];
    }
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    // TODO Fill this in
    int i, j;
    for (i = 0; i < im.w; i++)for (j = 0; j < im.h; j++)
    {
        float vog = get_pixel(im, i, j, 0) * 0.229 + get_pixel(im, i, j, 1) * 0.587 + get_pixel(im, i, j, 2) * 0.114;
        set_pixel(gray, i, j, 0, vog);
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    // TODO Fill this in
    int i, j;
    for (i = 0; i < im.w; i++)for (j = 0; j < im.h; j++)
    {
        float vos = get_pixel(im, i, j, c) + v;
        set_pixel(im, i, j, c, vos);
    }
}

void clamp_image(image im)
{
    // TODO Fill this in
    int i;
    for (i = 0; i < im.w * im.h * im.c; i++)
    {
        if (i < 0) { i = 0; }
        else if (i > 1) { i = 1; }
    }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    // TODO Fill this in
    int i, j;
    float r, g, b;
    float hue, sat, val, m, c, h;
    for (i = 0; i < im.h; i++) 
    {
        for (j = 0; j < im.w; j++)
        {
            r = get_pixel(im, i, j, 0);
            g = get_pixel(im, i, j, 1);
            b = get_pixel(im, i, j, 2);
            //Value
            val = three_way_max(r, g, b);
            m = three_way_min(r, g, b);
            c = val - m;
            //Saturation
            sat = (val == 0) ? 0 : (c / val);
            //Hue
            if (c == 0) { hue = 0; }
            else if (val == r) { h = (g - b) / c; }
            else if (val == g) { h = (b - r) / c + 2; }
            else if (val == b) { h = (r - g) / c + 4; }

            hue = (h < 0) ? ((h / 6) + 1) : (h / 6);

            set_pixel(im, i, j, 0, hue);
            set_pixel(im, i, j, 1, sat);
            set_pixel(im, i, j, 2, val);

        }
    }
}

void hsv_to_rgb(image im)
{
    // TODO Fill this in
    int i, j;
    float r, g, b;
    float hue, sat, val, m, c, h, d;
    for (i = 0; i < im.h; i++)for (j = 0; j < im.w; j++) 
    {
        hue = get_pixel(im, i, j, 0);
        sat = get_pixel(im, i, j, 1);
        val = get_pixel(im, i, j, 2);

        c = val * sat;
        m = val - c;
        h = hue * 6;
        int hi = (int)h;
        d = h - hi;
        switch (hi) 
        {
        case 0:
            r = val;
            g = val * (1 - (1 - d) * sat);
            b = val - c;
            break;
        case 1:
            r = val * (1 - d * sat);
            g = val;
            b = val - c;
            break;
        case 2:
            r = val - c;
            g = val;
            b = val * (1 - (1 - d) * sat);
            break;
        case 3:
            r = val - c;
            g = val * (1 - d * sat);
            b = val;
        case 4:
            r = val * (1 - (1 - d) * sat);
            g = val - c;
            b = val;
            break;
        default:
            r = val;
            g = val - c;
            b = val * (1 - d * sat);
        }
        set_pixel(im, i, j, 0, r);
        set_pixel(im, i, j, 1, g);
        set_pixel(im, i, j, 2, b);

    }

}

void scale_image(image im, int c, float v) 
{
    int i, j;
    for (i = 0; i < im.w; i++)for (j = 0; j < im.h; j++)
    {
        float vosca = get_pixel(im, i, j, c) + v;
        set_pixel(im, i, j, c, vosca);
    }

}

// Formula reference: https://www.easyrgb.com/en/math.php
void rgb_to_hcl(image im) 
{
    int i, j;
    float r, g, b, vr, vg, vb, x, y, z;
    float vx, vy, vz, l1, a1, b1;
    float L, C, H, vh, pi = 3.11415926;
    float x_ref = 0.9642, y_ref = 1, z_ref = 0.8249;

    for (i = 0; i < im.w; i++) 
    {
        for (j = 0; j < im.h; j++) 
        {
            //sRGB to XYZ
            r = get_pixel(im, i, j, 0);
            g = get_pixel(im, i, j, 1);
            b = get_pixel(im, i, j, 2);
            vr = r / 255;
            vg = g / 255;
            vb = b / 255;
            if (vr > 0.04045) { vr = pow((vr + 0.055) / 1.055, 2.4); }
            else { vr = vr / 12.92; }
            if (vg > 0.04045) { vg = pow((vg + 0.055) / 1.055, 2.4); }
            else { vg = vg / 12.92; }
            if (vb > 0.04045) { vb = pow((vb + 0.055) / 1.055, 2.4); }
            else { vb = vb / 12.92; }

            x = vr * 41.24 + vg * 35.76 + vb * 18.05;
            y = vr * 21.26 + vg * 71.52 + vb * 7.22;
            z = vr * 1.93 + vg * 11.92 + vb * 95.05;
            //XYZ to CIELab
            vx = x / x_ref;
            vy = y / y_ref;
            vz = z / z_ref;
            if (vx > 0.008856) { vx = pow(vx, 1 / 3); }
            else { vx = (7.787 * vx) + (16 / 116); }
            if (vy > 0.008856) { vy = pow(vy, 1 / 3); }
            else { vy = (7.787 * vy) + (16 / 116); }
            if (vz > 0.008856) { vz = pow(vz, 1 / 3); }
            else { vz = (7.787 * vz) + (16 / 116); }

            l1 = (116 * vy) - 16;
            a1 = 500 * (vx - vy);
            b1 = 200 * (vy - vz);
            //CIELab to CIELCH
            vh = atan(b1, a1);
            if (vh > 0) { H = (vh / pi) * 180; }
            else { H = 360 - (abs(vh) / pi) * 180; }
            L = l1;
            C = sqrt(pow(a1, 2) + pow(b1, 2));
            set_pixel(im, i, j, 0, H);
            set_pixel(im, i, j, 1, C);
            set_pixel(im, i, j, 2, L);

        }
    }
}

void hcl_to_rgb(image im)
{
    int i, j;
    float r, g, b, vr, vg, vb, x, y, z;
    float vx, vy, vz, a1, b1;
    float L, C, H, vh, pi = 3.11415926;
    float x_ref = 0.9642, y_ref = 1, z_ref = 0.8249;

    for (i = 0; i < im.w; i++)
    {
        for (j = 0; j < im.h; j++)
        {
            //CIELCH to CIELab
            H = get_pixel(im, i, j, 0);
            C = get_pixel(im, i, j, 1);
            L = get_pixel(im, i, j, 2);
            a1 = cos(H * pi / 180) * C;
            b1 = sin(H * pi / 180) * C;

            // CIELab to XYZ
            vy = (L + 16) / 116;
            vx = a1 / 500 + vy;
            vz = vy - b1 / 200;

            if (pow(vy, 3) > 0.008856) { vy = pow(vy, 3); }
            else { vy = (vy - 16 / 116) / 7.787; }
            if (pow(vx, 3) > 0.008856) { vx = pow(vx, 3); }
            else { vx = (vx - 16 / 116) / 7.787; }
            if (pow(vz, 3) > 0.008856) { vz = pow(vz, 3); }
            else { vz = (vz - 16 / 116) / 7.787; }
            x = vx * x_ref;
            y = vy * y_ref;
            z = vz * z_ref;

            //XYZ to sRGB
            vr = x * 324.06 + y * (-153.72) + z * (-49.86);
            vg = x * (-96.89) + y * 187.58 + z * 4.15;
            vb = x * 5.57 + vy * (-20.40) + z * 105.70;

            if (vr > 0.0031308) { vr = 1.055 * pow(vr, 1 / 2.4) - 0.055; }
            else { vr = 12.92 * vr; }
            if (vg > 0.0031308) { vg = 1.055 * pow(vg, 1 / 2.4) - 0.055; }
            else { vg = 12.92 * vg; }
            if (vb > 0.0031308) { vb = 1.055 * pow(vb, 1 / 2.4) - 0.055; }
            else { vb = 12.92 * vb; }
            r = vr * 255;
            g = vg * 255;
            b = vb * 255;
            set_pixel(im, i, j, 0, r);
            set_pixel(im, i, j, 1, g);
            set_pixel(im, i, j, 2, b);
        }
    }
}
