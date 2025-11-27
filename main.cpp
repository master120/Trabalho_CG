#include <GL/freeglut.h>
#include <cmath>

float camX = 0, camY = 1.8f, camZ = 5;
float yaw = -90.0f;
float pitch = 0.0f;

float dirX, dirY, dirZ;

float speed = 0.60f;
float mouseSensitivity = 0.15f;

bool keys[256] = { false };

int windowWidth = 900, windowHeight = 600;
bool warpMouse = false;



void drawCube(float x, float y, float z, float size, float r, float g, float b) {
    glPushMatrix();
    glColor3f(r, g, b);
    glTranslatef(x, y, z);
    glutSolidCube(size);
    glPopMatrix();
}

void drawCylinder(float x, float y, float z, float radius, float height, float r, float g, float b) {
    glPushMatrix();
    glColor3f(r, g, b);
    glTranslatef(x, y, z);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, radius, radius, height, 20, 20);
    gluDisk(q, 0, radius, 20, 20);
    glTranslatef(0, 0, height);
    gluDisk(q, 0, radius, 20, 20);
    gluDeleteQuadric(q);
    glPopMatrix();
}

void drawTree(float x, float y, float z) {
    // tronco
    drawCylinder(x, y, z, 0.2f, 1.2f, 0.4f, 0.2f, 0.1f);
    // copa
    glPushMatrix();
    glColor3f(0.0f, 0.6f, 0.1f);
    glTranslatef(x, y + 1.2f, z);
    glutSolidSphere(0.8f, 20, 20);
    glPopMatrix();
}

void drawBush(float x, float y, float z) {
    glPushMatrix();
    glColor3f(0.1f, 0.7f, 0.2f);
    glTranslatef(x, y, z);
    glutSolidSphere(0.4f, 20, 20);
    
    glPopMatrix();
}

//atualiza vetor de dire??o
void updateCameraDirection() {
    float radYaw = yaw * M_PI / 180.0f;
    float radPitch = pitch * M_PI / 180.0f;

    dirX = cos(radYaw) * cos(radPitch);
    dirY = sin(radPitch);
    dirZ = sin(radYaw) * cos(radPitch);
}

// Desenha um telhado em empena (duas �guas)
// Par�metros:
// width: largura total do telhado (eixo X)
// depth: comprimento do telhado (eixo Z)
// height: altura do beiral at� o �pice (eleva��o da cumeeira)
// x,y,z: posi��o da base do telhado (y = topo das paredes onde o telhado se assenta)
// rotY: rota��o em torno do eixo Y (graus)
// r,g,b: cor do telhado
void drawRoof(float width, float depth, float height,
              float x = 0.0f, float y = 0.0f, float z = 0.0f,
              float rotY = 0.0f,
              float r = 0.6f, float g = 0.2f, float b = 0.0f) {
    float halfW = width / 2.0f;
    float halfD = depth / 2.0f;

    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    // Faces do telhado (duas �guas)
    glColor3f(r, g, b);

    // Lateral esquerda (quad)
    glBegin(GL_QUADS);
        glVertex3f(-halfW, 0.0f, -halfD);
        glVertex3f(0.0f, height, -halfD);
        glVertex3f(0.0f, height, halfD);
        glVertex3f(-halfW, 0.0f, halfD);
    glEnd();

    // Lateral direita (quad)
    glBegin(GL_QUADS);
        glVertex3f(halfW, 0.0f, -halfD);
        glVertex3f(0.0f, height, -halfD);
        glVertex3f(0.0f, height, halfD);
        glVertex3f(halfW, 0.0f, halfD);
    glEnd();

    // Empenas frontais (tri�ngulos)
    glBegin(GL_TRIANGLES);
        glVertex3f(-halfW, 0.0f, -halfD);
        glVertex3f(halfW, 0.0f, -halfD);
        glVertex3f(0.0f, height, -halfD);
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex3f(-halfW, 0.0f, halfD);
        glVertex3f(halfW, 0.0f, halfD);
        glVertex3f(0.0f, height, halfD);
    glEnd();

    // Desenha contorno das faces para arestas em preto, sem diagonais
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.5f);

    // Contorno lateral esquerda
    glBegin(GL_LINE_LOOP);
        glVertex3f(-halfW, 0.0f, -halfD);
        glVertex3f(0.0f, height, -halfD);
        glVertex3f(0.0f, height, halfD);
        glVertex3f(-halfW, 0.0f, halfD);
    glEnd();

    // Contorno lateral direita
    glBegin(GL_LINE_LOOP);
        glVertex3f(halfW, 0.0f, -halfD);
        glVertex3f(0.0f, height, -halfD);
        glVertex3f(0.0f, height, halfD);
        glVertex3f(halfW, 0.0f, halfD);
    glEnd();

    // Contorno empena frontal
    glBegin(GL_LINE_LOOP);
        glVertex3f(-halfW, 0.0f, -halfD);
        glVertex3f(halfW, 0.0f, -halfD);
        glVertex3f(0.0f, height, -halfD);
    glEnd();

    // Contorno empena traseira
    glBegin(GL_LINE_LOOP);
        glVertex3f(-halfW, 0.0f, halfD);
        glVertex3f(halfW, 0.0f, halfD);
        glVertex3f(0.0f, height, halfD);
    glEnd();

    glPopMatrix();
}




