#include "../headers/process.h"

extern float lastX, lastY, deltaTime;
extern unsigned int SCR_WIDTH, SCR_HEIGHT;
extern bool firstMouse;
extern Camera camera;

// Initializes GLFW and GLAD, defines OpenGL version and profile, creates window and set context
// Set callbacks and cursor input mode, enable depth testing
GLFWwindow* setupWindow(int WIDTH, int HEIGHT, const char* TITLE) {
    // Initialize GLFW
    glfwInit();
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // MSAA
    glfwWindowHint(GLFW_SAMPLES, 4);
    // Create window
    GLFWwindow* GLwindow = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
    if (GLwindow == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw "Failed to create GLFW window";
    }
    // make the context of our window the main context on the current thread
    glfwMakeContextCurrent(GLwindow);
    // tell GLFW we want to call this function on every window resize by registering it
    // lambda callback function on the window that gets called each time the window is resized
    // when the window is first displayed, this gets called as well
    glfwSetFramebufferSizeCallback(GLwindow, [](GLFWwindow* window, int width, int height) {
        // tell OpenGL the size of the rendering window
        glViewport(0, 0, width, height);
        });
    glfwSetCursorPosCallback(GLwindow, mouse_callback);
    glfwSetScrollCallback(GLwindow, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(GLwindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        throw "Failed to initialize GLAD";
    }
    // enable depth testing
    glEnable(GL_DEPTH_TEST);

    return GLwindow;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window) {
    static bool captMouse = false;
    static bool captMouseLock = true;

    // Close program
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    // Capture mouse
    if (glfwGetKey(window, GLFW_KEY_T) != GLFW_PRESS)
        captMouseLock = false;
    if (glfwGetKey(window, GLFW_KEY_T) != GLFW_RELEASE && !captMouseLock) {
        if (captMouse) firstMouse = true;
        glfwSetInputMode(window, GLFW_CURSOR, (captMouse = !captMouse) ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        captMouseLock = true;
    }
    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) return;
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}