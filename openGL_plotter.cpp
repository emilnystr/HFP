#include <GLFW/glfw3.h>
#include <cmath>

int main()
{
    if (!glfwInit())
        return -1;

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Fixed Pipeline Test", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // V-sync

    while (!glfwWindowShouldClose(window))
    {
        float t = glfwGetTime();

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Setup simple 2D projection
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1, 1, -1, 1, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Rotate triangle
        glRotatef(t * 50.0f, 0.0f, 0.0f, 1.0f);

        // Draw triangle
        glBegin(GL_TRIANGLES);

        glColor3f(1.0f, 0.2f, 0.2f);
        glVertex2f(0.0f, 0.6f);

        glColor3f(0.2f, 1.0f, 0.2f);
        glVertex2f(-0.6f, -0.6f);

        glColor3f(0.2f, 0.2f, 1.0f);
        glVertex2f(0.6f, -0.6f);

        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
