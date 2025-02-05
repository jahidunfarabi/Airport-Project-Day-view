#include<windows.h>
#include<mmsystem.h>
#include <GL/glut.h>
#include <cmath>
#define PI 3.14159f
#include <algorithm>
#include <cstdlib> // For srand, rand
#include <ctime>   // For time

int i;
float cloudOffset = -1.0f;
float planeX = 1.0f;
float planeY = -0.28f;
float speed = 0.006;
float angle = 0.0;
bool isAscending = false;
bool hasFlown = false;

void drawCircle(float x, float y, float radius)
{
    int numSegments = 100; // Smoothness of the circle
    float angle;

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // Center of the circle
    for (int i = 0; i <= numSegments; i++)
    {
        angle = (i * 2.0f * PI )/ numSegments;
        glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
    }
    glEnd();
}

void drawSky() {
    glBegin(GL_QUADS);

    // Top part of the sky (Light Blue)
    glColor3f(0.5, 0.8, 1.0);
    glVertex2f(-1.0, 0.0);
    //glVertex2f(1.0, 0.0);
    glVertex2f(-1.0, 1.0);
    glVertex2f(1.0, 1.0);

    // Transition area towards the sun (Yellowish blend)
    glColor3f(0.8, 0.7, 0.2); // Yellowish blend
    glVertex2f(1.0, 0.2); // Adjust 0.2 to control the gradient height/start
    glVertex2f(-1.0, 0.2);

    // Bottom part of the sky (more Yellowish close to horizon/sun)
    glColor3f(0.9, 0.8, 0.0); // More yellowish
    glVertex2f(-1.0, -0.2); // Adjust -0.2 to control the gradient height/end
    glVertex2f(1.0, -0.2);

    glEnd();
}

void drawGround() {
    glBegin(GL_QUADS);

    for (float x = -1.0; x < 1.0; x += 0.02) {
        for (float y = -1.0; y < 0.0; y += 0.02) {

            // Calculate varying green with a stronger light green effect
            float green = 0.5 + (x + 1.0) * 0.1 + (y + 1.0) * 0.1; // Reduced base green variation
            float lightGreenFactor = (1.0 - (y + 1.0)) * 0.3; // Increased light green influence
            green += lightGreenFactor;

            // Add a small amount of blue, more subtle
            float blue = (1.0 - (y + 1.0)) * 0.03; // Reduced blue amount

            // Clamp colors
            green = std::fmin(1.0f, std::fmax(0.0f, green));
            blue = std::fmin(1.0f, std::fmax(0.0f, blue));

            glColor3f(0.0, green, blue);

            glVertex2f(x, y);
            glVertex2f(x + 0.02, y);
            glVertex2f(x + 0.02, y + 0.02);
            glVertex2f(x, y + 0.02);
        }
    }
    glEnd();
}

// Function to draw a rectangle
void drawRectangle(float x, float y, float width, float height, float r, float g, float b) {
    glBegin(GL_QUADS);
    glColor3f(r, g, b);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

// Function to display text on the screen
void renderBitmapString(float x, float y, float z, void *font, char *string) {
    char *c;
    glRasterPos3f(x, y, z);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

// Function to draw the terminal
void drawStripes(float x, float y, float width, float height, float stripeWidth, float r, float g, float b, float r2, float g2, float b2) {
    for (float i = 0; i < width; i += stripeWidth * 2) {
        drawRectangle(x + i, y, stripeWidth, height, r, g, b);
        drawRectangle(x + i + stripeWidth, y, stripeWidth, height, r2, g2, b2);
    }
}


// Function to draw the terminal
void drawTerminal() {
  // 1. Curved Canopy (Darker Brown)
    glColor3f(0.2, 0.15, 0.1);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0, 0.3);
    for (float angle = 0; angle <= PI; angle += PI / 200.0) {
        float x = 0.45 * cos(angle);
        float y = 0.3 * sin(angle) + 0.3;
        glVertex2f(x, y);
    }
    glEnd();

       // Central arch
    glBegin(GL_POLYGON);
    glColor3f(0.35, 0.25, 0.15); // Ash color
    for (float angle = 0; angle <= M_PI; angle += 0.01) {
        float x = 0.3 * cos(angle);
        float y = 0.3 * sin(angle) + 0.3;
        glVertex2f(x, y);
    }
    glEnd();

    // Main building
    drawRectangle(-0.8, 0.0, 1.6, 0.3, 1.9f, 1.9f, 1.9f);

    // Windows
    float windowX[] = {-0.64, -0.46, -0.27, 0.15, 0.34, 0.53};
    for (int i = 0; i < 6; i++) {
        drawStripes(windowX[i], 0.1, 0.1, 0.1, 0.003, 0.5, 0.8, 0.9, 0.2, 0.2, 0.4);
    }

    // Door
    drawRectangle(-0.1, 0.0, 0.19, 0.18, 0.5, 0.5, 0.5);

    // Side buildings
    drawRectangle(-0.8, 0.0, 0.15, 0.4, 0.698f, 0.745f, 0.710f);
    drawRectangle(0.7, 0.0, 0.1, 0.4, 0.698f, 0.745f, 0.710f);

    // Display the station name
    glColor3f(1.0, 1.0, 0.0);
    renderBitmapString(-0.18f, 0.32f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "BANGLADESH AIRPORT");
}
void drawBuilding(float x, float y, float width, float height, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void drawWindow(float x, float y, float width, float height) {
    glColor3f(1.0, 1.0, 1.0); // White color
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}


void drawBuildingWithWindows(float x, float y, float width, float height) {
    drawBuilding(x, y, width, height, 0.0, 0.0, 0.0); // Black building

    // Draw the windows on the building
    float windowWidth = 0.05;
    float windowHeight = 0.1;
    float windowSpacingX = 0.08;
    float windowSpacingY = 0.15;

    glColor3f(1.0, 1.0, 1.0);
    for (int i = 0; i < 3; ++i) { // Rows of windows
        for (int j = 0; j < 3; ++j) { // Columns of windows
            float windowX = x + 0.05 + j * windowSpacingX; // Adjust offsets as needed
            float windowY = y + 0.1 + i * windowSpacingY; // Adjust offsets as needed
            drawWindow(windowX, windowY, windowWidth, windowHeight);
        }
    }
}

// Function to draw the control tower
void drawControlTower() {
    // Base of the tower (tapered rectangle)
    glBegin(GL_POLYGON);
    glColor3f(0.3, 0.6, 0.7); // Light blue-gray
    glVertex2f(0.47, 0.3); // Bottom left
    glVertex2f(0.52, 0.3); // Bottom right
    glVertex2f(0.55, 0.55); // Top right (slightly narrower)
    glVertex2f(0.44, 0.55); // Top left (slightly narrower)
    glEnd();

    // Circular top of the tower
    glBegin(GL_POLYGON);
    glColor3f(0.5, 0.5, 0.5); // Light gray
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float x = 0.05 * cos(angle) + 0.5;
        float y = 0.05 * sin(angle) + 0.6;
        glVertex2f(x, y);
    }
    glEnd();

    // Antenna on top
    glBegin(GL_LINES);
    glColor3f(0.1, 0.1, 0.1); // Dark gray
    glVertex2f(0.5, 0.62); // Bottom of the antenna
    glVertex2f(0.5, 0.72); // Top of the antenna
    glEnd();

    // Base for the antenna
    glBegin(GL_POLYGON);
    glColor3f(0.3, 0.3, 0.3); // Dark gray
    glVertex2f(0.48, 0.62);
    glVertex2f(0.52, 0.62);
    glVertex2f(0.52, 0.65);
    glVertex2f(0.48, 0.65);
    glEnd();
}

// Function to draw the traffic light
void drawTrafficLight() {
    float baseX = -0.2; // Adjust position near horizontal runway
    float baseY = -0.36;
    float lightWidth = 0.03; // Smaller size
    float lightHeight = 0.09; // Smaller size

    // Pole
    drawRectangle(baseX + 0.01, baseY - 0.2, 0.005, 0.2, 0.1, 0.1, 0.1);

    // Light housing
    drawRectangle(baseX, baseY, lightWidth, lightHeight, 0.1, 0.1, 0.1);

    // Red light
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 0.0);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        glVertex2f(baseX + lightWidth / 2 + 0.005 * cos(angle), baseY + 0.06 + 0.005 * sin(angle));
    }
    glEnd();

    // Yellow light
    glBegin(GL_POLYGON);
    glColor3f(1.0, 1.0, 0.0);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        glVertex2f(baseX + lightWidth / 2 + 0.005 * cos(angle), baseY + 0.04 + 0.005 * sin(angle));
    }
    glEnd();

    // Green light
    glBegin(GL_POLYGON);
    glColor3f(0.0, 1.0, 0.0);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        glVertex2f(baseX + lightWidth / 2 + 0.005 * cos(angle), baseY + 0.02 + 0.005 * sin(angle));
    }
    glEnd();
}

