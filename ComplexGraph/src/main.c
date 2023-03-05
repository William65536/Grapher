#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <complex.h>

#include "picture.h"
#include "parser.h" /* `parser.h' is terrible AF. You had better fix it. */

float complex cfloorf(float complex z)
{
    return floorf(crealf(z)) + I * floorf(cimagf(z));
}

float complex cmodf(float complex a, float complex b)
{
    return a - b * cfloorf(a / b);
}

complex cfloor(complex z)
{
    return floor(creal(z)) + I * floor(cimag(z));
}

complex cmod(complex a, complex b)
{
    return a - b * cfloor(a / b);
}

// int main(void)
// {
//     unsigned int width = 1280, height = 720;

//     Picture picture = Picture_new(width, height);

//     float num_px_per_x_unit = 100, num_px_per_y_unit = 100;

//     float complex z, value;

//     // Node *f = parser(stdin);

//     // printf("%f + I * %f", creal(eval(f, M_PI_2)), cimag(eval(f, M_PI_2)));

//     for (unsigned int x = 0; x < width; x++) {
//         for (unsigned int y = 0; y < height; y++) {
//             z = ((x - width * 0.5f) / num_px_per_x_unit) + I * (-(y - height * 0.5f) / num_px_per_y_unit);

//             // value = cmodf(cabsf(z), csinf(z));
//             // value = cmodf(1.0f, csinf(z));

//             // value = eval(f, z);

//             Picture_point(picture, x, y, Color_from_HL(cargf(value), 1.0f - 1.0f / (1.0f + cabsf(value))));
//             // Picture_point(picture, x, y, Color_from_HL(cargf(value), 0.5f));
//         }
//     }

//     Picture_paint(picture, "test.bmp");

//     Picture_delete(&picture);
// }


int main(void)
{
    unsigned int width = 1280, height = 720;

    Picture picture = Picture_new(width, height);

    double num_px_per_x_unit = 100.0, num_px_per_y_unit = 100.0;

    complex z, value;

    printf("Enter in an expression: ");

    Node *f = parser(stdin);

    for (unsigned int x = 0; x < width; x++) {
        for (unsigned int y = 0; y < height; y++) {
            z = ((x - width * 0.5) / num_px_per_x_unit) + I * (-(y - height * 0.5) / num_px_per_y_unit);

            // value = cmod(1.0, csin(z));

            // tan(exp(Z) + sin(2 * Z))

            // value = ctan(cexp(z) + csin(2 * z));

            value = eval(f, z);

            Picture_point(picture, x, y, Color_from_HL(carg(value), 1.0 - 1.0 / (1.0 + cabs(value))));
        }
    }

    Picture_paint(picture, "bro.bmp");

    Picture_delete(&picture);
}
