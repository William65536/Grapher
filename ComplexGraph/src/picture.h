typedef struct Color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

Color Color_from_HL(float h, float l);

typedef struct Picture *Picture;

Picture Picture_new(unsigned int width, unsigned int height);

void Picture_delete(Picture *picture);

void Picture_point(Picture picture, unsigned int x, unsigned int y, Color color);

void Picture_paint(Picture picture, const char *destinationpath);