// Function to draw the runway
void drawRunway() {
    float offsetY = -0.3; // Adjust the entire runway downward
    float runwayWidth = 0.3; // Increased runway width

    // Vertical runway
    drawRectangle(-0.15, -0.7 + offsetY, runwayWidth, 1.0, 0.3, 0.3, 0.3); // Vertical tarmac

    // Horizontal runway
    drawRectangle(-1.0, -0.2 + offsetY, 2.0, runwayWidth, 0.3, 0.3, 0.3); // Horizontal tarmac

    // Runway centerlines (vertical dashed line)
    for (float y = -0.9; y <= -0.2; y += 0.2) { // Exclude overlapping region
        drawRectangle(-0.01, y + offsetY, 0.02, 0.1, 1.0, 1.0, 1.0);
    }
    for (float y = 0.09; y <= 0.1; y += 0.2) { // Above the overlap
        drawRectangle(-0.01, y + offsetY, 0.02, 0.1, 1.0, 1.0, 1.0);
    }

    // Runway centerlines (horizontal dashed line)
    for (float x = -0.9; x <= 0.9; x += 0.2) {
        drawRectangle(x, -0.03 + offsetY, 0.1, 0.04, 1.0, 1.0, 1.0);
    }

    // Side boundary lines (vertical)
    drawRectangle(-0.16, -0.7 + offsetY, 0.01, 0.5, 1.0, 1.0, 1.0); // Left boundary below overlap
    drawRectangle(0.14, -0.7 + offsetY, 0.01, 0.5, 1.0, 1.0, 1.0);  // Right boundary below overlap
    drawRectangle(-0.16, 0.1 + offsetY, 0.01, 0.2, 1.0, 1.0, 1.0);  // Left boundary above overlap
    drawRectangle(0.14, 0.1 + offsetY, 0.01, 0.2, 1.0, 1.0, 1.0);   // Right boundary above overlap

    // Side boundary lines (horizontal)
    drawRectangle(-1.0, -0.22 + offsetY, 0.85, 0.02, 1.0, 1.0, 1.0); // Left boundary before overlap
    drawRectangle(0.15, -0.22 + offsetY, 0.9, 0.02, 1.0, 1.0, 1.0);  // Right boundary after overlap
    drawRectangle(-1.0, 0.1 + offsetY, 0.85, 0.02, 1.0, 1.0, 1.0);   // Left boundary before overlap
    drawRectangle(0.15, 0.1 + offsetY, 0.9, 0.02, 1.0, 1.0, 1.0);    // Right boundary after overlap
}

// Function to draw clouds
void drawCloud(float x, float y) {
    x += cloudOffset;

    glBegin(GL_POLYGON);
    glColor3f(1.0, 1.0, 1.0);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float dx = 0.1 * cos(angle);
        float dy = 0.05 * sin(angle);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();

    glBegin(GL_POLYGON);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float dx = 0.08 * cos(angle);
        float dy = 0.04 * sin(angle);
        glVertex2f(x + 0.12 + dx, y + dy);
    }
    glEnd();

    glBegin(GL_POLYGON);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float dx = 0.08 * cos(angle);
        float dy = 0.04 * sin(angle);
        glVertex2f(x - 0.12 + dx, y + dy);
    }
    glEnd();
}


// Function to draw trees
void drawTree(float x, float y) {
    // Trunk
    drawRectangle(x, y, 0.05, 0.15, 0.55, 0.27, 0.07);

    // Leaves (layered circles)
    glBegin(GL_POLYGON);
    glColor3f(0.2, 0.8, 0.2);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float dx = 0.1 * cos(angle);
        float dy = 0.1 * sin(angle);
        glVertex2f(x + 0.025 + dx, y + 0.2 + dy);
    }
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.2, 0.7, 0.2);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float dx = 0.08 * cos(angle);
        float dy = 0.08 * sin(angle);
        glVertex2f(x + 0.025 + dx, y + 0.3 + dy);
    }
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.2, 0.6, 0.2);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float dx = 0.06 * cos(angle);
        float dy = 0.06 * sin(angle);
        glVertex2f(x + 0.025 + dx, y + 0.38 + dy);
    }
    glEnd();
}


