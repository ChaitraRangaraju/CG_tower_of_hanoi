#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

char str1[] = "                                 VIDYAVARDAKA COLLEGE OF ENGINEERING,MYSURU";
char str2[] = "			            DEPARTMENT OF COMPUTER SCIENCE AND ENGINEERING";
char str3[] = "COMPUTER GRAPHICS AND VISUALIZATION LABORATORY  (18CSL67)";
char str4[] = "A MINI PROJECT";
char str5[] = "ON";
char str6[] = "           TOWER OF HANOI";
char str7[] = "Submitted By:";
char str8[] = "CHAITRA R - 4VV19CS027";
char str9[] = "CHANDANA C SHEKAR - 4VV19CS031";
char str10[] = "Submitted To:";
char str11[] = "PROF.Arpitha M S";
char str12[] = "PROF.Gaurav P R";
char str28[] = "( Press ENTER to continue )";


GLfloat baseVertices[] = { 100, -20, -300, 1260, -20, -300,
                          1260, 0, 300, 100, 0, -300, 100,
                  -20, 300, 1260, -20, 300, 1260,
                  0, 300, 100, 0, 300 };

GLfloat baseColors[] = { 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
                        1.0, 1.0, 0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 1.0, 1.0, 0.0, 1.0,
            1.0, 1.0, 1.0, 0.0, 1.0, 1.0 };

GLubyte cubeIndices[] = { 0,3,2,1,
                   2,3,7,6,
               0,4,7,3,
               1,2,6,5,
               4,5,6,7,
               0,1,5,4 };

GLfloat WIDTH = 1360;
GLfloat HEIGHT = 768;

GLint NUM_DISKS;

GLboolean motion = GL_FALSE;
GLfloat xangle = 0, yangle = 0;
GLfloat xlangle = 0, ylangle = 0;

#define other(i,j) (6-(i+j))
#define DISK_HEIGHT 35
#define CONE NUM_DISKS+1
#define HANOI_SOLVE 0
#define HANOI_QUIT 1

GLfloat lightTwoColor[] = { 1.0, 0.0, 1, 1.0 };
GLfloat lightZeroColor[] = { .3, .3, .3, .3 };

GLfloat diskColor[] = { 0.0, 1.0, 0.0 };
GLfloat poleColor[] = { 0.5, 0.5, 1.5 };


//Structures for stack
typedef struct stack_node
{
    int size;
    struct stack_node* next;
} stack_node;


typedef struct stack
{
    struct stack_node* head;
    int depth;
} stack;


stack poles[4];


int push(int which, int size)
{
    stack_node* next = new(stack_node);
    if (!next)
    {
        fprintf(stderr, "out of memory!\n");
        exit(-1);
    }
    next->size = size;
    next->next = poles[which].head;
    poles[which].head = next;
    poles[which].depth++;
}


int pop(int which)
{
    int retval = poles[which].head->size;
    stack_node* temp = poles[which].head;
    poles[which].head = poles[which].head->next;
    poles[which].depth--;
    free(temp);
    return retval;
}


typedef struct move_node
{
    int t, f;
    struct move_node* next;
    struct move_node* prev;
} move_node;


typedef struct move_stack
{
    int depth;
    struct move_node* head, * tail;
} move_stack;


move_stack moves;


//Initialize towers and rings
void init(void)
{
    glClearColor(0.15, 0.15, 0.15, 0.15);
    glShadeModel(GL_SMOOTH);
    int i;
    for (i = 0; i < 4; i++)
    {
        poles[i].head = NULL;
        poles[i].depth = 0;
    }
    moves.head = NULL;
    moves.tail = NULL;
    moves.depth = 0;

    for (i = 1; i <= NUM_DISKS; i++)
    {
        glNewList(i, GL_COMPILE);
        {
            glutSolidTorus(DISK_HEIGHT / 2, 12 * i, 15, 15); //glutSolidTorus(innerRadius, outerRadius, nsides, rings)
        }
        glEndList();
    }

    glNewList(CONE, GL_COMPILE);
    {
        glutSolidCone(15, (NUM_DISKS + 1) * DISK_HEIGHT, 20, 20); //glutSolidCone(base, height, slices, stacks);
    }
    glEndList();
}