// Agora recebe largura, altura, profundidade, posicao Y e COR (R, G, B)
// Par?metros:
// w, h, d: Tamanho
// tx, ty, tz: Transla??o (movimento extra relativo ao centro) - Padr?o 0.0
// r, g, b: Cor - Padr?o 1.0 (Branco)
void drawBox(float w, float h, float d, 
             float tx = 0.0f, float ty = 0.0f, float tz = 0.0f, 
             float r = 1.0f, float g = 1.0f, float b = 1.0f) {
    glPushMatrix();
    
    // 1. Cor
    glColor3f(r, g, b);

    // 2. Transla??o (Agora em 3 eixos)
    glTranslatef(tx, ty, tz);
    
    // 3. Escala
    glScalef(w, h, d);
    
    // 4. Desenho
    // Desenha s�lido
    glutSolidCube(1.0f);

    // Desenha arestas em preto por cima do s�lido (use wireframe para evitar diagonais)
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.5f);
    glutWireCube(1.0f);
    
    glPopMatrix();
}

// Desenha s� a moldura da janela (sem vidro).
// width: largura total (x), height: altura total (y), frameThickness: espessura da moldura
// x,y,z: posi��o do topo central da janela
// rotY: rota��o em Y
// r,g,b: cor da moldura (padr�o marrom)
void drawWindowFrame(float width, float height, float frameThickness,
                     float x, float y, float z, float rotY = 0.0f,
                     float r=0.55f, float g=0.27f, float b=0.07f) {

    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    // cor da moldura
    glColor3f(r, g, b);

    // moldura superior
    drawBox(width, frameThickness, frameThickness, 0.0f, 0.0f, 0.0f, r, g, b);

    // moldura inferior
    glPushMatrix();
    glTranslatef(0.0f, -height + frameThickness, 0.0f);
    drawBox(width, frameThickness, frameThickness, 0.0f, 0.0f, 0.0f, r, g, b);
    glPopMatrix();

    // moldura esquerda
    glPushMatrix();
    glTranslatef(-width/2.0f + frameThickness/2.0f, -height/2.0f, 0.0f);
    drawBox(frameThickness, height, frameThickness, 0.0f, 0.0f, 0.0f, r, g, b);
    glPopMatrix();

    // moldura direita
    glPushMatrix();
    glTranslatef(width/2.0f - frameThickness/2.0f, -height/2.0f, 0.0f);
    drawBox(frameThickness, height, frameThickness, 0.0f, 0.0f, 0.0f, r, g, b);
    glPopMatrix();

    // Vidro (insere um plano no centro da moldura)
    float innerW = width - 2.0f * frameThickness;
    float innerH = height - 2.0f * frameThickness;
    if (innerW > 0.0f && innerH > 0.0f) {
        // desenha vidro levemente deslocado para frente (evita z-fighting)
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glPushMatrix();
        glTranslatef(0.0f, -height/2.0f, 0.001f);
        // vidro azul claro semi-transparente
        glColor4f(0.7f, 0.9f, 1.0f, 0.45f);
        glBegin(GL_QUADS);
            glVertex3f(-innerW/2.0f,  innerH/2.0f, 0.0f);
            glVertex3f( innerW/2.0f,  innerH/2.0f, 0.0f);
            glVertex3f( innerW/2.0f, -innerH/2.0f, 0.0f);
            glVertex3f(-innerW/2.0f, -innerH/2.0f, 0.0f);
        glEnd();

        // contorno preto do vidro
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(1.2f);
        glBegin(GL_LINE_LOOP);
            glVertex3f(-innerW/2.0f,  innerH/2.0f, 0.0f);
            glVertex3f( innerW/2.0f,  innerH/2.0f, 0.0f);
            glVertex3f( innerW/2.0f, -innerH/2.0f, 0.0f);
            glVertex3f(-innerW/2.0f, -innerH/2.0f, 0.0f);
        glEnd();

        glPopMatrix();
        glDisable(GL_BLEND);
    }

    glPopMatrix();
}

//parede

//wallLength = largura da parede (horizontal)
//wallHeight = altura
//thickness  = espessura
//(x, y, z) = posi??o do centro da parede
//rotY = rota??o para posicionar a parede
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

void drawPillar(float size, float height,
                float x, float y, float z,
                float r=0.7f, float g=0.7f, float b=0.7f) {

    glPushMatrix();
    glTranslatef(x, y + height/2.0f, z);
    drawBox(size, height, size, 0.0f, 0.0f, 0.0f, r, g, b);
    glPopMatrix();
}



void drawClosedDoor(float w, float h, float thickness, 
                    float x, float y, float z, float rotY = 0.0f) {
    glPushMatrix();
    glTranslatef(x, y + h/2.0f, z); // Sobe metade da altura para ficar no chao
    glRotatef(rotY, 0, 1, 0);

    // 1. A Porta (Marrom Madeira)
    drawBox(w, h, thickness, 0.0f, 0.0f, 0.0f, 0.55f, 0.27f, 0.07f);

    // 2. A Ma?aneta (Amarela/Dourada)
    // Pequeno cubo deslocado para a direita e um pouco pra frente
    glPushMatrix();
    glTranslatef(w/2.5f, 0.0f, thickness/1.5f); // Posi??o relativa ao centro da porta
    // Ma�aneta dourada
    drawBox(0.1f, 0.1f, 0.1f, 0.0f, 0.0f, 0.0f, 1.0f, 0.8f, 0.0f);
    glPopMatrix();
    glPopMatrix();
}