// Function to draw a single palm leaf (now a more elongated shape)
void drawPalmLeaf(float x, float y, float width, float height, float angle) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);

    glColor3f(0.0f, 0.6f, 0.0f); // Darker green

    glBegin(GL_QUADS);
    // Adjust these values to control leaf shape
    glVertex2f(0, 0); // Base
    glVertex2f(width * 0.2, height * 0.8); // Right side
    glVertex2f(0, height); // Top
    glVertex2f(-width * 0.2, height * 0.8); // Left side
    glEnd();

    glPopMatrix();
}

void PalmTree(float x, float y, float size) {
    // Trunk (Brown, Tapered)
    glColor3f(0.5f, 0.35f, 0.05f);
    glBegin(GL_POLYGON);
    glVertex2f(x - size * 0.04, y);
    glVertex2f(x + size * 0.04, y);
    glVertex2f(x + size * 0.01, y + size * 0.6); // Tapered top
    glVertex2f(x - size * 0.01, y + size * 0.6); // Tapered top
    glEnd();


   // --- Leaves ---
    int numLeaves = 10;
    float leafWidth = size * 0.25;
    float leafHeight = size * 0.6;
    float angleStep = 360.0f / numLeaves;

    float crownRadius = size * 0.4;

    // Seed the random number generator ONCE at the beginning of your program
    static bool seeded = false; // Ensure seeding only happens once
    if (!seeded) {
        srand(time(0)); // Seed with the current time
        seeded = true;
    }

    for (int i = 0; i < numLeaves; ++i) {
        float angle = i * angleStep;

        // ... (Leaf position calculation remains the same)

        float leafAngle = angle + 90.0f;
float leafX = 0.0f; // Initialize to 0
        float leafY = 0.0f; // Initialize to 0

        // Calculate leaf position offset from center - on a circle
        leafX = x + crownRadius * cos(angle * M_PI / 180.0f);
        leafY = y + size * 0.7 + crownRadius * sin(angle * M_PI / 180.0f);
        // Add some randomness to the angle (NOW CORRECTED)
      //  float angleRandomness = 20.0f;
       // leafAngle += (rand() % (int)(2 * angleRandomness + 1)) - angleRandomness; // + 1 to avoid 0

        drawPalmLeaf(leafX, leafY, leafWidth, leafHeight, leafAngle);
    }
}


void treebig()
{

    /**....................................... Big Tree (Left) ..........................................**/


    /**....................................... Big Tree (Left) Stem ..........................................**/

    glPushMatrix();
    glTranslated(-1.66f,-0.19f,0.0f);
    glBegin(GL_QUADS);
    glColor3f(0.314, 0.184, 0.149);
    glVertex2f(0.73f, 0.19f);
    glVertex2f(0.744f, 0.472f);
    glVertex2f(0.754f, 0.472f);
    glVertex2f(0.764f, 0.19f);

    glVertex2f(0.738f, 0.255f);
    glVertex2f(0.736f, 0.274f);
    glVertex2f(0.684f, 0.303f);
    glVertex2f(0.680f, 0.298f);

    glVertex2f(0.759f, 0.303f);
    glVertex2f(0.759f, 0.319f);
    glVertex2f(0.815f, 0.350f);
    glVertex2f(0.824f, 0.347f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(0.314, 0.184, 0.149);
    glVertex2f(0.744f, 0.408f);
    glVertex2f(0.687f, 0.460f);
    glVertex2f(0.744f, 0.424f);

    glVertex2f(0.755f, 0.421f);
    glVertex2f(0.815f, 0.493f);
    glVertex2f(0.755f, 0.439f);
    glEnd();
    glPopMatrix();
}

void tree_leaf1()
{
    /**....................................... Big Tree (Left) Top Leaf ..........................................**/
    glPushMatrix();
    glTranslated(-1.7f,-0.14f,0.0f);

    drawCircle(0.644,0.532,0.06);

    glColor3f(0.549, 0.639, 0.153);
    drawCircle(0.644,0.477,0.05);

    glColor3f(0.647, 0.745, 0.184);
    drawCircle(0.691,0.568,0.07);

    glColor3f(0.647, 0.745, 0.184);
    drawCircle(0.771,0.563,0.06);

    glColor3f(0.694, 0.78, 0.294);
    drawCircle(0.814,0.562,0.045);

    glColor3f(0.694, 0.78, 0.294);
    drawCircle(0.835,0.543,0.05);

    glColor3f(0.694, 0.78, 0.294);
    drawCircle(0.87,0.438,0.05);

    glColor3f(0.647, 0.745, 0.184);
    drawCircle(0.749,0.457,0.06);

    glColor3f(0.694, 0.78, 0.294);
    drawCircle(0.796,0.485,0.05);

    glColor3f(0.647, 0.745, 0.184);
    drawCircle(0.709,0.442,0.05);

    glColor3f(0.647, 0.745, 0.184);
    drawCircle(0.89,0.517,0.055);

    /**....................................... Big Tree (Left) Right Leaf ..........................................**/

    glColor3f(0.549, 0.639, 0.153);
    drawCircle(0.855,0.276,0.035);

    glColor3f(0.647, 0.745, 0.184);
    drawCircle(0.896,0.290,0.025);

    glColor3f(0.694, 0.78, 0.294);
    drawCircle(0.836,0.306,0.03);

    glColor3f(0.647, 0.745, 0.184);
    drawCircle(0.881,0.327,0.03);


    /**....................................... Big Tree (Left) Left Leaf ..........................................**/

    glPushMatrix();
    glTranslated(-0.14f,-0.11f,0.0f);

    glColor3f(0.549, 0.639, 0.153);
    drawCircle(0.865,0.376,0.035);

    glColor3f(0.647, 0.745, 0.184);
    drawCircle(0.896,0.390,0.025);

    glColor3f(0.694, 0.78, 0.294);
    drawCircle(0.835,0.406,0.03);

    glColor3f(0.694, 0.78, 0.294);
    drawCircle(0.802,0.384,0.03);

    glColor3f(0.549, 0.639, 0.153);
    drawCircle(0.836,0.366,0.035);

    glColor3f(0.647, 0.745, 0.184);
    drawCircle(0.881,0.408,0.03);

    glPopMatrix();

    glPopMatrix();

}

// Function to draw the sun
void drawSun(float x, float y, float radius) {
    glBegin(GL_POLYGON);
    glColor3f(1.0, 1.0, 0.5);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float dx = radius * cos(angle);
        float dy = radius * sin(angle);
        glVertex2f(x + dx, y + dy);
    }

        glEnd();


    // Add a glow effect around the sun (optional, but adds a nice touch)
    glEnable(GL_BLEND); // Enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Standard alpha blending

    for (float r = radius; r < radius + 0.04; r += 0.02) { // Larger radius for glow
        float alpha = 0.4 - (r - radius) / 0.2; // Alpha decreases with distance
        glColor4f(1.0, 1.0, 0.0, alpha); // Yellow with alpha (transparency)
        glBegin(GL_POLYGON);
        for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
            float dx = r * cos(angle);
            float dy = r * sin(angle);
            glVertex2f(x + dx, y + dy);
        }
        glEnd();
    }
    glDisable(GL_BLEND);
}