void mpop(void)
{
    move_node* temp = moves.head;
    moves.head = moves.head->next;
    free(temp);
    moves.depth--;
}


void mpush(int t, int f)
{
    move_node* new1 = new(move_node);
    new1->t = t;
    new1->f = f;
    new1->next = NULL;
    new1->prev = moves.tail;
    if (moves.tail)
        moves.tail->next = new1;
    moves.tail = new1;
    if (!moves.head)
        moves.head = moves.tail;
    moves.depth++;
}


void update(void)
{
    while (motion == GL_TRUE && motion++)	//To solve it at once without mouse clicks, comment this while loop
    {
        glutPostRedisplay();
    }
}


void DrawPost(GLfloat xcenter)
{
    glPushMatrix();
    {
        glTranslatef(xcenter, 0, 0);
        glRotatef(90, -1, 0, 0);
        glCallList(CONE);
    }
    glPopMatrix();
}


void DrawPosts(void)
{
    glColor3fv(poleColor);
    glLineWidth(10);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, poleColor);
    DrawPost((int)(WIDTH / 4));
    DrawPost((int)(2 * WIDTH / 4));
    DrawPost((int)(3 * WIDTH / 4));
}


void DrawDisk(GLfloat xcenter, GLfloat ycenter, GLfloat size)
{
    glPushMatrix();
    {
        glTranslatef(xcenter, ycenter, 0);
        glRotatef(90, 1, 0, 0);
        glCallList(size);
    }
    glPopMatrix();
}


void DrawDisks(void)
{
    int i;
    stack_node* temp;
    int xcenter, ycenter;
    glColor3fv(diskColor);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diskColor);
    for (i = 1; i <= 3; i++)
    {
        xcenter = i * WIDTH / 4;
        for (temp = poles[i].head, ycenter = DISK_HEIGHT * poles[i].depth - DISK_HEIGHT / 2; temp; temp = temp->next, ycenter -= DISK_HEIGHT) {
            DrawDisk(xcenter, ycenter, temp->size);
        }
    }
}


#define MOVE(t,f) mpush((t),(f))


static void mov(int n, int f, int t)
{
    int o;
    if (n == 1)
    {
        MOVE(t, f);
        printf("\nDisk moves From Peg: %d -> Peg: %d", f, t);
        return;
    }
    o = other(f, t);
    mov(n - 1, f, o);
    mov(1, f, t);
    mov(n - 1, o, t);
}


void Drawcube() {
    glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, cubeIndices);
    glColor3fv(baseColors);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, baseColors);

}


void draw(void)
{
    int t, f;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, (int)WIDTH, (int)HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, -70.0, HEIGHT, -10000, 10000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0, 0, 0, 0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);

    glLightfv(GL_LIGHT2, GL_DIFFUSE, lightTwoColor);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 10);
    glEnable(GL_LIGHT2);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    glPushMatrix();
    {
        glTranslatef(WIDTH / 2, HEIGHT / 2, 0);
        glRotatef(xangle, 0, 1, 0);
        glRotatef(yangle, 1, 0, 0);
        glTranslatef(-WIDTH / 2, -HEIGHT / 2, 0);
        DrawPosts();
        DrawDisks();
        Drawcube();
    }
    glPopMatrix();

    if (motion && moves.depth)
    {
        t = moves.head->t;
        f = moves.head->f;
        push(t, pop(f));
        mpop();
    }
    glutSwapBuffers();
}


void hanoi_menu(int value)
{
    switch (value)
    {
    case HANOI_SOLVE: motion = !motion;
        if (motion)
        {
            glutIdleFunc(update);
        }
        else
        {
            glutIdleFunc(NULL);
        }
        break;

    case HANOI_QUIT: exit(0);
        break;
    }
    glutPostRedisplay();
}


int oldx, oldy;

GLboolean leftb = GL_FALSE, middleb = GL_FALSE;


void hanoi_mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        oldx = x;
        oldy = y;
        if (state == GLUT_DOWN)
            leftb = GL_TRUE;
        else
            leftb = GL_FALSE;
    }
    if (button == GLUT_MIDDLE_BUTTON)
    {
        oldx = x;
        oldy = y;
        if (state == GLUT_DOWN)
            middleb = GL_TRUE;
        else
            middleb = GL_FALSE;
    }
}


