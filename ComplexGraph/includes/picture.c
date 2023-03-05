#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

typedef struct Color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

// Color Color_from_HL(double h, double l)
// {
//     double hh = (h + M_PI) * 3.0f * M_1_PI;

//     int i = floor(hh);

//     double incr = hh - i;

//     i = (6 - (-i % 6)) % 6;

//     double r = (i == 3 || i == 2) + (1.0f - incr) * (i == 4) + incr * (i == 1);
//     double g = (i == 4 || i == 5) + (1.0f - incr) * (i == 0) + incr * (i == 3);
//     double b = (i == 0 || i == 1) + (1.0f - incr) * (i == 2) + incr * (i == 5);

//     r = r * (2.0f * l) * (0.0f <= l && l <= 0.5f) + (r + (1.0f - r) * (2.0f * l - 1.0f)) * (0.5f < l && l <= 1.0f);
//     g = g * (2.0f * l) * (0.0f <= l && l <= 0.5f) + (g + (1.0f - g) * (2.0f * l - 1.0f)) * (0.5f < l && l <= 1.0f);
//     b = b * (2.0f * l) * (0.0f <= l && l <= 0.5f) + (b + (1.0f - b) * (2.0f * l - 1.0f)) * (0.5f < l && l <= 1.0f);

//     return (Color) { .r = r * 255, .g = g * 255, .b = b * 255 };
// }

Color Color_from_HL(float h, float l)
{
    float hh = (h + M_PI) * 3.0f * M_1_PI;

    int i = floorf(hh);

    float incr = hh - i;

    i = (6 - (-i % 6)) % 6;

    float r = (i == 3 || i == 2) + (1.0f - incr) * (i == 4) + incr * (i == 1);
    float g = (i == 4 || i == 5) + (1.0f - incr) * (i == 0) + incr * (i == 3);
    float b = (i == 0 || i == 1) + (1.0f - incr) * (i == 2) + incr * (i == 5);

    r = r * (2.0f * l) * (0.0f <= l && l <= 0.5f) + (r + (1.0f - r) * (2.0f * l - 1.0f)) * (0.5f < l && l <= 1.0f);
    g = g * (2.0f * l) * (0.0f <= l && l <= 0.5f) + (g + (1.0f - g) * (2.0f * l - 1.0f)) * (0.5f < l && l <= 1.0f);
    b = b * (2.0f * l) * (0.0f <= l && l <= 0.5f) + (b + (1.0f - b) * (2.0f * l - 1.0f)) * (0.5f < l && l <= 1.0f);

    return (Color) { .r = r * 255, .g = g * 255, .b = b * 255 };
}

typedef struct Picture
{
    unsigned int width;
    unsigned int height;
    unsigned char *pixeldata;
} Picture;

Picture *Picture_new(unsigned int width, unsigned int height)
{
    Picture *ret = malloc(sizeof *ret);
    ret->width = width;
    ret->height = height;
    ret->pixeldata = malloc(ret->width * ret->height * 3 * sizeof *ret->pixeldata);

    for (unsigned i = 0; i < ret->width * ret->height * 3; i++)
        ret->pixeldata[i] = 255;

    return ret;
}

void Picture_delete(Picture **picture)
{
    free((*picture)->pixeldata);

    free(*picture);
    
    *picture = NULL;
}

void Picture_point(Picture *picture, unsigned int x, unsigned int y, Color color)
{
    // Maybe do a check if the point is on the canvas

    picture->pixeldata[(y * picture->width + x) * 3 + 0] = color.r;
    picture->pixeldata[(y * picture->width + x) * 3 + 1] = color.g;
    picture->pixeldata[(y * picture->width + x) * 3 + 2] = color.b;
}

void Picture_rect(Picture *picture, int x, int y, unsigned int width, unsigned int height, Color color)
{
    int startx = (x + abs(x)) / 2, starty = (y + abs(y)) / 2;

    for (unsigned int i = startx; i < width + startx && i < picture->width; i++)
        for (unsigned int j = starty; j < height + starty && j < picture->height; j++)
            Picture_point(picture, i, j, color);
}

void Picture_paint(Picture *picture, const char *destinationpath)
{
    FILE *file = fopen(destinationpath, "wb");

    const unsigned int paddingamount = (4 - picture->width * 3 % 4) % 4, headersize = 14, infoheadersize = 40, pixelarraysize = picture->height * (picture->width * 3 + paddingamount), filesize = headersize + infoheadersize + pixelarraysize;

    char header[] = {
        /* File type */             'B', 'M',
        /* File size */             filesize, filesize >> 8, filesize >> 16, filesize >> 24,
        /* Reserved 1 */            0, 0,
        /* Reserved 2 */            0, 0,
        /* Pixel data offset */     headersize + infoheadersize, 0, 0, 0
    };

    char infoheader[] = {
        /* Header size */           infoheadersize, 0, 0, 0,
        /* Image width */           picture->width, picture->width >> 8, picture->width >> 16, picture->width >> 24,
        /* Image height */          picture->height, picture->height >> 8, picture->height >> 16, picture->height >> 24,
        /* Planes */                1, 0,
        /* Bits per pixel */        24, 0,
        /* Compression */           0, 0, 0, 0,
        /* Image size */            0, 0, 0, 0,
        /* X pixels per meter */    0, 0, 0, 0,
        /* Y pixels per meter */    0, 0, 0, 0,
        /* Total colors */          0, 0, 0, 0,
        /* Important colors */      0, 0, 0, 0
    };

    char *pixelarray = calloc(pixelarraysize, sizeof *pixelarray);

    for (unsigned int y = 0; y < picture->height; y++)
        for (unsigned int x = 0; x < picture->width; x++)
            for (unsigned int i = 0; i < 3; i++)
                pixelarray[(picture->height - 1 - y) * (picture->width * 3 + paddingamount) + x * 3 + 2 - i] = picture->pixeldata[(y * picture->width + x) * 3 + i];

    fwrite(header, sizeof *header, sizeof header / sizeof *header, file);

    fwrite(infoheader, sizeof *infoheader, sizeof infoheader / sizeof *infoheader, file);

    fwrite(pixelarray, sizeof *pixelarray, pixelarraysize / sizeof *pixelarray, file);

    free(pixelarray);

    fclose(file);
}
