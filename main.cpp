#include <GL/freeglut.h>
#include <cmath>

float camX = 0, camY = 1.8f, camZ = 5;
float yaw = -90.0f;
float pitch = 0.0f;

float dirX, dirY, dirZ;

float speed = 0.05f;
float mouseSensitivity = 0.15f;

bool keys[256] = { false };

int windowWidth = 900, windowHeight = 600;
bool warpMouse = false;

// Atualiza vetor de direção
void updateCameraDirection() {
    float radYaw = yaw * M_PI / 180.0f;
    float radPitch = pitch * M_PI / 180.0f;

    dirX = cos(radYaw) * cos(radPitch);
    dirY = sin(radPitch);
    dirZ = sin(radYaw) * cos(radPitch);
}

// Desenha cubo
void drawCube() {
    glColor3f(1.0, 0.5, 0.2);
    glutSolidCube(1.0);
}


// =====================================================
//  FUNÇÕES AUXILIARES DE CONSTRUÇÃO 3D
// =====================================================

// Desenha um paralelepípedo (cubo escalado)
void drawBox(float w, float h, float d) {
    glPushMatrix();
    glScalef(w, h, d);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// -----------------------------------------------------
// 1) PAREDE
// -----------------------------------------------------
// wallLength = largura da parede (horizontal)
// wallHeight = altura
// thickness  = espessura
// (x, y, z) = posição do centro da parede
// rotY = rotação para posicionar a parede
void drawWall(float wallLength, float wallHeight, float thickness,
              float x, float y, float z, float rotY = 0.0f,
              float r=0.8f, float g=0.8f, float b=0.8f) {

    glColor3f(r, g, b);
    glPushMatrix();
    glTranslatef(x, y + wallHeight/2.0f, z);
    glRotatef(rotY, 0, 1, 0);
    drawBox(wallLength, wallHeight, thickness);
    glPopMatrix();
}

// -----------------------------------------------------
// 2) JANELA (moldura)
// -----------------------------------------------------
void drawWindow(float width, float height, float frameThickness,
                float x, float y, float z, float rotY = 0.0f,
                float r=0.5f, float g=0.5f, float b=0.6f) {

    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0, 1, 0);

    glColor3f(r, g, b);

    // Moldura horizontal superior
    drawBox(width, frameThickness, frameThickness);

    // Moldura horizontal inferior
    glPushMatrix();
    glTranslatef(0, -height + frameThickness, 0);
    drawBox(width, frameThickness, frameThickness);
    glPopMatrix();

    // Lateral esquerda
    glPushMatrix();
    glTranslatef(-width/2 + frameThickness/2, -height/2, 0);
    drawBox(frameThickness, height, frameThickness);
    glPopMatrix();

    // Lateral direita
    glPushMatrix();
    glTranslatef(width/2 - frameThickness/2, -height/2, 0);
    drawBox(frameThickness, height, frameThickness);
    glPopMatrix();

    glPopMatrix();
}

// -----------------------------------------------------
// 3) PILAR
// -----------------------------------------------------
void drawPillar(float size, float height,
                float x, float y, float z,
                float r=0.7f, float g=0.7f, float b=0.7f) {

    glPushMatrix();
    glColor3f(r, g, b);
    glTranslatef(x, y + height/2.0f, z);
    drawBox(size, height, size);
    glPopMatrix();
}

// -----------------------------------------------------
// 4) CHÃO / PISO
// -----------------------------------------------------
void drawFloor(float width, float depth,
               float x, float y, float z,
               float r=0.3f, float g=0.3f, float b=0.3f) {

    glPushMatrix();
    glColor3f(r, g, b);
    glTranslatef(x, y, z);
    drawBox(width, 0.1f, depth);
    glPopMatrix();
}



// Processa teclas continuamente
void processMovement() {
    float rightX = -dirZ;
    float rightZ = dirX;

    if (keys['w']) {
        camX += dirX * speed;
        camY += dirY * speed;
        camZ += dirZ * speed;
    }
    if (keys['s']) {
        camX -= dirX * speed;
        camY -= dirY * speed;
        camZ -= dirZ * speed;
    }
    if (keys['a']) {
        camX -= rightX * speed;
        camZ -= rightZ * speed;
    }
    if (keys['d']) {
        camX += rightX * speed;
        camZ += rightZ * speed;
    }
    if (keys[' ']) {
        camY += speed;
    }
    if (keys['q']) {
        camY -= speed;
    }
}

void drawScene() {
    // CHÃO
    drawFloor(20, 20, 0, 0, 0);

    // PAREDES DO QUARTO
    drawWall(20, 3, 0.2,  0, 0, -10);
    drawWall(20, 3, 0.2,  0, 0,  10);
    drawWall(20, 3, 0.2, -10, 0, 0, 90);
    drawWall(20, 3, 0.2,  10, 0, 0, 90);

    // PILARES NOS CANTOS
    drawPillar(0.5, 3, -10, 0, -10);
    drawPillar(0.5, 3, -10, 0,  10);
    drawPillar(0.5, 3,  10, 0, -10);
    drawPillar(0.5, 3,  10, 0,  10);

    // JANELA NA PAREDE
    drawWindow(3.0f, 2.0f, 0.2f,
               0, 1.5f, -10 + 0.15f, 0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // FPS CAMERA
    gluLookAt(camX, camY, camZ,
              camX + dirX, camY + dirY, camZ + dirZ,
              0, 1, 0);

    drawScene();

    glutSwapBuffers();
}


void idle() {
    processMovement();
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    keys[key] = true;

    if (key == 27) exit(0); // ESC
}

void keyboardUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

// Mouse para rotação FPS
void passiveMotion(int x, int y) {
    if (warpMouse) {  
        warpMouse = false;
        return;
    }

    int centerX = windowWidth / 2;
    int centerY = windowHeight / 2;

    int dx = x - centerX;
    int dy = centerY - y;

    yaw   += dx * mouseSensitivity;
    pitch += dy * mouseSensitivity;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    updateCameraDirection();

    warpMouse = true;
    glutWarpPointer(centerX, centerY);
}

void reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / h, 0.1, 200.0);

    glMatrixMode(GL_MODELVIEW);
}

void init() {
    glClearColor(0.1, 0.1, 0.1, 1);
    glEnable(GL_DEPTH_TEST);

    updateCameraDirection();

    glutSetCursor(GLUT_CURSOR_NONE);  
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("trabalho CG bloco A");

    init();

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutReshapeFunc(reshape);

    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);

    glutPassiveMotionFunc(passiveMotion);

    glutWarpPointer(windowWidth / 2, windowHeight / 2);

    glutMainLoop();
    return 0;
}

