#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include "stb_image.h"

/**
 * @brief A struct that represents an image.
 */
typedef struct image
{
    unsigned long width;
    unsigned long height;
    char *data;
} Image;

void logConsole(char *functionName, char *message);
void drawText(char *text, GLint x, GLint y, GLfloat r, GLfloat g, GLfloat b);
GLuint loadImage(char *fileName, GLint *imageWidth, GLint *imageHeight);
void drawImage(GLuint texture, GLint x, GLint y, GLint width, GLint height);

Image *loadTexture(char *filename);
int ImageLoad(char *filename, Image *image);
GLuint createTexture(Image *image);
Image *loadTexture(char *filename)
{
    Image *image = (Image *)malloc(sizeof(Image));
    if (image == NULL)
    {
        printf("Error allocating space for image");
        exit(0);
    }
    if (!ImageLoad(filename, image))
    {
        exit(1);
    }
    return image;
}

int ImageLoad(char *filename, Image *image)
{
    FILE *file;
    unsigned long size;        // size of the image in bytes.
    unsigned long i;           // standard counter.
    unsigned short int planes; // number of planes in image (must be 1)
    unsigned short int bpp;    // number of bits per pixel (must be 24)

    char temp; // temporary color storage for bgr-rgb conversion.

    // make sure the file is there.
    if ((file = fopen(filename, "rb")) == NULL)
    {
        printf("File Not Found : %s\n", filename);
        return 0;
    }

    // seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);

    // read the width
    if ((i = fread(&image->width, 4, 1, file)) != 1)
    {
        printf("Error reading width from %s.\n", filename);
        return 0;
    }

    // read the height
    if ((i = fread(&image->height, 4, 1, file)) != 1)
    {
        printf("Error reading height from %s.\n", filename);
        return 0;
    }

    size = image->width * image->height * 3;

    if ((fread(&planes, 2, 1, file)) != 1)
    {
        printf("Error reading planes from %s.\n", filename);
        return 0;
    }

    if (planes != 1)
    {
        printf("Planes from %s is not 1: %u\n", filename, planes);
        return 0;
    }

    // read the bitsperpixel
    if ((i = fread(&bpp, 2, 1, file)) != 1)
    {
        printf("Error reading bpp from %s.\n", filename);
        return 0;
    }

    if (bpp != 24)
    {
        printf("Bpp from %s is not 24: %u\n", filename, bpp);
        return 0;
    }

    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);

    image->data = (char *)malloc(size);
    if (image->data == NULL)
    {
        printf("Error allocating memory for color-corrected image data");
        return 0;
    }

    if ((i = fread(image->data, size, 1, file)) != 1)
    {
        printf("Error reading image data from %s.\n", filename);
        return 0;
    }

    for (i = 0; i < size; i += 3)
    { // reverse all of the colors. (bgr -> rgb)
        temp = image->data[i];
        image->data[i] = image->data[i + 2];
        image->data[i + 2] = temp;
    }
    return 1;
}

GLuint createTexture(Image *image)
{
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);
    return textureId;
}

/**
 * @brief Logs a message to the console.
 *
 * @param functionName The name of the function that called this function.
 * @param message The message to log.
 */
void logConsole(char *functionName, char *message)
{
    printf("[%s]: %s\n", functionName, message);
}

/**
 * @brief Draws a given string of text to the screen.
 *
 * @param text The text to draw.
 * @param x The x position of the text.
 * @param y The y position of the text.
 * @param r The red value of the text.
 * @param g The green value of the text.
 * @param b The blue value of the text.
 */
void drawText(char *text, GLint x, GLint y, GLfloat r, GLfloat g, GLfloat b)
{
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (int i = 0; i < strlen(text); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
}

/**
 * @brief x
 *
 * @param fileName The name of the file to load.
 * @param imageWidth The width of the image.
 * @param imageHeight The height of the image.
 * @return unsigned int The texture ID.
 */
GLuint loadImage(char *fileName, GLint *imageWidth, GLint *imageHeight)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLint width;
    GLint height;
    GLint nrChannels;
    unsigned char *data = stbi_load(fileName, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        logConsole("loadImage", "Failed to load texture");
    }

    stbi_image_free(data);
    *imageWidth = width;
    *imageHeight = height;
    return texture;
}

/**
 * @brief Draws an image to the screen with the given texture.
 *
 * @param texture The texture to draw.
 * @param x The x position of the image.
 * @param y The y position of the image.
 * @param width The width of the image.
 * @param height The height of the image.
 */
void drawImage(GLuint texture, GLint x, GLint y, GLint width, GLint height)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(x, y);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(x + width, y);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(x + width, y + height);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(x, y + height);

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}
