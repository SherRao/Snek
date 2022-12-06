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
void checkCollisions(void);
void drawSnake(void);
void drawApples(void);
void drawBombs(void);
void drawData(void);
void generateApples(void);
void generateBombs(void);
void endGame(void);
void logConsole(char *functionName, char *message);

// Constants
const char *TITLE = "Snek - Version 1.0";
const int FPS = 1000 / 60;
const int APPLE_SIZE = 25;
const int BOMB_SIZE = 50;

// Variables
int width = 1280;
int height = 720;
bool running = true;
int gameState = 0; // 0 = MENU, 1 = GAME, 2 = PAUSE, 3 = GAME OVER

Player player;
Bomb bombs[10000];
Apple apples[10000];
int score = 0;
int level = 1;
int bombCount = 0;
int appleCount = 0;

/**
 * @brief Called 60 times per second - used to update the game state.
 *
 * @param value The value passed to the timer function.
 */
void update(int value)
{
    if (gameState == 0)
    {
    }
    else if (gameState == 1)
    {
        // Update the player's position.
        player.x += player.xVelocity;
        player.y += player.yVelocity;
        if (player.size <= 0)
        {
            gameState = 3;
        }

        // Check if apples are all eaten.
        if (appleCount <= 0)
        {
            level++;
            generateApples();
            generateBombs();
        }

        checkCollisions();
    }

    glutPostRedisplay();
    glutTimerFunc(FPS, update, 0);
}

/**
 * @brief Called 60 times per second - used to draw the game.
 */
void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (gameState == 0)
    {
        glColor3f(0.0, 0.0, 0.0);
        glBegin(GL_QUADS);
        glVertex2f(width / 2 - 200, height / 2 - 50);
        glVertex2f(width / 2 + 200, height / 2 - 50);
        glVertex2f(width / 2 + 200, height / 2 + 50);
        glVertex2f(width / 2 - 200, height / 2 + 50);
        glVertex2f(width / 2 - 200, height / 2 + 150 - 50);
        glVertex2f(width / 2 + 200, height / 2 + 150 - 50);
        glVertex2f(width / 2 + 200, height / 2 + 150 + 50);
        glVertex2f(width / 2 - 200, height / 2 + 150 + 50);
        glEnd();

        // Write the word "Play" inside the first rectangle
        glColor3f(1.0, 1.0, 1.0);
        glRasterPos2f(width / 2 - 50, height / 2 - 20);
        char *string = "Play";
        for (int i = 0; i < strlen(string); i++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);

        // Write the word "Quit" inside the second rectangle
        glColor3f(1.0, 1.0, 1.0);
        glRasterPos2f(width / 2 - 50, height / 2 + 150 - 20);
        string = "Quit";
        for (int i = 0; i < strlen(string); i++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
    }
    else if (gameState == 1)
    {
        drawSnake();
        drawApples();
        drawBombs();
        drawData();
    }
    else if (gameState == 3)
    {
        // draw game over in the center of the screen
        glColor3f(1.0, 0.0, 0.0);
        glRasterPos2f(width / 2 - 100, height / 2);
        char *string = "Game Over";
        for (int i = 0; i < strlen(string); i++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
    }

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

    if (gameState == 0)
    {
        if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN)
        {
            if (xMouse >= width / 2 - 200 && xMouse <= width / 2 + 200 && yMouse >= height / 2 - 50 && yMouse <= height / 2 + 50)
            {
                gameState = 1;
            }
            else if (xMouse >= width / 2 - 200 && xMouse <= width / 2 + 200 && yMouse >= height / 2 + 150 - 50 && yMouse <= height / 2 + 150 + 50)
            {
                exit(0);
            }
        }
    }
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

    if (gameState == 0)
    {
    }
    else
    {
        if (key == 'w' || key == GLUT_KEY_UP)
        {
            player.yVelocity = -player.speed;
            player.xVelocity = 0;
        }
        if (key == 's' || key == GLUT_KEY_DOWN)
        {
            player.yVelocity = player.speed;
            player.xVelocity = 0;
        }
        if (key == 'a' || key == GLUT_KEY_LEFT)
        {
            player.xVelocity = -player.speed;
            player.yVelocity = 0;
        }
        if (key == 'd' || key == GLUT_KEY_RIGHT)
        {
            player.xVelocity = player.speed;
            player.yVelocity = 0;
        }
    }
}

/**
 * @brief Called by OpenGL when a special keyboard key is pressed.
 */
void onSpecialKeyPress(int key, int a, int b)
{
    char logMessage[200];
    sprintf(logMessage, "Special keyboard key pressed: %d", key);
    logConsole("onSpecialKeyPress", logMessage);
}

/**
 * @brief Checks for collisions between the player, environment, and apples/bombs.
 */
