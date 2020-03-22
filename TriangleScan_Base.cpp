/* Extend the code to satisfy the assignment 1 requirements */

#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <GL/glut.h>
#define WIDTH 400
#define HEIGHT 300
static GLubyte frame_buffer[HEIGHT][WIDTH][3];

// Fill each scanline
// yT = top of the scanline, yB = bottom of the scanline
void FillScanLine(int x, float yT, float rT, float gT, float bT, float yB, float rB, float gB, float bB)
{
    int yT_floor, yB_ceil;
    int y;
    float r, g, b, mr, mg, mb;

    yT_floor = floor(yT);
    yB_ceil = ceil(yB);

    mr = (rT - rB) / (yT - yB);
    mg = (gT - gB) / (yT - yB);
    mb = (bT - bB) / (yT - yB);
    r = rB + (yB_ceil - yB) * mr;
    g = gB + (yB_ceil - yB) * mg;
    b = bB + (yB_ceil - yB) * mb;

    for (y = yB_ceil; y <= yT_floor; y++) {
        frame_buffer[y][x][0] = (unsigned char) (r + 0.5f);
        frame_buffer[y][x][1] = (unsigned char) (g + 0.5f);
        frame_buffer[y][x][2] = (unsigned char) (b + 0.5f);

        r += mr;
        g += mg;
        b += mb;
    }
}

// The main triangle scan conversion function 
// Assumes x0 <= x1 <= x2
void ScanConvertTriangle(
    int x0, int y0, int r0, int g0, int b0,
    int x1, int y1, int r1, int g1, int b1,
    int x2, int y2, int r2, int g2, int b2
)
{
    float l01_y, l01_m;
    float l01_r, l01_g, l01_b, l01_mr, l01_mg, l01_mb;
    float l02_y, l02_m;
    float l02_r, l02_g, l02_b, l02_mr, l02_mg, l02_mb;
    int x;

    // Computes the slopes for L01 and L02
    l01_m = (float) (y1 - y0) / (x1 - x0);
    l01_mr = (float) (r1 - r0) / (x1 - x0);
    l01_mg = (float) (g1 - g0) / (x1 - x0);
    l01_mb = (float) (b1 - b0) / (x1 - x0);

    l02_m = (float) (y2 - y0) / (x2 - x0);
    l02_mr = (float) (r2 - r0) / (x2 - x0);
    l02_mg = (float) (g2 - g0) / (x2 - x0);
    l02_mb = (float) (b2 - b0) / (x2 - x0);

    // Initializes the edge tracing
    l01_y = y0;
    l01_r = r0;
    l01_g = g0;
    l01_b = b0;

    l02_y = y0;
    l02_r = r0;
    l02_g = g0;
    l02_b = b0;

    // Scan the first half of the triangle
    // TODO: handle the case when x0==x1 
    for (x = x0; x < x1; x++) {
        if (l02_y >= l01_y) {
            FillScanLine(x, l02_y, l02_r, l02_g, l02_b, l01_y, l01_r, l01_g, l01_b);
        } else {
            FillScanLine(x, l01_y, l01_r, l01_g, l01_b, l02_y, l02_r, l02_g, l02_b);
        }

        l01_y += l01_m;
        l01_r += l01_mr;
        l01_g += l01_mg;
        l01_b += l01_mb;

        l02_y += l02_m;
        l02_r += l02_mr;
        l02_g += l02_mg;
        l02_b += l02_mb;

        if (x0 == x1) {
            if (l02_y >= l01_y) {
                FillScanLine(x, l02_y, l02_r, l02_g, l02_b, l01_y, l01_r, l01_g, l01_b);
            } else {
                FillScanLine(x, l01_y, l01_r, l01_g, l01_b, l02_y, l02_r, l02_g, l02_b);
            }
        }
    }

    // Scan the second half of the triangle
    // TODO: handle the case when x1==x2

    // Computes the slopes for L01

    l01_m = (float) (y2 - y1) / (x2 - x1);
    l01_mr = (float) (r2 - r1) / (x2 - x1);
    l01_mg = (float) (g2 - g1) / (x2 - x1);
    l01_mb = (float) (b2 - b1) / (x2 - x1);

    for (x = x1; x < x2; x++) {
        if (l02_y >= l01_y) {
            FillScanLine(x, l02_y, l02_r, l02_g, l02_b, l01_y, l01_r, l01_g, l01_b);
        } else {
            FillScanLine(x, l01_y, l01_r, l01_g, l01_b, l02_y, l02_r, l02_g, l02_b);
        }

        l01_y += l01_m;
        l01_r += l01_mr;
        l01_g += l01_mg;
        l01_b += l01_mb;

        l02_y += l02_m;
        l02_r += l02_mr;
        l02_g += l02_mg;
        l02_b += l02_mb;

        if (x2 == x1) {
            if (l02_y > l01_y) {
                FillScanLine(x, l02_y, l02_r, l02_g, l02_b, l01_y, l01_r, l01_g, l01_b);
            } else {
                FillScanLine(x, l01_y, l01_r, l01_g, l01_b, l02_y, l02_r, l02_g, l02_b);
            }
        }
    }
}

