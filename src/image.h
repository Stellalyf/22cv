#ifndef IMAGE_H
#define IMAGE_H
#include <stdio.h>

#include "matrix.h"
#define TWOPI 6.2831853

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#ifdef UWIMGDLL_EXPORTS
#define UWIMGDLL_API __declspec(dllexport)
#else
#define UWIMGDLL_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

// DO NOT CHANGE THIS FILE

typedef struct{
    int w,h,c;
    float *data;
} image;

// A 2d point.
// float x, y: the coordinates of the point.
typedef struct{
    float x, y;
} point;

// A descriptor for a point in an image.
// point p: x,y coordinates of the image pixel.
// int n: the number of floating point values in the descriptor.
// float *data: the descriptor for the pixel.
typedef struct{
    point p;
    int n;
    float *data;
} descriptor;

// A match between two points in an image.
// point p, q: x,y coordinates of the two matching pixels.
// int ai, bi: indexes in the descriptor array. For eliminating duplicates.
// float distance: the distance between the descriptors for the points.
typedef struct{
    point p, q;
    int ai, bi;
    float distance;
} match;

// Basic operations
float UWIMGDLL_API get_pixel(image im, int x, int y, int c);
void UWIMGDLL_API set_pixel(image im, int x, int y, int c, float v);
image UWIMGDLL_API copy_image(image im);
image UWIMGDLL_API rgb_to_grayscale(image im);
image UWIMGDLL_API grayscale_to_rgb(image im, float r, float g, float b);
void UWIMGDLL_API rgb_to_hsv(image im);
void UWIMGDLL_API hsv_to_rgb(image im);
void UWIMGDLL_API shift_image(image im, int c, float v);
void UWIMGDLL_API scale_image(image im, int c, float v);
void UWIMGDLL_API clamp_image(image im);
image UWIMGDLL_API get_channel(image im, int c);
int UWIMGDLL_API same_image(image a, image b, float eps);
image UWIMGDLL_API sub_image(image a, image b);
image UWIMGDLL_API add_image(image a, image b);

// Loading and saving
image UWIMGDLL_API make_image(int w, int h, int c);
image UWIMGDLL_API load_image(char *filename);
void UWIMGDLL_API save_image(image im, const char *name);
void UWIMGDLL_API save_image_binary(image im, const char *fname);
image UWIMGDLL_API load_image_binary(const char *fname);
void UWIMGDLL_API save_png(image im, const char *name);
void UWIMGDLL_API free_image(image im);

// Resizing
float UWIMGDLL_API nn_interpolate(image im, float x, float y, int c);
image UWIMGDLL_API nn_resize(image im, int w, int h);
float UWIMGDLL_API bilinear_interpolate(image im, float x, float y, int c);
image UWIMGDLL_API bilinear_resize(image im, int w, int h);

// Filtering
image UWIMGDLL_API convolve_image(image im, image filter, int preserve);
image UWIMGDLL_API make_box_filter(int w);
image UWIMGDLL_API make_highpass_filter();
image UWIMGDLL_API make_sharpen_filter();
image UWIMGDLL_API make_emboss_filter();
image UWIMGDLL_API make_gaussian_filter(float sigma);
image UWIMGDLL_API make_gx_filter();
image UWIMGDLL_API make_gy_filter();
void UWIMGDLL_API feature_normalize(image im);
void UWIMGDLL_API l1_normalize(image im);
void UWIMGDLL_API threshold_image(image im, float thresh);
image UWIMGDLL_API*sobel_image(image im);
image UWIMGDLL_API colorize_sobel(image im);
image UWIMGDLL_API smooth_image(image im, float sigma);

// Harris and Stitching
point UWIMGDLL_API make_point(float x, float y);
point UWIMGDLL_API project_point(matrix H, point p);
matrix UWIMGDLL_API compute_homography(match *matches, int n);
image UWIMGDLL_API structure_matrix(image im, float sigma);
image UWIMGDLL_API cornerness_response(image S);
void UWIMGDLL_API free_descriptors(descriptor *d, int n);
image UWIMGDLL_API cylindrical_project(image im, float f);
void UWIMGDLL_API mark_corners(image im, descriptor *d, int n);
image UWIMGDLL_API find_and_draw_matches(image a, image b, float sigma, float thresh, int nms);
void UWIMGDLL_API detect_and_draw_corners(image im, float sigma, float thresh, int nms);
int UWIMGDLL_API model_inliers(matrix H, match *m, int n, float thresh);
image UWIMGDLL_API combine_images(image a, image b, matrix H);
match UWIMGDLL_API*match_descriptors(descriptor *a, int an, descriptor *b, int bn, int *mn);
descriptor UWIMGDLL_API*harris_corner_detector(image im, float sigma, float thresh, int nms, int *n);
image UWIMGDLL_API panorama_image(image a, image b, float sigma, float thresh, int nms, float inlier_thresh, int iters, int cutoff);

// Optical Flow
image UWIMGDLL_API make_integral_image(image im);
image UWIMGDLL_API box_filter_image(image im, int s);
image UWIMGDLL_API time_structure_matrix(image im, image prev, int s);
image UWIMGDLL_API velocity_image(image S, int stride);
image UWIMGDLL_API optical_flow_images(image im, image prev, int smooth, int stride);
void UWIMGDLL_API optical_flow_webcam(int smooth, int stride, int div);
void UWIMGDLL_API draw_flow(image im, image v, float scale);

#ifdef OPENCV
void UWIMGDLL_API *open_video_stream(const char *f, int c, int w, int h, int fps);
image UWIMGDLL_API get_image_from_stream(void *p);
void UWIMGDLL_API make_window(char *name, int w, int h, int fullscreen);
int UWIMGDLL_API show_image(image im, const char *name, int ms);
#endif

// Machine Learning

typedef enum{LINEAR, LOGISTIC, RELU, LRELU, SOFTMAX} ACTIVATION;

typedef struct {
    matrix in;              // Saved input to a layer
    matrix w;               // Current weights for a layer
    matrix dw;              // Current weight updates
    matrix v;               // Past weight updates (for use with momentum)
    matrix out;             // Saved output from the layer
    ACTIVATION activation;  // Activation the layer uses
} layer;

typedef struct{
    matrix X;
    matrix y;
} data;

typedef struct {
    layer *layers;
    int n;
} model;

data UWIMGDLL_API load_classification_data(char *images, char *label_file, int bias);
void UWIMGDLL_API free_data(data d);
data UWIMGDLL_API random_batch(data d, int n);
char UWIMGDLL_API*fgetl(FILE *fp);
void UWIMGDLL_API activate_matrix(matrix m, ACTIVATION a);
void UWIMGDLL_API gradient_matrix(matrix m, ACTIVATION a, matrix d);
matrix UWIMGDLL_API forward_layer(layer *l, matrix in);
matrix UWIMGDLL_API backward_layer(layer *l, matrix delta);
void UWIMGDLL_API update_layer(layer *l, double rate, double momentum, double decay);
layer UWIMGDLL_API make_layer(int input, int output, ACTIVATION activation);
matrix UWIMGDLL_API load_matrix(const char *fname);
void UWIMGDLL_API save_matrix(matrix m, const char *fname);

void UWIMGDLL_API train_model(model m, data d, int batch, int iters, double rate, double momentum, double decay);
double UWIMGDLL_API accuracy_model(model m, data d);
matrix UWIMGDLL_API forward_model(model m, matrix X);

#ifdef __cplusplus
}
#endif
#endif