// Drawing Of Birds
void drawCircle(float x, float y, float radius);

// Birds
void circle_bird(float x1, float y1, float r) {
    int i;

    GLfloat x = x1;
    GLfloat y = y1;
    GLfloat radius = r;
    int triangleAmount = 100; // # of lines used to draw circle

    // Get the aspect ratio of the window
    int width2 = glutGet(GLUT_WINDOW_WIDTH);
    int height2 = glutGet(GLUT_WINDOW_HEIGHT);
    GLfloat aspect2 = (GLfloat)height2 / (GLfloat)width2; // Aspect ratio (height/width)

    // GLfloat radius = 0.8f; //radius
    GLfloat twicePi = 2.0f * PI;

    glColor3ub(80, 80, 80);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // center of circle
    for (i = 0; i <= triangleAmount; i++) {
        glVertex2f(x + (radius * cos(i * twicePi / triangleAmount) * aspect2),
                     y + (radius * sin(i * twicePi / triangleAmount)));
    }
    glEnd();
}

void circle(float x1, float y1, float r, char c) {
    int i;
    // Get the aspect ratio of the window
    int width2 = glutGet(GLUT_WINDOW_WIDTH);
    int height2 = glutGet(GLUT_WINDOW_HEIGHT);
    GLfloat aspect2 = (GLfloat)height2 / (GLfloat)width2; // Aspect ratio (height/width)

    GLfloat x = x1;
    GLfloat y = y1;
    GLfloat radius = r;
    int triangleAmount = 100; // # of lines used to draw circle

    // GLfloat radius = 0.8f; //radius
    GLfloat twicePi = 2.0f * PI;
    // glColor3f(0.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // center of circle
    for (i = 0; i <= triangleAmount; i++) {

        if (c == 's') {
            glVertex2f(x + (radius * cos(i * twicePi / triangleAmount)),
                         y + (radius * sin(i * twicePi / triangleAmount)));

        } else {
            glVertex2f(x + (radius * cos(i * twicePi / triangleAmount)),
                         y + (radius * sin(i * twicePi / triangleAmount)));

        }
    }
    glEnd();
}

// Function to draw a bird
void draw_bird(float xAdjust, float yAdjust, int birdType) {
    int wingColorR = 255, wingColorG = 140, wingColorB = 0;
    int bodyColorR = 255, bodyColorG = 255, bodyColorB = 0;
    int beakColorR = 190, beakColorG = 0, beakColorB = 0;

    if (birdType == 1) { // Green birds
        wingColorR = 0; wingColorG = 215; wingColorB = 0;
        bodyColorR = 0; bodyColorG = 100; bodyColorB = 0;
    } else if (birdType == 2) { // Black birds
        wingColorR = 0; wingColorG = 0; wingColorB = 0;
        bodyColorR = 50; bodyColorG = 50; bodyColorB = 50;
        beakColorR = 0; beakColorG = 0; beakColorB = 0;
    }

    glColor3ub(wingColorR, wingColorG, wingColorB);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.5f + xAdjust, 0.508f + yAdjust);
    glVertex2f(-0.5f + xAdjust, 0.492f + yAdjust);
    glVertex2f(-0.48f + xAdjust, 0.50f + yAdjust);
    glEnd();

    glColor3ub(bodyColorR, bodyColorG, bodyColorB);
    glBegin(GL_QUADS);
    glVertex2f(-0.51f + xAdjust, 0.49f + yAdjust);
    glVertex2f(-0.53f + xAdjust, 0.55f + yAdjust);
    glVertex2f(-0.53f + xAdjust, 0.52f + yAdjust);
    glVertex2f(-0.52f + xAdjust, 0.47f + yAdjust);
    glEnd();

    glColor3ub(0, 0, 0);
    circle_bird(-0.5 + xAdjust, 0.5 + yAdjust, 0.01);

    glColor3ub(beakColorR, beakColorG, beakColorB);
    circle(-0.52 + xAdjust, 0.49 + yAdjust, 0.015, 'l');

    glColor3ub(bodyColorR, bodyColorG, bodyColorB);
    glBegin(GL_POLYGON);
    glVertex2f(-0.52f + xAdjust, 0.49f + yAdjust);
    glVertex2f(-0.55f + xAdjust, 0.47f + yAdjust);
    glVertex2f(-0.535f + xAdjust, 0.465f + yAdjust);
    glVertex2f(-0.55f + xAdjust, 0.43f + yAdjust);
    glVertex2f(-0.515f + xAdjust, 0.485f + yAdjust);
    glEnd();
}

// Bird data
float birdX[8] = {-0.06f, -0.06f, -0.12f, -0.12f, -0.07f, -0.07f, -0.12f, 1.2f};  // Last bird starts from right
float birdY[8] = {0.05f, 0.09f, 0.03f, 0.07f, 0.25f, 0.29f, 0.24f, 0.21f};
int birdColorType[8] = {0, 1, 2, 0, 1, 2, 0, 1};
float birdMoveX[8] = {0.0055f, 0.0055f, 0.0035f, 0.0055f, 0.0035f, 0.0035f, 0.0055f, -0.007f}; // Last bird moves left
bool birdMovingRight[8] = {true, true, true, true, true, true, true, true};