/* Called when mouse button pressed: */
void mousebuttonhandler(int button, int state, int x, int y)
{
    static int cnt = 0;
    static int points[3][2];
    static unsigned char color[3][3] = {
      {255,   0,   0},      // r0, g0, b0
      {  0, 255,   0},      // r1, g1, b1
      {  0,   0, 255}       // r2, g2, b2
    };

    //printf("Mouse button event, button=%d, state=%d, x=%d, y=%d\n", button, state, x, y);

    // set a pixel's red color value when left mouse button is pressed down:
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        points[cnt][0] = x;
        points[cnt][1] = HEIGHT - y - 1;
        cnt++;

        //printf("Mouse clicked=%d, x=%d, y=%d\n", cnt, x, y);
        if (cnt == 1) {
            memset(frame_buffer, 0, sizeof(frame_buffer));
        }

        if (cnt == 3) {
            // TODO: make sure point0, 1, 2 are passed in the correct order x0<=x1<=x2
            if (points[0][0] <= points[1][0]) {
                if (points[1][0] <= points[2][0]) {  // x0 <= x1 <= x2
                    ScanConvertTriangle(
                        points[0][0], points[0][1], color[0][0], color[0][1], color[0][2],  // x0, y0, r0, g0, b0
                        points[1][0], points[1][1], color[1][0], color[1][1], color[1][2],  // x1, y1, r1, g1, b1
                        points[2][0], points[2][1], color[2][0], color[2][1], color[2][2]   // x2, y2, r2, g2, b2
                    );
                } else
                    if (points[0][0] <= points[2][0]) {  // x0 <= x2 <= x1
                        ScanConvertTriangle(
                            points[0][0], points[0][1], color[0][0], color[0][1], color[0][2],  // x0, y0, r0, g0, b0
                            points[2][0], points[2][1], color[2][0], color[2][1], color[2][2],  // x2, y2, r2, g2, b2
                            points[1][0], points[1][1], color[1][0], color[1][1], color[1][2]   // x1, y1, r1, g1, b1
                        );
                    } else {  // x2 <= x0 <= x1
                        ScanConvertTriangle(
                            points[2][0], points[2][1], color[2][0], color[2][1], color[2][2],  // x2, y2, r2, g2, b2
                            points[0][0], points[0][1], color[0][0], color[0][1], color[0][2],  // x0, y0, r0, g0, b0
                            points[1][0], points[1][1], color[1][0], color[1][1], color[1][2]   // x1, y1, r1, g1, b1
                        );
                    }
            } else {  // x1 < x0
                // TODO: fill in the rest here
                if (points[0][0] <= points[2][0]) {  // x1 < x0 <= x2
                    ScanConvertTriangle(
                        points[1][0], points[1][1], color[1][0], color[1][1], color[1][2],  // x1, y1, r1, g1, b1
                        points[0][0], points[0][1], color[0][0], color[0][1], color[0][2],  // x0, y0, r0, g0, b0
                        points[2][0], points[2][1], color[2][0], color[2][1], color[2][2]   // x2, y2, r2, g2, b2
                    );
                } else {
                    if (points[1][0] <= points[2][0]) {  // x1 <= x2 < x0
                        ScanConvertTriangle(
                            points[1][0], points[1][1], color[1][0], color[1][1], color[1][2],  // x1, y1, r1, g1, b1
                            points[2][0], points[2][1], color[2][0], color[2][1], color[2][2],  // x2, y2, r2, g2, b2
                            points[0][0], points[0][1], color[0][0], color[0][1], color[0][2]   // x0, y0, r0, g0, b0
                        );
                    } else {  // x2 < x1 < x0
                        ScanConvertTriangle(
                            points[2][0], points[2][1], color[2][0], color[2][1], color[2][2],  // x2, y2, r2, g2, b2
                            points[1][0], points[1][1], color[1][0], color[1][1], color[1][2],  // x1, y1, r1, g1, b1
                            points[0][0], points[0][1], color[0][0], color[0][1], color[0][2]   // x0, x1, r0, g0, b0
                        );
                    }
                }
            }
            cnt = 0;
        }
    }

    // cause a display event to occur for GLUT:
    glutPostRedisplay();
}

/* Called by GLUT when a display event occurs: */
void display(void)
{
    /*	Set the raster position to the lower-left corner to avoid a problem
        (with glDrawPixels) when the window is resized to smaller dimensions.*/
    glRasterPos2i(-1, -1);

    // Write the information stored in "frame_buffer" to the color buffer
    glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, frame_buffer);
    glFlush();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Frame Buffer Example");

    // Specify which functions get called for display and mouse events:
    glutDisplayFunc(display);
    glutMouseFunc(mousebuttonhandler);

    glutMainLoop();

    return 0;
}
