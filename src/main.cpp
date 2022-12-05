#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#include "player.c"
#include "apple.c"
#include "bomb.c"

// Function prototypes
void init(void);
void render(void);
void onResize(GLint width, GLint height);
void onMouseAction(GLint button, GLint action, GLint xMouse, GLint yMouse);
void onMouseMove(GLint xMouse, GLint yMouse);
void onKeyPress(unsigned char key, GLint xMouse, GLint yMouse);
void drawSnake(void);
void drawApples(void);
void drawBombs(void);
void logConsole(char *functionName, char *message);

// Constants
const char *TITLE = "Snek - Version 1.0";
const int FPS = 1000 / 60;

// Variables
int width = 1280;
int height = 720;
bool running = true;

Player player;
Bomb bombs[100];
Apple apples[100];
int score;
int bombCount;
int appleCount;

void update(int value)
{
    // Update the player's position.
    player.x += player.xVelocity;
    player.y += player.yVelocity;

    glutPostRedisplay();
    glutTimerFunc(FPS, update, 0);
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawSnake();
    glFlush();
}

/**
 * @brief Called by OpenGL when the window is resized. Updates the viewport and project matrix.
 */
void onResize(GLint newWidth, GLint newHeight)
{
    char logMessage[200];
    sprintf(logMessage, "Resizing window to new dimensions: (%d, %d)", newWidth, newHeight);
    logConsole("onResize", logMessage);

    glViewport(0, 0, newWidth, newHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, GLdouble(newWidth), GLdouble(newHeight), 0.0);
}

/**
 * @brief Called by OpenGL when the mouse has an action occur, such as a click or release.
 */
void onMouseAction(GLint button, GLint action, GLint xMouse, GLint yMouse)
{
    char logMessage[200];
    sprintf(logMessage, "Mouse action occured at (%d, %d)", xMouse, yMouse);
    logConsole("onMouseAction", logMessage);
}

/**
 * @brief Called by OpenGL when the mouse is moved.
 */
void onMouseMove(GLint xMouse, GLint yMouse)
{
    char logMessage[200];
    sprintf(logMessage, "Mouse moved to (%d, %d)", xMouse, yMouse);
    logConsole("onMouseMove", logMessage);
}

/**
 * @brief Called by OpenGL when a keyboard key is pressed.
 */
void onKeyPress(unsigned char key, GLint x, GLint y)
{
    char logMessage[200];
    sprintf(logMessage, "Keyboard key pressed: %c", key);
    logConsole("onKeyPress", logMessage);

    // Escape key
    if (key == 27)
    {
        exit(0);
    }

    if (key == 'w' || key == GLUT_KEY_UP)
    {
        player.yVelocity = 1;
        player.xVelocity = 0;
    }
    if (key == 's' || key == GLUT_KEY_DOWN)
    {
        player.yVelocity = -1;
        player.xVelocity = 0;
    }
    if (key == 'a' || key == GLUT_KEY_LEFT)
    {
        player.xVelocity = -1;
        player.yVelocity = 0;
    }
    if (key == 'd' || key == GLUT_KEY_RIGHT)
    {
        player.xVelocity = 1;
        player.yVelocity = 0;
    }
}

void onSpecialKeyPress(int key, int a, int b)
{
    char logMessage[200];
    sprintf(logMessage, "Special keyboard key pressed: %d", key);
    logConsole("onSpecialKeyPress", logMessage);
}

void drawSnake()
{
    glColor3f(player.color[0], player.color[1], player.color[2]);
    glBegin(GL_POLYGON);
    glVertex2f(player.x, player.y);
    glVertex2f(player.x + player.size, player.y);
    glVertex2f(player.x + player.size, player.y + player.size);
    glVertex2f(player.x, player.y + player.size);
    glEnd();
}

void drawApples() {}

void drawBombs() {}

void logConsole(char *functionName, char *message)
{
    printf("[%s]: %s\n", functionName, message);
}

void init(int argc, char *argv[])
{
    logConsole("init", "Initializing...");
    srand(time(NULL));
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    player.x = 100; // rand() % WIDTH;
    player.y = 100; // rand() % HEIGHT;
    player.xVelocity = 5;
    player.yVelocity = 0;
    player.size = 10;
    player.health = 3;
    player.color[0] = 0.0;
    player.color[1] = 1.0;
    player.color[2] = 0.0;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(width, height);
    glutCreateWindow(TITLE);

    glutDisplayFunc(render);
    glutReshapeFunc(onResize);
    glutMouseFunc(onMouseAction);
    glutMotionFunc(onMouseMove);
    glutKeyboardFunc(onKeyPress);
    glutSpecialFunc(onSpecialKeyPress);
    glutTimerFunc(FPS, update, 0);

    glClearColor(0.6, 0.6, 0.6, 1.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, 300.0, 0.0, 300.0);
    glutMainLoop();
}

int main(int argc, char *argv[])
{
    init(argc, argv);
}
