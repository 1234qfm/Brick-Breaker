
#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <ctime>

const int windowWidth = 800;
const int windowHeight = 600;

struct Paddle {
    float x = 350;
    float y = 20;
    float width = 100;
    float height = 20;
    float speed = 10;
} paddle;

struct Ball {
    float x = 400;
    float y = 300;
    float radius = 10;
    float dx = 4;
    float dy = 4;
} ball;

struct Brick {
    float x, y;
    float width = 60;
    float height = 20;
    int hitsRemaining;
    bool isVisible;
};

std::vector<Brick> bricks;

void initBricks() {
    bricks.clear();
    int rows = 5;
    int cols = 10;
    float startX = 60;
    float startY = 500;
    float gap = 5;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Brick b;
            b.x = startX + j * (b.width + gap);
            b.y = startY - i * (b.height + gap);
            b.hitsRemaining = (i % 2 == 0) ? 1 : 2;
            b.isVisible = true;
            bricks.push_back(b);
        }
    }
}

void drawRect(float x, float y, float width, float height, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void drawCircle(float x, float y, float radius) {
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 100; i++) {
        float angle = 2.0f * 3.14159f * i / 100;
        glVertex2f(x + (float)cos(angle) * radius, y + (float)sin(angle) * radius);
    }
    glEnd();
}

void drawPaddle() {
    drawRect(paddle.x, paddle.y, paddle.width, paddle.height, 0.2f, 0.6f, 1.0f);
}

void drawBall() {
    drawCircle(ball.x, ball.y, ball.radius);
}

void drawBricks() {
    for (auto &brick : bricks) {
        if (!brick.isVisible) continue;
        if (brick.hitsRemaining == 2)
            drawRect(brick.x, brick.y, brick.width, brick.height, 1.0f, 0.4f, 0.4f); // Red
        else
            drawRect(brick.x, brick.y, brick.width, brick.height, 0.8f, 0.8f, 0.3f); // Yellow
    }
}

void updateBall() {
    ball.x += ball.dx;
    ball.y += ball.dy;

    // Wall collisions
    if (ball.x - ball.radius < 0 || ball.x + ball.radius > windowWidth)
        ball.dx *= -1;
    if (ball.y + ball.radius > windowHeight)
        ball.dy *= -1;

    // Bottom collision
    if (ball.y - ball.radius < 0) {
        ball.x = 400;
        ball.y = 300;
        ball.dx = 4;
        ball.dy = 4;
    }

    // Paddle collision
    if (ball.y - ball.radius <= paddle.y + paddle.height &&
        ball.x >= paddle.x && ball.x <= paddle.x + paddle.width) {
        ball.dy *= -1;
        float hitPos = (ball.x - paddle.x) / paddle.width - 0.5f;
        ball.dx = hitPos * 8;
    }

    // Brick collisions
    for (auto &brick : bricks) {
        if (!brick.isVisible) continue;
        if (ball.x + ball.radius > brick.x &&
            ball.x - ball.radius < brick.x + brick.width &&
            ball.y + ball.radius > brick.y &&
            ball.y - ball.radius < brick.y + brick.height) {
            ball.dy *= -1;
            brick.hitsRemaining--;
            if (brick.hitsRemaining <= 0)
                brick.isVisible = false;
            break;
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawPaddle();
    drawBall();
    drawBricks();
    glutSwapBuffers();
}

void update(int value) {
    updateBall();
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0);
}

void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            if (paddle.x > 0)
                paddle.x -= paddle.speed;
            break;
        case GLUT_KEY_RIGHT:
            if (paddle.x + paddle.width < windowWidth)
                paddle.x += paddle.speed;
            break;
    }
    glutPostRedisplay();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    srand(time(0));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Brick Breaker - GLUT");

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    initBricks();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}