void drawFloor(float width, float depth,
               float x, float y, float z,
               float r=0.3f, float g=0.3f, float b=0.3f) {

    glPushMatrix();
    glTranslatef(x, y, z);

    // Chama o drawBox atualizado
    // Par�metros: 
    // width (largura), 0.1f (espessura fina), depth (profundidade)
    // tx, ty, tz: transla��o (colocados em 0.0f)
    // r, g, b (cores)
    drawBox(width, 0.1f, depth, 0.0f, 0.0f, 0.0f, r, g, b);

    glPopMatrix();
}

void drawDoorFrame(float width, float height, float thickness, 
                   float x, float y, float z, float rotY = 0.0f) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0, 1, 0);

    // Cor Marrom Escuro para o batente
    float r = 0.4f, g = 0.2f, b = 0.0f;
    float frameThick = 0.2f; // Espessura da madeira do batente

    // 1. Pilar Esquerdo
    glPushMatrix();
    glTranslatef(-(width/2.0f), height/2.0f, 0.0f);
    // CORRE??O AQUI: Note os 3 zeros antes do r, g, b
    drawBox(frameThick, height, thickness, 0.0f, 0.0f, 0.0f, r, g, b);
    glPopMatrix();

    // 2. Pilar Direito
    glPushMatrix();
    glTranslatef((width/2.0f), height/2.0f, 0.0f);
    // CORRE??O AQUI
    drawBox(frameThick, height, thickness, 0.0f, 0.0f, 0.0f, r, g, b);
    glPopMatrix();

    // 3. Topo (Verga)
    glPushMatrix();
    glTranslatef(0.0f, height, 0.0f); 
    // CORRE??O AQUI
    drawBox(width + frameThick, frameThick, thickness, 0.0f, 0.0f, 0.0f, r, g, b);
    glPopMatrix();

    glPopMatrix();
}

void drawChair(float x, float y, float z, float scale, float rotY = 0.0f) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0, 1, 0);
    glScalef(scale, scale, scale); // Aumenta ou diminui a cadeira inteira

    // Cor da madeira (Marrom)
    float r = 0.55f, g = 0.27f, b = 0.07f;

    // --- PERNAS (0.1 largura, 0.4 altura) ---
    // Nota: O Y � 0.2 porque o drawBox desenha a partir do centro, 
    // ent�o para uma perna de altura 0.4 tocar o ch�o (0.0), o centro deve ser 0.2

    // Perna Frente Esquerda
    drawBox(0.1f, 0.4f, 0.1f, -0.2f, 0.2f, 0.2f, r, g, b);
    // Perna Frente Direita
    drawBox(0.1f, 0.4f, 0.1f, 0.2f, 0.2f, 0.2f, r, g, b);
    // Perna Tr�s Esquerda
    drawBox(0.1f, 0.4f, 0.1f, -0.2f, 0.2f, -0.2f, r, g, b);
    // Perna Tr�s Direita
    drawBox(0.1f, 0.4f, 0.1f, 0.2f, 0.2f, -0.2f, r, g, b);

    // --- ASSENTO ---
    // Largura 0.5, Altura fina 0.05
    // Y = 0.4 (topo das pernas) + 0.025 (metade da altura do assento)
    drawBox(0.5f, 0.05f, 0.5f, 0.0f, 0.425f, 0.0f, r, g, b);

    // --- ENCOSTO ---
    // Pilares do encosto (extens�o das pernas traseiras)
    drawBox(0.1f, 0.5f, 0.05f, -0.2f, 0.675f, -0.225f, r, g, b);
    drawBox(0.1f, 0.5f, 0.05f, 0.2f, 0.675f, -0.225f, r, g, b);

    // T�bua horizontal do encosto
    drawBox(0.5f, 0.15f, 0.05f, 0.0f, 0.85f, -0.225f, r, g, b);

    glPopMatrix();
}

void drawTable(float x, float y, float z, float scale) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);

    float r = 0.4f, g = 0.2f, b = 0.0f; // Madeira mais escura

    // 4 Pernas da mesa
    drawBox(0.15f, 0.6f, 0.15f, -0.4f, 0.3f, 0.4f, r, g, b);
    drawBox(0.15f, 0.6f, 0.15f, 0.4f, 0.3f, 0.4f, r, g, b);
    drawBox(0.15f, 0.6f, 0.15f, -0.4f, 0.3f, -0.4f, r, g, b);
    drawBox(0.15f, 0.6f, 0.15f, 0.4f, 0.3f, -0.4f, r, g, b);

    // Tampo da mesa
    drawBox(1.2f, 0.05f, 1.2f, 0.0f, 0.625f, 0.0f, r, g, b);

    glPopMatrix();
}