// Function to draw the road light
void drawRoadLight(float x, float y) {
    // Base of the pole
    drawRectangle(x - 0.02, y, 0.04, 0.05, 0.2, 0.2, 0.2); // Dark gray

    // Pole
    drawRectangle(x - 0.005, y + 0.05, 0.01, 0.4, 0.5, 0.5, 0.5); // Light gray

    // Y-shaped structure
    glBegin(GL_QUADS);
    glColor3f(0.5, 0.5, 0.5); // light gray
    glVertex2f(x - 0.005, y + 0.4);
    glVertex2f(x + 0.005, y + 0.4);
    glVertex2f(x + 0.05, y + 0.55);
    glVertex2f(x + 0.04, y + 0.55);

    glVertex2f(x - 0.005, y + 0.4);
    glVertex2f(x + 0.005, y + 0.4);
    glVertex2f(x - 0.05, y + 0.55);
    glVertex2f(x - 0.04, y + 0.55);
    glEnd();

    // Left lamp
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 0.0); // Light Yellow
    glVertex2f(x - 0.06, y + 0.53);
    glVertex2f(x - 0.03, y + 0.53);
    glVertex2f(x - 0.03, y + 0.58);
    glVertex2f(x - 0.06, y + 0.58);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.2, 0.2, 0.2); //  Dark gray
    glVertex2f(x - 0.06, y + 0.58);
    glVertex2f(x - 0.03, y + 0.58);
    glVertex2f(x - 0.03, y + 0.61);
    glVertex2f(x - 0.06, y + 0.61);
    glEnd();

    // Right lamp
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 0.0); // Light Yellow
    glVertex2f(x + 0.03, y + 0.53);
    glVertex2f(x + 0.06, y + 0.53);
    glVertex2f(x + 0.06, y + 0.58);
    glVertex2f(x + 0.03, y + 0.58);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.2, 0.2, 0.2); //  dark gray
    glVertex2f(x + 0.03, y + 0.58);
    glVertex2f(x + 0.06, y + 0.58);
    glVertex2f(x + 0.06, y + 0.61);
    glVertex2f(x + 0.03, y + 0.61);
    glEnd();
}
//Human
void human1()
{

    /**..........................................Human...............................................**/
    ///..........Head..........
    glPushMatrix();
    glTranslatef(-0.01f,-0.37f,0.0f);
    glScalef(0.9f,0.9f,0.0f);
    GLfloat x=0.3f;
    GLfloat y=0.28f;
    GLfloat radius =0.01f;
    int triangleAmount = 100;
    GLfloat twicePi = 1.0f * PI;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );
    }
    glEnd();

    x=0.3f;
    y=0.28f;
    radius =0.01f;

    triangleAmount = 100;
    twicePi = -1.0f * PI;

    glColor3f(0.702f, 0.447f, 0.240f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }
    glEnd();

/// Hair bun for the head
    x=0.284f;
    y=0.278f;
    radius =0.0087f;

    triangleAmount = 100;
    twicePi = 2.0f * PI;

    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }

    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.296f, 0.278f);
    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.306f, 0.278f);
    glEnd();

    glPointSize(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.298f, 0.275f);
    glVertex2f(0.304f, 0.275f);
    glEnd();

    ///.........................Body.....................
    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.0f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///.........................Shari.....................
    glBegin(GL_POLYGON);
    glColor3f(1.4f, 0.540f, 1.10f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glVertex2f(0.31f, 0.26f);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.248f);
    glVertex2f(0.29f, 0.232f);
    glVertex2f(0.296f, 0.232f);
    glVertex2f(0.304f, 0.232f);
    glVertex2f(0.31f, 0.232f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///....................Hand Right....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.254f);
    glVertex2f(0.318f, 0.25f);
    glVertex2f(0.31f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.252f);
    glVertex2f(0.318f, 0.254f);
    glVertex2f(0.31f, 0.265f);
    glEnd();



    ///....................Hand left....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.254f);
    glVertex2f(0.282f, 0.25f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.252f);
    glVertex2f(0.282f, 0.254f);
    glVertex2f(0.29f, 0.265f);
    glEnd();
    glPopMatrix();
}


void human2()
{
    ///..........Head..........
    glPushMatrix();
    glTranslatef(0.02f,-0.37f,0.0f);
    glScalef(0.9f,0.9f,0.0f);
    GLfloat x=0.3f;
    GLfloat y=0.28f;
    GLfloat radius =0.01f;
    int triangleAmount = 100;
    GLfloat twicePi = 1.0f * PI;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );
    }
    glEnd();

    x=0.3f;
    y=0.28f;
    radius =0.01f;

    triangleAmount = 100;
    twicePi = -1.0f * PI;

    glColor3f(0.702f, 0.447f, 0.240f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }

    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.296f, 0.278f);
    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.306f, 0.278f);
    glEnd();

    glPointSize(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.298f, 0.275f);
    glVertex2f(0.304f, 0.275f);
    glEnd();

    ///.........................Body.....................
    glBegin(GL_QUADS);
    glColor3f(0.9f, 0.7f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glLineWidth(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.3f, 0.273f);
    glVertex2f(0.3f, 0.25f);

    glEnd();

    ///....................Hand Right....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.254f);
    glVertex2f(0.318f, 0.25f);
    glVertex2f(0.31f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.252f);
    glVertex2f(0.318f, 0.254f);
    glVertex2f(0.31f, 0.265f);
    glEnd();



    ///....................Hand left....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.254f);
    glVertex2f(0.282f, 0.25f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.252f);
    glVertex2f(0.282f, 0.254f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    ///................................Legs............................

    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.29f, 0.23f);
    glVertex2f(0.296f, 0.23f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.304f, 0.23f);
    glVertex2f(0.31f, 0.23f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.698f, 0.745f, 0.710f);
    glVertex2f(0.29f, 0.248f);
    glVertex2f(0.29f, 0.232f);
    glVertex2f(0.296f, 0.232f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.304f, 0.232f);
    glVertex2f(0.31f, 0.232f);
    glVertex2f(0.31f, 0.25f);
    glEnd();
    glPopMatrix();
}
void human3()
{
    ///..........Head..........
    glPushMatrix();
    glTranslatef(0.07f,-0.37f,0.0f);
    glScalef(0.9f,0.9f,0.0f);
    GLfloat x=0.3f;
    GLfloat y=0.28f;
    GLfloat radius =0.01f;
    int triangleAmount = 100;
    GLfloat twicePi = 1.0f * PI;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );
    }
    glEnd();

    x=0.3f;
    y=0.28f;
    radius =0.01f;

    triangleAmount = 100;
    twicePi = -1.0f * PI;

    glColor3f(0.702f, 0.447f, 0.240f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }

    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.296f, 0.278f);
    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.306f, 0.278f);
    glEnd();

    glPointSize(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.298f, 0.275f);
    glVertex2f(0.304f, 0.275f);
    glEnd();

    ///.........................Body.....................
    glBegin(GL_QUADS);
    glColor3f(0.3, 0.3, 0.6);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glLineWidth(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.3f, 0.273f);
    glVertex2f(0.3f, 0.25f);

    glEnd();

    ///....................Hand Right....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.254f);
    glVertex2f(0.318f, 0.25f);
    glVertex2f(0.31f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.252f);
    glVertex2f(0.318f, 0.254f);
    glVertex2f(0.31f, 0.265f);
    glEnd();



    ///....................Hand left....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.254f);
    glVertex2f(0.282f, 0.25f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.252f);
    glVertex2f(0.282f, 0.254f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    ///................................Legs............................

    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.29f, 0.23f);
    glVertex2f(0.296f, 0.23f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.304f, 0.23f);
    glVertex2f(0.31f, 0.23f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.698f, 0.745f, 0.710f);
    glVertex2f(0.29f, 0.248f);
    glVertex2f(0.29f, 0.232f);
    glVertex2f(0.296f, 0.232f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.304f, 0.232f);
    glVertex2f(0.31f, 0.232f);
    glVertex2f(0.31f, 0.25f);
    glEnd();
    glPopMatrix();
}