void checkCollisions()
{
    for (int i = 0; i < appleCount; i++)
    {

        if (player.x + player.size >= apples[i].x && player.x <= apples[i].x + APPLE_SIZE)
        {
            if (player.y + player.size >= apples[i].y && player.y <= apples[i].y + APPLE_SIZE)
            {
                logConsole("checkCollisions", "Player hit apple!");
                score++;
                appleCount--;
                player.size += 10;
                break;
            }
        }
    }

    for (int i = 0; i < bombCount; i++)
    {
        if (player.x + player.size >= bombs[i].x && player.x <= bombs[i].x + BOMB_SIZE)
        {
            if (player.y + player.size >= bombs[i].y && player.y <= bombs[i].y + BOMB_SIZE)
            {
                logConsole("checkCollisions", "Player hit bomb!");
                player.health--;
                bombCount--;
                player.size -= 10;
                break;
            }
        }
    }

    if (player.x > width)
        player.x = 0;

    if (player.x < 0)
        player.x = width;

    if (player.y > height)
        player.y = 0;

    if (player.y < 0)
        player.y = height;
}

/**
 * @brief Draws the snake to the screen.
 */
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

/**
 * @brief Draws the apples to the screen.
 */
void drawApples()
{
    for (int i = 0; i < appleCount; i++)
    {
        glColor3d(1.0, 1.0, 0.0);
        glColor3f(apples[i].color[0], apples[i].color[1], apples[i].color[2]);
        glBegin(GL_POLYGON);
        glVertex2f(apples[i].x, apples[i].y);
        glVertex2f(apples[i].x + APPLE_SIZE, apples[i].y);
        glVertex2f(apples[i].x + APPLE_SIZE, apples[i].y + APPLE_SIZE);
        glVertex2f(apples[i].x, apples[i].y + APPLE_SIZE);
        glEnd();
    }
}

/**
 * @brief Draws the bombs to the screen.
 */
void drawBombs()
{
    glColor3d(.5, .5, 0.0);
    for (int i = 0; i < bombCount; i++)
    {
        glColor3f(bombs[i].color[0], bombs[i].color[1], bombs[i].color[2]);
        glBegin(GL_POLYGON);
        glVertex2f(bombs[i].x, bombs[i].y);
        glVertex2f(bombs[i].x + BOMB_SIZE, bombs[i].y);
        glVertex2f(bombs[i].x + BOMB_SIZE, bombs[i].y + BOMB_SIZE);
        glVertex2f(bombs[i].x, bombs[i].y + BOMB_SIZE);
        glEnd();
    }
}

/**
 * @brief Draws the score to the screen in Times New Roman size 24.
 */
void drawData()
{
    glColor3f(0.0f, 0.0f, 1.0f);
    glRasterPos2f(10, 30);
    char scoreString[25];
    sprintf(scoreString, "Score: %d (Level %d)", score, level);
    for (int i = 0; i < strlen(scoreString); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, scoreString[i]);

    glColor3f(0.0f, 1.0f, 0.0f);
    glRasterPos2f(10, 60);
    char healthString[12];
    sprintf(healthString, "Health: %d", player.size);
    for (int i = 0; i < strlen(healthString); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, healthString[i]);
}

/**
 * @brief
 */
void generateApples()
{
    appleCount = level * 2;
    // for (int i = 0; i < appleCount; i++)
    // {
    //     apples[i].x = rand() % (width - 50);
    //     apples[i].y = rand() % (height - 50);
    // }

    // generate apples only in the area of the sceeen the player isnt in
    if (player.x > width / 2)
    {
        for (int i = 0; i < appleCount; i++)
        {
            apples[i].x = rand() % (width / 2);
            apples[i].y = rand() % (height - 50);
        }
    }
    else
    {
        for (int i = 0; i < appleCount; i++)
        {
            apples[i].x = rand() % (width / 2) + (width / 2);
            apples[i].y = rand() % (height - 50);
        }
    }
}

/**
 * @brief x
 */
void generateBombs()
{
    bombCount = pow(2, level);
    // for (int i = 0; i < bombCount; i++)
    // {
    //     bombs[i].x = rand() % (width - 50);
    //     bombs[i].y = rand() % (height - 50);
    // }

    // generate bombs only in the area of the sceeen the player isnt in
    if (player.x > width / 2)
    {
        for (int i = 0; i < bombCount; i++)
        {
            bombs[i].x = rand() % (width / 2);
            bombs[i].y = rand() % (height - 50);
        }
    }
    else
    {
        for (int i = 0; i < bombCount; i++)
        {
            bombs[i].x = rand() % (width / 2) + (width / 2);
            bombs[i].y = rand() % (height - 50);
        }
    }
}

/**
 * @brief x
 */
void endGame()
{
    char logMessage[200];
    sprintf(logMessage, "Game over! Score: %d", score);
    logConsole("endGame", logMessage);
    exit(0);
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
 * @brief Initializes the game.
 */
void init(int argc, char *argv[])
{
    logConsole("init", "Initializing...");
    srand(time(NULL));
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    generateBombs();
    generateApples();
    player.x = width / 2;
    player.y = height / 2;
    player.xVelocity = player.speed = 10;
    player.yVelocity = 0;
    player.size = 50;
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

/**
 * @brief The main function.
 */
int main(int argc, char *argv[])
{
    init(argc, argv);
}