void hanoi_visibility(int state)
{
    if (state == GLUT_VISIBLE && motion)
    {
        glutIdleFunc(update);
    }
    else
    {
        glutIdleFunc(NULL);
    }
}


void hanoi_motion(int x, int y)
{
    if (leftb)
    {
        xangle -= (x - oldx);
        yangle -= (y - oldy);
    }
    if (middleb)
    {
        xlangle -= (x - oldx);
        ylangle -= (y - oldy);
    }
    oldx = x;
    oldy = y;
    glutPostRedisplay();
}


void myKey(unsigned char key, int x, int y) {
    if (key == (char)13)
    {
        glutDisplayFunc(draw);
        draw();

        glEnd();
        glFlush();
    }

}


void display1() {

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-10, 10, -10, 10);
    glMatrixMode(GL_MODELVIEW);
    glColor3f(1, 1, 1);
    int i;
    glRasterPos2f(-6.5, 7);
    for (i = 0; i < strlen(str1); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str1[i]);

    glRasterPos2f(-5.5, 4.2);
    for (i = 0; i < strlen(str2); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str2[i]);

    glRasterPos2f(-5, 3);
    for (i = 0; i < strlen(str3); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str3[i]);

    glRasterPos2f(-1.5, 1);
    for (i = 0; i < strlen(str4); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str4[i]);

    glRasterPos2f(-0.5, 0);
    for (i = 0; i < strlen(str5); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str5[i]);

    glRasterPos2f(-2.5, -1);
    for (i = 0; i < strlen(str6); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str6[i]);
    glRasterPos2f(-9, -5);
    for (i = 0; i < strlen(str7); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str7[i]);
    glRasterPos2f(-8.5, -6);
    for (i = 0; i < strlen(str8); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str8[i]);
    glRasterPos2f(-8.5, -7);
    for (i = 0; i < strlen(str9); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str9[i]);
    glRasterPos2f(-8.5, -8);

    glRasterPos2f(5, -5);
    for (i = 0; i < strlen(str10); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str10[i]);
    glRasterPos2f(5.5, -6);
    for (i = 0; i < strlen(str11); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str11[i]);
    glRasterPos2f(5.5, -7);
    for (i = 0; i < strlen(str12); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str12[i]);
    glColor3f(1.0, 0.0, 0.0);
    glRasterPos2f(-1.75, -9);
    for (i = 0; i < strlen(str28); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str28[i]);


    glutKeyboardFunc(myKey);
    glFlush();
    glutSwapBuffers();
}

int main(int argc, char* argv[])
{
    int i;
    NUM_DISKS = 5;

    //printf("Enter the number of Disks: ");
    //scanf_s("%d", &NUM_DISKS);

    double DISK_MOVES = pow(2.0, NUM_DISKS) - 1;
    printf("\nTotal number of Disk movements for %d disks : %.0lf\n", NUM_DISKS, DISK_MOVES);

    if (NUM_DISKS > 0)
    {
        glutInit(&argc, argv);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0,0,0,0);
        glutInitWindowSize(1500, 700);
        glutInitWindowPosition(0, 0);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
        glutCreateWindow("Tower of Hanoi");
        glutDisplayFunc(display1);
        glutKeyboardFunc(myKey);
        glutMouseFunc(hanoi_mouse);
        glutMotionFunc(hanoi_motion);
        glutVisibilityFunc(hanoi_visibility);
        
        glutCreateMenu(hanoi_menu);
        glutAddMenuEntry("Solve", HANOI_SOLVE);
        glutAddMenuEntry("Quit", HANOI_QUIT);
        glutAttachMenu(GLUT_RIGHT_BUTTON);
        init();

        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, baseVertices);
        glColorPointer(3, GL_FLOAT, 0, baseColors);

        for (i = 0; i < NUM_DISKS; i++)
            push(1, NUM_DISKS - i);
        mov(NUM_DISKS, 1, 3);

       
        glutMainLoop();
        return 0;
       
    }
    else
    {
        exit(0);
    }
}