#include <GL/glut.h>
#include <ctime>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

float ballX = 0.0f, ballY = 0.0f;
float ballSpeedX = 0.015f, ballSpeedY = 0.015f;

float leftPaddleY = 0.0f, rightPaddleY = 0.0f;
const float paddleHeight = 0.3f;

int scoreLeft = 0, scoreRight = 0;

bool powerShotLeft = false, powerShotRight = false;

const float bgR = 0.0f, bgG = 0.0f, bgB = 0.2f; // Dark blue background
float ballR = 1.0f, ballG = 1.0f, ballB = 1.0f; // White ball

void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    while (*text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
}

void drawPaddle(float x, float y, bool powered) {
    if (powered) glColor3f(1.0f, 0.5f, 0.0f); // Orange (power shot)
    else glColor3f(0.0f, 1.0f, 0.0f);         // Green (normal)

    glBegin(GL_POLYGON);
    glVertex2f(x - 0.02f, y - paddleHeight / 2);
    glVertex2f(x + 0.02f, y - paddleHeight / 2);
    glVertex2f(x + 0.02f, y + paddleHeight / 2);
    glVertex2f(x - 0.02f, y + paddleHeight / 2);
    glEnd();
}

void drawBall() {
    glColor3f(ballR, ballG, ballB);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; ++i) {
        float angle = i * 2.0f * 3.14159f / 100;
        glVertex2f(ballX + 0.02f * cos(angle), ballY + 0.02f * sin(angle));
    }
    glEnd();
}

void display() {
    glClearColor(bgR, bgG, bgB, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    drawPaddle(-0.95f, leftPaddleY, powerShotLeft);
    drawPaddle(0.95f, rightPaddleY, powerShotRight);
    drawBall();

    char scoreText[32];
    sprintf(scoreText, "%d : %d", scoreLeft, scoreRight);
    glColor3f(1.0f, 1.0f, 1.0f); // White text
    drawText(-0.05f, 0.9f, scoreText);

    glutSwapBuffers();
}

void update(int value) {
    ballX += ballSpeedX;
    ballY += ballSpeedY;

    // Bounce off top/bottom walls
    if (ballY > 1.0f || ballY < -1.0f)
        ballSpeedY *= -1;

    // Collision with left paddle
    if (ballX < -0.93f && ballX > -0.97f &&
        ballY > leftPaddleY - paddleHeight / 2 &&
        ballY < leftPaddleY + paddleHeight / 2) {
        ballSpeedX *= -1;
        PlaySound("hit.wav", NULL, SND_ASYNC);

        if (powerShotLeft) {
            ballSpeedX *= 1.5f;
            powerShotLeft = false;
        }
    }

    // Collision with right paddle
    if (ballX > 0.93f && ballX < 0.97f &&
        ballY > rightPaddleY - paddleHeight / 2 &&
        ballY < rightPaddleY + paddleHeight / 2) {
        ballSpeedX *= -1;
        PlaySound("hit.wav", NULL, SND_ASYNC);

        if (powerShotRight) {
            ballSpeedX *= 1.5f;
            powerShotRight = false;
        }
    }

    // Score for left player
    if (ballX > 1.0f) {
        scoreLeft++;
        PlaySound("score.wav", NULL, SND_ASYNC);
        ballX = ballY = 0;
        ballSpeedX = -0.015f;
    }

    // Score for right player
    if (ballX < -1.0f) {
        scoreRight++;
        PlaySound("score.wav", NULL, SND_ASYNC);
        ballX = ballY = 0;
        ballSpeedX = 0.015f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 'w': leftPaddleY += 0.05f; break;
        case 's': leftPaddleY -= 0.05f; break;
        case 'f':
            powerShotLeft = true;
            PlaySound("power.wav", NULL, SND_ASYNC);
            break;
        case 'j':
            powerShotRight = true;
            PlaySound("power.wav", NULL, SND_ASYNC);
            break;
    }
}

void specialInput(int key, int, int) {
    switch (key) {
        case GLUT_KEY_UP: rightPaddleY += 0.05f; break;
        case GLUT_KEY_DOWN: rightPaddleY -= 0.05f; break;
    }
}

int main(int argc, char** argv) {
    srand(static_cast<unsigned>(time(0)));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Colorful Ping Pong - OpenGL");

    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-1, 1, -1, 1);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialInput);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    return 0;
}