void human4()
{
    ///..........Head..........
    glPushMatrix();
    glTranslatef(0.09f,-0.32f,0.0f);
    glScalef(0.9f,0.9f,0.0f);
    GLfloat x=0.3f;
    GLfloat y=0.28f;
    GLfloat radius =0.01f;
    int triangleAmount = 100;
    GLfloat twicePi = 1.0f * PI;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );
    }
    glEnd();

    x=0.3f;
    y=0.28f;
    radius =0.01f;

    triangleAmount = 100;
    twicePi = -1.0f * PI;

    glColor3f(0.702f, 0.447f, 0.240f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }

    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.296f, 0.278f);
    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.306f, 0.278f);
    glEnd();

    glPointSize(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.298f, 0.275f);
    glVertex2f(0.304f, 0.275f);
    glEnd();

    ///.........................Body.....................
    glBegin(GL_QUADS);
    glColor3f(0.9f, 0.7f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glLineWidth(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.3f, 0.273f);
    glVertex2f(0.3f, 0.25f);

    glEnd();

    ///....................Hand Right....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.254f);
    glVertex2f(0.318f, 0.25f);
    glVertex2f(0.31f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.252f);
    glVertex2f(0.318f, 0.254f);
    glVertex2f(0.31f, 0.265f);
    glEnd();



    ///....................Hand left....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.254f);
    glVertex2f(0.282f, 0.25f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.252f);
    glVertex2f(0.282f, 0.254f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    ///................................Legs............................

    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.29f, 0.23f);
    glVertex2f(0.296f, 0.23f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.304f, 0.23f);
    glVertex2f(0.31f, 0.23f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.0, 0.0, 0.0);
    glVertex2f(0.29f, 0.248f);
    glVertex2f(0.29f, 0.232f);
    glVertex2f(0.296f, 0.232f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.304f, 0.232f);
    glVertex2f(0.31f, 0.232f);
    glVertex2f(0.31f, 0.25f);
    glEnd();
    glPopMatrix();
}

void human5()
{

    /**..........................................Human...............................................**/
    ///..........Head..........
    glPushMatrix();
    glTranslatef(0.17f,-0.37f,0.0f);
    glScalef(0.9f,0.9f,0.0f);
    GLfloat x=0.3f;
    GLfloat y=0.28f;
    GLfloat radius =0.01f;
    int triangleAmount = 100;
    GLfloat twicePi = 1.0f * PI;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );
    }
    glEnd();

    x=0.3f;
    y=0.28f;
    radius =0.01f;

    triangleAmount = 100;
    twicePi = -1.0f * PI;

    glColor3f(0.702f, 0.447f, 0.240f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }
    glEnd();

/// Hair bun for the head
    x=0.284f;
    y=0.278f;
    radius =0.0087f;

    triangleAmount = 100;
    twicePi = 2.0f * PI;

    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }

    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.296f, 0.278f);
    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.306f, 0.278f);
    glEnd();

    glPointSize(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.298f, 0.275f);
    glVertex2f(0.304f, 0.275f);
    glEnd();

    ///.........................Body.....................
    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.0f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///.........................Shari.....................
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glVertex2f(0.31f, 0.26f);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.248f);
    glVertex2f(0.29f, 0.232f);
    glVertex2f(0.296f, 0.232f);
    glVertex2f(0.304f, 0.232f);
    glVertex2f(0.31f, 0.232f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///....................Hand Right....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.254f);
    glVertex2f(0.318f, 0.25f);
    glVertex2f(0.31f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.252f);
    glVertex2f(0.318f, 0.254f);
    glVertex2f(0.31f, 0.265f);
    glEnd();



    ///....................Hand left....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.254f);
    glVertex2f(0.282f, 0.25f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.252f);
    glVertex2f(0.282f, 0.254f);
    glVertex2f(0.29f, 0.265f);
    glEnd();
    glPopMatrix();
}

void human6()
{

    /**..........................................Human...............................................**/
    ///..........Head..........
    glPushMatrix();
    glTranslatef(0.2f,-0.37f,0.0f);
    glScalef(0.9f,0.9f,0.0f);
    GLfloat x=0.3f;
    GLfloat y=0.28f;
    GLfloat radius =0.01f;
    int triangleAmount = 100;
    GLfloat twicePi = 1.0f * PI;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );
    }
    glEnd();

    x=0.3f;
    y=0.28f;
    radius =0.01f;

    triangleAmount = 100;
    twicePi = -1.0f * PI;

    glColor3f(0.702f, 0.447f, 0.240f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }
    glEnd();

/// Hair bun for the head
    x=0.284f;
    y=0.278f;
    radius =0.0087f;

    triangleAmount = 100;
    twicePi = 2.0f * PI;

    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }

    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.296f, 0.278f);
    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.306f, 0.278f);
    glEnd();

    glPointSize(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.298f, 0.275f);
    glVertex2f(0.304f, 0.275f);
    glEnd();

    ///.........................Body.....................
    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.0f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///.........................Shari.....................
    glBegin(GL_POLYGON);
    glColor3f(0.54, 0.77, 0.8);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glVertex2f(0.31f, 0.26f);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.3, 0.3, 0.6);
    glVertex2f(0.29f, 0.248f);
    glVertex2f(0.29f, 0.232f);
    glVertex2f(0.296f, 0.232f);
    glVertex2f(0.304f, 0.232f);
    glVertex2f(0.31f, 0.232f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///....................Hand Right....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.254f);
    glVertex2f(0.318f, 0.25f);
    glVertex2f(0.31f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.252f);
    glVertex2f(0.318f, 0.254f);
    glVertex2f(0.31f, 0.265f);
    glEnd();



    ///....................Hand left....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.254f);
    glVertex2f(0.282f, 0.25f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.252f);
    glVertex2f(0.282f, 0.254f);
    glVertex2f(0.29f, 0.265f);
    glEnd();
    glPopMatrix();
}

