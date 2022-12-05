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
void onKeyboardAction(unsigned char key, GLint xMouse, GLint yMouse);
void drawSnake(void);
void drawApples(void);
void drawBombs(void);
void logConsole(char *functionName, char *message);

// Constants
const int WIDTH = 1280;
const int HEIGHT = 720;
const char *TITLE = "Snek - Version 1.0";

// Variables
Player player;
Bomb bombs[100];
Apple apples[100];
int score;
int bombCount;
int appleCount;

int main(int argc, char *argv[])
{
    init(argc, argv);
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
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow(TITLE);

    glutDisplayFunc(render);
    glutReshapeFunc(onResize);
    glutMouseFunc(onMouseAction);
    glutMotionFunc(onMouseMove);
    glutKeyboardFunc(onKeyboardAction);

    glClearColor(0.6, 0.6, 0.6, 1.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, 300.0, 0.0, 300.0);
    glutMainLoop();

    // point.push_back(make_pair(bodyX - 1, bodyY));
    // point.push_back(make_pair(bodyX - 2, bodyY));

    // srand(time(NULL)); // execute initialization procedure
    // newFood();

    // glutTimerFunc(200, update, 0);
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Update the player's position.
    player.x += player.xVelocity;
    player.y += player.yVelocity;

    glFlush();
}

/**
 * @brief Called by OpenGL when the window is resized. Updates the viewport and project matrix.
 */
void onResize(GLint newWidth, GLint newHeight)
{
    char *logMessage;
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
    char *logMessage;
    sprintf(logMessage, "Mouse action occured at (%d, %d)", xMouse, yMouse);
    logConsole("onMouseAction", logMessage);
}

/**
 * @brief Called by OpenGL when the mouse is moved.
 */
void onMouseMove(GLint xMouse, GLint yMouse)
{
    char *logMessage;
    sprintf(logMessage, "Mouse moved to (%d, %d)", xMouse, yMouse);
    logConsole("onMouseMove", logMessage);
}

/**
 * @brief Called by OpenGL when a keyboard key is pressed.
 */
void onKeyboardAction(unsigned char key, GLint x, GLint y)
{
    char *logMessage;
    sprintf(logMessage, "Keyboard key pressed: %c", key);
    logConsole("onKeyboardAction", logMessage);

    if (key == 'w')
    {
        player.yVelocity = 1;
        player.xVelocity = 0;
    }
    if (key == 's')
    {
        player.yVelocity = -1;
        player.xVelocity = 0;
    }
    if (key == 'a')
    {
        player.xVelocity = -1;
        player.yVelocity = 0;
    }
    if (key == 'd')
    {
        player.xVelocity = 1;
        player.yVelocity = 0;
    }
}

void drawSnake() {}

void drawApples() {}

void drawBombs() {}

void logConsole(char *functionName, char *message)
{
    printf("[%s]: %s\n", functionName, message);
}