void drawPaths() {
    glColor3f(0.94f, 0.88f, 0.65f);

    // horizontal
    glPushMatrix();
    glTranslatef(0, 0, 0);
    glScalef(100, 0.1, 8);
    glutSolidCube(1);
    glPopMatrix();

    // vertical
    glPushMatrix();
    glTranslatef(0, 0, 0);
    glScalef(8, 0.1, 100);
    glutSolidCube(1);
    glPopMatrix();
}

void drawRoundabout() {
    // c�rculo verde (base)
    glPushMatrix();
    glColor3f(0.0f, 0.6f, 0.2f);
    glTranslatef(0, 0.05f, 0);
    glRotatef(90, 1, 0, 0);        // <- Deita o cilindro no ch�o
    glutSolidCylinder(10, 0.3, 40, 40);
    glPopMatrix();

    // borda branca
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 0.9f);
    glTranslatef(0, 0.06f, 0);
    glRotatef(90, 1, 0, 0);        // <- Deita o cilindro no ch�o
    glutSolidCylinder(10.5f, 0.1f, 40, 40);
    glPopMatrix();

    // caixa d'�gua (em p�, sem rota��o)
    drawCylinder(0, 0.5f, 0, 3.0f, 5.0f, 1.0f, 1.0f, 1.0f);
}

void drawVegetation() {

    // �rvores distribu�das
    drawTree(-18, 0, -18);
    drawTree(18, 0, -18);
    drawTree(-18, 0, 18);
    drawTree(18, 0, 18);
    drawTree(-10, 0, -10);
    drawTree(10, 0, -10);
    drawTree(-10, 0, 10);
    drawTree(10, 0, 10);
    drawTree(-32, 0, -32);
    drawTree(32, 0, -32);
    drawTree(-32, 0, 32);
    drawTree(32, 0, 32);
    

    // Arbustos perto dos bancos
    drawBush(-12, 0,  6);
    drawBush(-12, 0, -6);
    drawBush(12, 0,  6);
    drawBush(12, 0, -6);

    drawBush(6, 0, -12);
    drawBush(-6, 0, -12);
    drawBush(6, 0, 12);
    drawBush(-6, 0, 12);
}

void drawGrass() {
    glColor3f(0.1f, 0.7f, 0.2f);
    glPushMatrix();
    glTranslatef(0, -0.1f, 0);
    glScalef(100, 0.1f, 100);
    glutSolidCube(1);
    glPopMatrix();
}