void human7()
{

    /**..........................................Human...............................................**/
    ///..........Head..........
    glPushMatrix();
    glTranslatef(0.33f,-0.37f,0.0f);
    glScalef(0.9f,0.9f,0.0f);
    GLfloat x=0.3f;
    GLfloat y=0.28f;
    GLfloat radius =0.01f;
    int triangleAmount = 100;
    GLfloat twicePi = 1.0f * PI;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );
    }
    glEnd();

    x=0.3f;
    y=0.28f;
    radius =0.01f;

    triangleAmount = 100;
    twicePi = -1.0f * PI;

    glColor3f(0.702f, 0.447f, 0.240f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }
    glEnd();

/// Hair bun for the head
    x=0.284f;
    y=0.278f;
    radius =0.0087f;

    triangleAmount = 100;
    twicePi = 2.0f * PI;

    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }

    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.296f, 0.278f);
    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.306f, 0.278f);
    glEnd();

    glPointSize(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.298f, 0.275f);
    glVertex2f(0.304f, 0.275f);
    glEnd();

    ///.........................Body.....................
    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.0f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///.........................Shari.....................
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glVertex2f(0.31f, 0.26f);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.248f);
    glVertex2f(0.29f, 0.232f);
    glVertex2f(0.296f, 0.232f);
    glVertex2f(0.304f, 0.232f);
    glVertex2f(0.31f, 0.232f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///....................Hand Right....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.254f);
    glVertex2f(0.318f, 0.25f);
    glVertex2f(0.31f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.252f);
    glVertex2f(0.318f, 0.254f);
    glVertex2f(0.31f, 0.265f);
    glEnd();



    ///....................Hand left....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.254f);
    glVertex2f(0.282f, 0.25f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.252f);
    glVertex2f(0.282f, 0.254f);
    glVertex2f(0.29f, 0.265f);
    glEnd();
    glPopMatrix();
}
void human8()
{

    /**..........................................Human...............................................**/
    ///..........Head..........
    glPushMatrix();
    glTranslatef(0.39f,-0.37f,0.0f);
    glScalef(0.9f,0.9f,0.0f);
    GLfloat x=0.3f;
    GLfloat y=0.28f;
    GLfloat radius =0.01f;
    int triangleAmount = 100;
    GLfloat twicePi = 1.0f * PI;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );
    }
    glEnd();

    x=0.3f;
    y=0.28f;
    radius =0.01f;

    triangleAmount = 100;
    twicePi = -1.0f * PI;

    glColor3f(0.702f, 0.447f, 0.240f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }
    glEnd();

/// Hair bun for the head
    x=0.284f;
    y=0.278f;
    radius =0.0087f;

    triangleAmount = 100;
    twicePi = 2.0f * PI;

    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }

    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.296f, 0.278f);
    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.306f, 0.278f);
    glEnd();

    glPointSize(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.298f, 0.275f);
    glVertex2f(0.304f, 0.275f);
    glEnd();

    ///.........................Body.....................
    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.0f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///.........................Shari.....................
    glBegin(GL_POLYGON);
    glColor3f(0.5, 0.27, 0.5);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glVertex2f(0.31f, 0.26f);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.5, 0.27, 0.5);
    glVertex2f(0.29f, 0.248f);
    glVertex2f(0.29f, 0.232f);
    glVertex2f(0.296f, 0.232f);
    glVertex2f(0.304f, 0.232f);
    glVertex2f(0.31f, 0.232f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///....................Hand Right....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.254f);
    glVertex2f(0.318f, 0.25f);
    glVertex2f(0.31f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.252f);
    glVertex2f(0.318f, 0.254f);
    glVertex2f(0.31f, 0.265f);
    glEnd();



    ///....................Hand left....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.254f);
    glVertex2f(0.282f, 0.25f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.252f);
    glVertex2f(0.282f, 0.254f);
    glVertex2f(0.29f, 0.265f);
    glEnd();
    glPopMatrix();
}

void human9()
{
    ///..........Head..........
    glPushMatrix();
    glTranslatef(0.2f,-0.3f,0.0f);
    glScalef(0.9f,0.9f,0.0f);
    GLfloat x=0.3f;
    GLfloat y=0.28f;
    GLfloat radius =0.01f;
    int triangleAmount = 100;
    GLfloat twicePi = 1.0f * PI;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );
    }
    glEnd();

    x=0.3f;
    y=0.28f;
    radius =0.01f;

    triangleAmount = 100;
    twicePi = -1.0f * PI;

    glColor3f(0.702f, 0.447f, 0.240f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }

    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.296f, 0.278f);
    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.306f, 0.278f);
    glEnd();

    glPointSize(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.298f, 0.275f);
    glVertex2f(0.304f, 0.275f);
    glEnd();

    ///.........................Body.....................
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 0.0);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glLineWidth(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.3f, 0.273f);
    glVertex2f(0.3f, 0.25f);

    glEnd();

    ///....................Hand Right....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.254f);
    glVertex2f(0.318f, 0.25f);
    glVertex2f(0.31f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.252f);
    glVertex2f(0.318f, 0.254f);
    glVertex2f(0.31f, 0.265f);
    glEnd();



    ///....................Hand left....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.254f);
    glVertex2f(0.282f, 0.25f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.252f);
    glVertex2f(0.282f, 0.254f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    ///................................Legs............................

    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.29f, 0.23f);
    glVertex2f(0.296f, 0.23f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.304f, 0.23f);
    glVertex2f(0.31f, 0.23f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.0, 0.0, 0.0);
    glVertex2f(0.29f, 0.248f);
    glVertex2f(0.29f, 0.232f);
    glVertex2f(0.296f, 0.232f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.304f, 0.232f);
    glVertex2f(0.31f, 0.232f);
    glVertex2f(0.31f, 0.25f);
    glEnd();
    glPopMatrix();
}

void drawWheel(float x, float y, float radius) {
    glColor3f(0.0, 0.0, 0.0); // Black
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= 30; i++) {
        float angle = 2.0 * M_PI * i / 30;
        glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
    }
    glEnd();
}