void drawBenchDetailed(float x, float y, float z, float rot) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rot, 0, 1, 0);
    glColor3f(0.6f, 0.3f, 0.1f);

    // Par�metros do banco
    float seatWidth = 2.5f;
    float seatThickness = 0.2f;
    float seatDepth = 0.7f;
    float seatCenterY = 0.5f;              // como antes (centro do assento)
    float seatTop = seatCenterY + seatThickness * 0.5f; // altura da superf�cie do assento

    // --- Assento ---
    glPushMatrix();
    glTranslatef(0.0f, seatCenterY, 0.0f);
    glScalef(seatWidth, seatThickness, seatDepth);
    glutSolidCube(1.0f);
    glPopMatrix();

    // --- Encosto (CORRIGIDO: rota��o em torno da base do encosto) ---
    float backHeight = 0.8f;
    float backThickness = 0.2f;
    float backDepth = backThickness; // profundidade (espessura) do encosto
    float seatHalfDepth = seatDepth * 0.5f;

    // Posi��o Z do encosto: um pouco atr�s do assento
    float backZ = - (seatHalfDepth + backDepth * 0.5f);

    glPushMatrix();
    // 1) transladar para o ponto de piv� (a base do encosto, na superf�cie do assento)
    glTranslatef(0.0f, seatTop, backZ);

    // 2) rotacionar em X (pivot est� na base)
    glRotatef(-15.0f, 1.0f, 0.0f, 0.0f);

    // 3) desenhar o cubo deslocado para cima de modo que sua base coincida com o piv�
    glTranslatef(0.0f, backHeight * 0.5f, 0.0f);

    // 4) escala do encosto
    glScalef(seatWidth, backHeight, backDepth);
    glutSolidCube(1.0f);
    glPopMatrix();

    // --- Pernas ---
    float legH = 0.5f;
    float legX = seatWidth * 0.4f; // dist�ncia lateral das pernas
    float legZ = seatDepth * 0.35f;

    // Frente esquerda
    glPushMatrix();
    glTranslatef(-legX, legH * 0.5f, legZ);
    glScalef(0.2f, legH, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Frente direita
    glPushMatrix();
    glTranslatef(legX, legH * 0.5f, legZ);
    glScalef(0.2f, legH, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Tr�s esquerda
    glPushMatrix();
    glTranslatef(-legX, legH * 0.5f, -legZ);
    glScalef(0.2f, legH, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Tr�s direita
    glPushMatrix();
    glTranslatef(legX, legH * 0.5f, -legZ);
    glScalef(0.2f, legH, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPopMatrix();
}


void drawAllBenchesDetailed() {

    // --- Caminho horizontal (esquerda-direita) ---
    // lado de cima
    drawBenchDetailed(-12, 0, -6, 0);
    drawBenchDetailed(12, 0, -6, 0);
    drawBenchDetailed(-24, 0, -6, 0);
    drawBenchDetailed(24, 0, -6, 0);

    // lado de baixo
    drawBenchDetailed(-12, 0, 6, 180);
    drawBenchDetailed(12, 0, 6, 180);
    drawBenchDetailed(-24, 0, 6, 180);
    drawBenchDetailed(24, 0, 6, 180);

    // --- Caminho vertical (cima-baixo) ---
    // lado esquerdo
    drawBenchDetailed(-6, 0, -12, 90);
    drawBenchDetailed(-6, 0, 12, 90);
    drawBenchDetailed(-6, 0, -24, 90);
    drawBenchDetailed(-6, 0, 24, 90);

    // lado direito
    drawBenchDetailed(6, 0, -12, -90);
    drawBenchDetailed(6, 0, 12, -90);
    drawBenchDetailed(6, 0, -24, -90);
    drawBenchDetailed(6, 0, 24, -90);
}

void drawPlaza() {
    drawGrass();
    drawPaths();
    drawRoundabout();
    drawAllBenchesDetailed();
    drawVegetation();
}


//teclado
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

void drawFront() {
    //ch�o 1 segundo andar
    drawFloor(300, 60, 0, 15, -35, 0.5f, 0.5f, 0.5f);
    //ch�o 2 segundo andar
    drawFloor(300, 60, 0, 30, -35, 1, 1, 0.85);

    //parede grande esquerda a porta
    drawBox(140,30,1,-80,15,-5,1,1,0.85); 	
    //parede grande direita a porta
    drawBox(140,30,1,80,15,-5,1,1,0.85);

    //parede 1 meio
    drawBox(140,30,1,80,15,-45,1,1,0.85);
    //parede 2 meio
    drawBox(140,30,1,-80,15,-45,1,1,0.85); 

    //parede acima pilar meio 1
    drawBox(140,5,1,-80,15,-65,1,1,0.85); 	
    //parede acima pilar meio 2
    drawBox(140,5,1,80,15,-65,1,1,0.85);
    //parede acima pilar meio 3
    drawBox(20,5,1,0,15,-65,1,1,0.85);
    

    //Frame da porta de entrada meio 1
	drawDoorFrame(20,7,1,0,0,-45,0);
    //Frame da porta de entrada meio 2
	drawDoorFrame(20,7,1,0,15,-45,0);

    //parede acima da porta do meio 1
    drawBox(20,8,1,0,26,-45,1,1,0.85); 

    //parede acima da porta do meio 2
    drawBox(20,8,1,0,11,-45,1,1,0.85);

    //pilar 1 meio
    drawPillar(2, 30, -10, 0, -65, 1, 1, 0.85);
    //pilar 2 meio
    drawPillar(2, 30, 10, 0, -65, 1, 1, 0.85);
    //pilar 3 meio
    drawPillar(2, 30, -30, 0, -65, 1, 1, 0.85);
    //pilar 4 meio
    drawPillar(2, 30, 30, 0, -65, 1, 1, 0.85);
    //pilar 5 meio
    drawPillar(2, 30, -50, 0, -65, 1, 1, 0.85);
    //pilar 6 meio
    drawPillar(2, 30, 50, 0, -65, 1, 1, 0.85);
    //pilar 7 meio
    drawPillar(2, 30, -70, 0, -65, 1, 1, 0.85);
    //pilar 8 meio
    drawPillar(2, 30, 70, 0, -65, 1, 1, 0.85);
    //pilar 9 meio
    drawPillar(2, 30, -90, 0, -65, 1, 1, 0.85);
    //pilar 10 meio
    drawPillar(2, 30, 90, 0, -65, 1, 1, 0.85);
    //pilar 11 meio
    drawPillar(2, 30, -110, 0, -65, 1, 1, 0.85);
    //pilar 12 meio
    drawPillar(2, 30, 110, 0, -65, 1, 1, 0.85);
    //pilar 13 meio
    drawPillar(2, 30, -130, 0, -65, 1, 1, 0.85);
    //pilar 14 meio
    drawPillar(2, 30, 130, 0, -65, 1, 1, 0.85);
    //pilar 15 meio
    drawPillar(2, 30, -145, 0, -65, 1, 1, 0.85);
    //pilar 16 meio
    drawPillar(2, 30, 145, 0, -65, 1, 1, 0.85);

    //janelas Frente
    //janelas direitas
    drawWindowFrame(15, 8, 0.5f, 50, 12.5, -4, 0);
    drawWindowFrame(15, 8, 0.5f, 50, 25, -4, 0);
    drawWindowFrame(15, 8, 0.5f, 70, 12.5, -4, 0);
    drawWindowFrame(15, 8, 0.5f, 70, 25, -4, 0);
    drawWindowFrame(15, 8, 0.5f, 90, 12.5, -4, 0);
    drawWindowFrame(15, 8, 0.5f, 90, 25, -4, 0);
    drawWindowFrame(15, 8, 0.5f, 110, 12.5, -4, 0);
    drawWindowFrame(15, 8, 0.5f, 110, 25, -4, 0);
    drawWindowFrame(15, 8, 0.5f, 130, 12.5, -4, 0);
    drawWindowFrame(15, 8, 0.5f, 130, 25, -4, 0);

    //janelas esquerdas
    drawWindowFrame(15, 8, 0.5f, -50, 12.5, -4, 0);
    drawWindowFrame(15, 8, 0.5f, -50, 25, -4, 0);
    drawWindowFrame(15, 8, 0.5f, -70, 12.5, -4, 0);
    drawWindowFrame(15, 8, 0.5f, -70, 25, -4, 0);
    drawWindowFrame(15, 8, 0.5f, -90, 12.5, -4, 0);
    drawWindowFrame(15, 8, 0.5f, -90, 25, -4, 0);
    drawWindowFrame(15, 8, 0.5f, -110, 12.5, -4, 0);
    drawWindowFrame(15, 8, 0.5f, -110, 25, -4, 0);
    drawWindowFrame(15, 8, 0.5f, -130, 12.5, -4, 0);
    drawWindowFrame(15, 8, 0.5f, -130, 25, -4, 0);

    //janelas direita meio
    drawWindowFrame(15, 8, 0.5f, 30, 12.5, -46, 0);
    drawWindowFrame(15, 8, 0.5f, 30, 25, -46, 0);
    drawWindowFrame(15, 8, 0.5f, 50, 12.5, -46, 0);
    drawWindowFrame(15, 8, 0.5f, 50, 25, -46, 0);
    drawWindowFrame(15, 8, 0.5f, 70, 12.5, -46, 0);
    drawWindowFrame(15, 8, 0.5f, 70, 25, -46, 0);
    drawWindowFrame(15, 8, 0.5f, 90, 12.5, -46, 0);
    drawWindowFrame(15, 8, 0.5f, 90, 25, -46, 0);
    drawWindowFrame(15, 8, 0.5f, 110, 12.5, -46, 0);
    drawWindowFrame(15, 8, 0.5f, 110, 25, -46, 0);
    drawWindowFrame(15, 8, 0.5f, 130, 12.5, -46, 0);
    drawWindowFrame(15, 8, 0.5f, 130, 25, -46, 0);

    //janelas esquerda meio
    drawWindowFrame(15, 8, 0.5f, -30, 12.5, -46, 0);
    drawWindowFrame(15, 8, 0.5f, -30, 25, -46, 0);
    drawWindowFrame(15, 8, 0.5f, -50, 12.5, -46, 0);
    drawWindowFrame(15, 8, 0.5f, -50, 25, -46, 0);
    drawWindowFrame(15, 8, 0.5f, -70, 12.5, -46, 0);
    drawWindowFrame(15, 8, 0.5f, -70, 25, -46, 0);
    drawWindowFrame(15, 8, 0.5f, -90, 12.5, -46, 0);
    drawWindowFrame(15, 8, 0.5f, -90, 25, -46, 0);
    drawWindowFrame(15, 8, 0.5f, -110, 12.5, -46, 0);
    drawWindowFrame(15, 8, 0.5f, -110, 25, -46, 0);
    drawWindowFrame(15, 8, 0.5f, -130, 12.5, -46, 0);
    drawWindowFrame(15, 8, 0.5f, -130, 25, -46, 0);

    //Lado Direito
    //Salas Reparti��o
    //Parede a direita grande reparti��o
    drawBox(140,30,1,80,15,-20,1,1,0.85);

    //Parede a direita que fecha a reparti��o
    drawBox(1,30,15,10,15,-13,1,1,0.85);

    //portas a direita da entrada
    drawClosedDoor(5,10,1,30,0,-20.5,180);
    drawClosedDoor(5,10,1,60,0,-20.5,180);
    drawClosedDoor(5,10,1,90,0,-20.5,180);

    //Lado Esquerdo
    //Salas Reparti��o
    //Parede a esquerda grande reparti��o
    drawBox(140,30,1,-80,15,-20,1,1,0.85);

    //Parede a esquerda que fecha a reparti��o
    drawBox(1,30,15,-10,15,-13,1,1,0.85);

    //portas a esquerda da entrada
    drawClosedDoor(5,10,1,-30,0,-20.5,180);
    drawClosedDoor(5,10,1,-60,0,-20.5,180);
    drawClosedDoor(5,10,1,-90,0,-20.5,180);

    //Lado Direito em cima
    //Salas Reparti��o
    //portas a direita da entrada
    drawClosedDoor(5,10,1,30,15,-20.5,180);
    drawClosedDoor(5,10,1,60,15,-20.5,180);
    drawClosedDoor(5,10,1,90,15,-20.5,180);

    //Lado Esquerdo
    //Salas Reparti��o
    //portas a esquerda da entrada
    drawClosedDoor(5,10,1,-30,15,-20.5,180);
    drawClosedDoor(5,10,1,-60,15,-20.5,180);
    drawClosedDoor(5,10,1,-90,15,-20.5,180);
}

void drawScene() {
    drawFront();
    // Segunda chamada rotacionada em 90 graus em Y e deslocada
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    // Transla��o aplicada ap�s a rota��o (ajusta no sistema local rotacionado)
    glTranslatef(215.0f, 0.0f, 215.0f);
    drawFront();
    glPopMatrix();

    // Terceira chamada rotacionada em 180 graus em Y e deslocada
    glPushMatrix();
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    // Transla��o aplicada ap�s a rota��o (ajusta no sistema local rotacionado)
    glTranslatef(0.0f, 0.0f, 430.0f);
    drawFront();
    glPopMatrix();

	//entrada
    drawFloor(1000, 1000, 0, 0, 0, 0.5f, 0.5f, 0.5f);
    //pilar 1 entrada
    drawPillar(2, 30, -10, 0, 25, 1, 1, 0.85);
    //pilar 2 entrada
    drawPillar(2, 30, 10, 0, 25, 1, 1, 0.85);
    //pilar 3 entrada
    drawPillar(2, 30, -30, 0, 25, 1, 1, 0.85);
    //pilar 4 entrada
    drawPillar(2, 30, 30, 0, 25, 1, 1, 0.85);

    //parede acima pilar meio 4
    drawBox(60,5,1,-180,15,-65,1,1,0.85); 	
    //parede acima pilar meio 5
    drawBox(1,5,20,-210,15,-55,1,1,0.85);
    //parede quina traseira interior 1
    drawBox(1,30,20,170,15,-375,1,1,0.85);
    //parede quina traseira interior 2
    drawBox(20,30,1,160,15,-385,1,1,0.85);
    //parede quina traseira interior 3
    drawBox(1,30,45,195,15,-387.5,1,1,0.85);
    //parede quina traseira interior 4
    drawBox(45,30,1,172.5,15,-410,1,1,0.85);
    //parede quina traseira exterior 1	
    drawBox(1,30,60,210,15,-395,1,1,0.85);
    //parede quina traseira exterior 2	
    drawBox(60,30,1,180,15,-425,1,1,0.85);

    //parede fechamento traseira esquerda
    drawBox(1,30,60,-150,15,-395,1,1,0.85);

    //parede fechamento traseira meio 1
    drawBox(20,30,1,0,15,-425,1,1,0.85); 
    //parede fechamento traseira meio 2
    drawBox(20,30,1,0,15,-410,1,1,0.85);

    //ch�o 3 segundo andar
    drawFloor(60, 75, -180, 30, -27.5, 1, 1, 0.85);
    //ch�o 4 segundo andar
    drawFloor(60, 75, -180, 15, -27.5, 0.5f, 0.5f, 0.5f);
    //ch�o cima quina esquerda
    drawFloor(60, 75, 180, 15, -27.5, 0.5f, 0.5f, 0.5f);
    //ch�o cima quina traseira
    drawFloor(60, 60, 180, 15, -395, 0.5f, 0.5f, 0.5f);
    //ch�o teto quina esquerda
    drawFloor(60, 75, 180, 30, -27.5, 1, 1, 0.85);
    //ch�o teto quina traseira
    drawFloor(60, 60, 180, 30, -395, 1, 1, 0.85);
    
    //varanda entre os pilares
    drawFloor(60, 40, 0, 15, 5, 0.5f, 0.5f, 0.5f);
    //Telhado acima dos pilares
    drawFloor(60, 40, 0, 30, 5, 1, 1, 0.85);

    //paredes da entrada (y = wallHeight/2 = 15)
    //parede esquerda porta
    drawBox(10,30,1,-10,15,0,1,1,0.85);
    //parede direita porta
    drawBox(10,30,1,10,15,0,1,1,0.85); 
    //parede esquerda perpendicular a porta	
    drawBox(1,30,5,-15,15,-2.5,1,0.85,0.85);
    //parede direita perpendicular a porta
    drawBox(1,30,5,15,15,-2.5,1,0.85,0.85);

    //parede varanda entrada
    drawBox(60,8,1,0,15,25,1,1,0.85);
    //parede varanda entrada
    drawBox(1,8,30,30,15,10,1,1,0.85);
    //parede varanda entrada
    drawBox(1,8,30,-30,15,10,1,1,0.85);

    //parede 1 direita quadrado
    drawBox(1,30,30,150,15,-5,1,1,0.85);
    //parede 2 direita quadrado
    drawBox(60,30,1,180,15,10,1,1,0.85);
    //parede 3 direita quadrado
    drawBox(1,30,200,210,15,-90,1,1,0.85);

    //parede 1 esquerda quadrado
    drawBox(1,30,30,-150,15,-5,1,1,0.85);
    //parede 2 esquerda quadrado
    drawBox(60,30,1,-180,15,10,1,1,0.85);
    //parede 3 esquerda quadrado
    drawBox(1,30,55,-210,15,-17.5,1,1,0.85);	


    //parede acima da porta de entrada
    drawBox(10,8,1,0,11,0,1,1,0.85); 

    //parede acima da porta da varanda
    drawBox(10,8,1,0,26,0,1,1,0.85); 
 
    //parede esquerda meio
    drawBox(60,30,1,-180,15,-45,1,1,0.85);


    //Frame da porta de entrada
	drawDoorFrame(10,7,1,0,0,0,0);
    //Frame da porta da varanda
	drawDoorFrame(10,7,1,0,15,0,0);
	
    //Telhado da frente perpendicular
    drawRoof(70, 300, 10, 0, 30, -30, 90, 0.6f, 0.2f, 0.0f);
    //Telhado da frente
    drawRoof(60, 55, 10, 0, 30, -2.5, 0, 0.6f, 0.2f, 0.0f);
    //Telhado direito 
    drawRoof(60, 375, 10, 180, 30, -177.5, 0, 0.6f, 0.2f, 0.0f);
    //Telhado esquerdo
    drawRoof(60, 75, 10, -180, 30, -27.5, 0, 0.6f, 0.2f, 0.0f);
    //Telhado traseiro
    drawRoof(60, 360, 10, 30, 30, -395, 90, 0.6f, 0.2f, 0.0f);
    

    //pilar 17 meio
    drawPillar(2, 30, -160, 0, -65, 1, 1, 0.85);
    //pilar 18 meio
    drawPillar(2, 30, -175, 0, -65, 1, 1, 0.85);
    //pilar 19 meio
    drawPillar(2, 30, -190, 0, -65, 1, 1, 0.85);
    //pilar 20 meio
    drawPillar(2, 30, -205, 0, -65, 1, 1, 0.85);
    
    //Auditorio A
    //Primeira passagem
     //Paredezinha
    drawBox(1,15,10,150,7.5,-25,1,1,0.85);
    drawDoorFrame(14,12,1,150,0,-37,90);
    drawBox(1,2.8,14,150,13.5,-37,1,1,0.85);



    //Segunda passagem
    //Delimita��o Auditorio Parede
    drawBox(1,15,30,180,7.5,-5,1,1,0.85);
    drawBox(1,3,15,180,13.3,-27.5,1,1,0.85);
    drawDoorFrame(14.2,12,1,180,0,-27.5,90);
    drawClosedDoor(7,12,1,180,0,-31.5,270);
    drawClosedDoor(7,12,1,183,0,-21.5,25);
    drawBox(1,15,15,180,7.5,-42.5,1,1,0.85);

    //Sa�da Auditorio
    
    
    

    //Porta aberta saida auditorio
    drawBox(1,30,20,170,15,-55,1,1,0.85);
    drawBox(7.5,15,1,168.625,7.5,-45,1,1,0.85);
    drawBox(14.5,4.5,1,157.5,12.5,-45,1,1,0.85);
    drawDoorFrame(14.5,10,1,157.5,0,-45,0);
    drawClosedDoor(6.5,10,1,161.425,0,-45,180);
    drawClosedDoor(7.5,10,1,150.425,0,-48.5,90);

    drawBox(7.5,15,1,176.2,7.5,-45,1,1,0.85);
    
    //cadeira perto audi A
    drawChair(165,0,-5,5,180);
    drawTable(165,0,-12,5);
    drawTable(159,0,-12,5);
    drawTable(171,0,-12,5);

    //Fechamento audit�rio
    drawBox(30,15,1,195,7.5,-45,1,1,0.85);

    //Preenchendo o aduitorio
    drawChair(185,0,-5,5,180);
    drawChair(188,0,-5,5,180);
    drawChair(191,0,-5,5,180);
    drawChair(194,0,-5,5,180);
    drawChair(197,0,-5,5,180);
    drawChair(200,0,-5,5,180);
    drawChair(203,0,-5,5,180);
    drawChair(206,0,-5,5,180);

    drawChair(185,0,4,5,180);
    drawChair(188,0,4,5,180);
    drawChair(191,0,4,5,180);
    drawChair(194,0,4,5,180);
    drawChair(197,0,4,5,180);
    drawChair(200,0,4,5,180);
    drawChair(203,0,4,5,180);
    drawChair(206,0,4,5,180);

    drawChair(185,0,-14,5,180);
    drawChair(188,0,-14,5,180);
    drawChair(191,0,-14,5,180);
    drawChair(194,0,-14,5,180);
    drawChair(197,0,-14,5,180);
    drawChair(200,0,-14,5,180);
    drawChair(203,0,-14,5,180);
    drawChair(206,0,-14,5,180);

    drawChair(185,0,-19,5,180);
    drawChair(188,0,-19,5,180);
    drawChair(191,0,-19,5,180);
    drawChair(194,0,-19,5,180);
    drawChair(197,0,-19,5,180);
    drawChair(200,0,-19,5,180);
    drawChair(203,0,-19,5,180);
    drawChair(206,0,-19,5,180);

    //Tel�o
    drawBox(13,8,1,195,10,-44,0.8,0.8,0.8);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // FPS CAMERA
    gluLookAt(camX, camY, camZ,
              camX + dirX, camY + dirY, camZ + dirZ,
              0, 1, 0);

    drawScene();
    glPushMatrix();
    glTranslatef(0, 0.2f, -205);
    glScalef(3, 2.5, 2.8);
	drawPlaza();
	glPopMatrix();
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

//mouse
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
    // Aumenta o far plane para 5000.0f para expandir o alcance de renderiza��o
    // e ajusta o near para 0.5f para preservar precis�o de profundidade
    gluPerspective(60.0, (float)w / h, 0.5, 5000.0);

    glMatrixMode(GL_MODELVIEW);
}

void init() {
    glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
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