void drawPlane(float x, float y, float angle) {
    glPushMatrix();
    glTranslatef(x, y, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);

    // Plane body
    glBegin(GL_POLYGON); // Use GL_POLYGON for a more defined shape
    glColor3f(2.7, 2.7,0.9); // White
    glVertex2f(-0.34, 0.0);
    glVertex2f(-0.3, 0.07);
    glVertex2f(-0.18, 0.12);
    glVertex2f(0.18, 0.12);
    glVertex2f(0.3, 0.07);
    glVertex2f(0.3, 0.0);
    glVertex2f(0.25, -0.07);
    glVertex2f(-0.25, -0.07);
    glEnd();

    // Red stripe
    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0); // Red
    glVertex2f(-0.31, -0.025);
    glVertex2f(0.28, -0.025);
    glVertex2f(0.28, -0.035);
    glVertex2f(-0.3, -0.035);
    glEnd();

    // Left Wing
    glBegin(GL_TRIANGLES);
    glColor3f(2.7, 2.7,0.9); // White
    glVertex2f(-0.03, -0.03);
    glVertex2f(0.07, -0.03);
    glVertex2f(0.2, -0.3);
    glEnd();

    // Right Wing
    /*glBegin(GL_TRIANGLES);
    glColor3f(1.0, 1.0, 1.0); // White
    glVertex2f(0.33, 0.13);
    glVertex2f(0.32, 0.1);
    glVertex2f(0.4, 0.07);
    glEnd();
*/
    // Windows
    glColor3f(0.54, 0.77, 0.8); // White

    glBegin(GL_QUADS);
    glVertex2f(-0.2, 0.07);
    glVertex2f(-0.15, 0.07);
    glVertex2f(-0.15, 0.03);
    glVertex2f(-0.2, 0.03);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(-0.1, 0.07);
    glVertex2f(-0.05, 0.07);
    glVertex2f(-0.05, 0.03);
    glVertex2f(-0.1, 0.03);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(0.0, 0.07);
    glVertex2f(0.05, 0.07);
    glVertex2f(0.05, 0.03);
    glVertex2f(0.0, 0.03);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(0.1, 0.07);
    glVertex2f(0.15, 0.07);
    glVertex2f(0.15, 0.03);
    glVertex2f(0.1, 0.03);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(0.2, 0.07);
    glVertex2f(0.25, 0.07);
    glVertex2f(0.25, 0.03);
    glVertex2f(0.2, 0.03);
    glEnd();

    // Tail
    glBegin(GL_TRIANGLES);
    glColor3f(2.7, 2.7,0.9); // Yellowish White
    glVertex2f(0.25, 0.07);
    glVertex2f(0.3, 0.0);
    glVertex2f(0.35, 0.2);
    glEnd();

    // Wheels
    drawWheel(-0.2, -0.08, 0.018);
    drawWheel(0.0, -0.08, 0.018);
    //drawWheel(0.08, -0.18, 0.018);

    glPopMatrix();
}

float planeAngle = 0.0f;
bool initialMovementDone = false;
bool isUserControlling = false;

void keyboard(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            isUserControlling = true;
            break;
    }
}

bool isSoundOn = false;
bool isSecondSoundOn = false;

void key_board(unsigned char key, int x, int y) {
    switch (key) {
        case 's':  // Press 'S' to play sound
            if (!isSoundOn) {
                PlaySound(TEXT("sound2.wav"), NULL, SND_ASYNC | SND_LOOP);
                isSoundOn = true;
            } else {  // Press 'S' again to stop sound
                PlaySound(NULL, NULL, SND_ASYNC);
                isSoundOn = false;
            }
            break;

        case 'b':  // Press 'B' to play another sound
            if (!isSecondSoundOn) {
                PlaySound(TEXT("sound.wav"), NULL, SND_ASYNC);
                isSecondSoundOn = true;
            } else {
                PlaySound(NULL, NULL, SND_ASYNC); // Stop sound
                isSecondSoundOn = false;
            }
            break;
    }
}

void keyUp(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            isUserControlling = false;
            break;

        case GLUT_KEY_LEFT:
            isUserControlling = false;
            break;

        default:
            glutIdleFunc(NULL);
            break;
    }
}

void update(int value) {
    cloudOffset += 0.002f;
    if (cloudOffset > 1.2f) {
        cloudOffset = -1.2f;
    }

    if (!initialMovementDone) {
        if (planeX > 0.1f) {
            planeX -= speed;
        } else {
            initialMovementDone = true;
        }
    }

    if (isUserControlling) {
        float angle = 45.0f;
        float angle_rad = angle * M_PI / 180.0f;

        planeX -= speed * cos(angle_rad);
        planeY += speed * sin(angle_rad);
        planeAngle += 1.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}


// Display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the scene
    drawSky();
    drawGround();
    drawTerminal();
    drawBuildingWithWindows(0.85, 0.0, 0.3, 0.6);
    drawControlTower();
    drawRunway();

    // Draw traffic light
    drawTrafficLight();

    // Draw trees
    //drawTree(0.85, -0.18);
    drawTree(-0.75, -0.55);
    drawTree(0.6, -0.02);
    drawTree(-0.7, -0.02);
 //Tree(0.0, -0.5, 0.4);  // Tree 1
    PalmTree(0.825, -0.18, 0.5);
    //Tree(0.825, -0.18, 0.5);
//    tree1();
    treebig();
    tree_leaf1();

// Draw the sun
    drawSun(0.8, 0.8, 0.1);

    // Draw clouds
    drawCloud(-0.5, 0.8);
    drawCloud(0.0, 0.9);
    drawCloud(0.5, 0.8);

    // Add a streetlight beside the horizontal runway
    drawRoadLight(-0.85, -0.18);

    // Draw birds in the sky
      for (int i = 0; i < 8; i++) {
        birdX[i] += birdMoveX[i]; // Move right
        if (birdX[i] > 1.5f) { // Reset when off-screen
            birdX[i] = -1.2f;
        }

        draw_bird(birdX[i], birdY[i], birdColorType[i]);
    }

    human1();
    human2();
    human3();
    human4();
    human5();
    human6();
    human7();
    human8();
    human9();

    // Draw the plane
    //drawPlane(0.55, -0.29, 0.0);
    //drawPlane(planeX, planeY, 45.0f);
    //drawPlane(planeX, planeY, angle);
    glPushMatrix();
    glTranslatef(planeX + 0.4, planeY - 0.02, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);  // Rotate around the new pivot
    //glTranslatef(-0.15, 0.05, 0.0); // Move back to original position

    drawPlane(0.0, 0.0, 0.0);
    glPopMatrix();

    glutSwapBuffers();
    glFlush();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Airport Scene");
    glClearColor(1.0, 1.0, 1.0, 1.0); // Background color
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // Coordinate system
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glutDisplayFunc(display);
    glutSpecialFunc(keyboard);
    glutKeyboardFunc(key_board);
    //glutSpecialUpFunc(keyUp);
    glutTimerFunc(10, update, 0);
   // sndPlaySound("sound.wav",SND_ASYNC);
    //sndPlaySound("sound2.wav",SND_ASYNC);

    glutMainLoop();

    return 0;
}

