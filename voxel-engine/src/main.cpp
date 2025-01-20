#include "../headers/main.h"

const unsigned int SCR_WIDTH = 1280, SCR_HEIGHT = 720;
float lastX = SCR_WIDTH / 2.0f, lastY = SCR_HEIGHT / 2.0f;
float deltaTime = 0.0f, lastFrame = 0.0f;
bool firstMouse = true;
Camera camera(glm::vec3(50.0f, 85.0f, 100.0f));

int SCX = 10;
int SCY = 10;
int SCZ = 10;
int worldChunkRadius = 5;

int main() {
    GLFWwindow* window = setupWindow(SCR_WIDTH, SCR_HEIGHT, "Voxel Terrain");
    ChunkManager chunkManager;
    Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static bool wireframeCheckBox = false;
    float frustumFarBoundary = 500.0f;

    shader.use();

    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowSize(ImVec2((float)320, (float)210));
        {
            ImGui::Begin("Debug");
            if (ImGui::BeginTabBar("Tabs")) {
                if (ImGui::BeginTabItem("Debug")) {
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Press T to capture/uncapture mouse");
                    ImGui::Text("Avg FPS: %.2f", ImGui::GetIO().Framerate);
                    ImGui::Text("Camera pos. X: %.1f | Y: %.1f | Z: %.1f", camera.Position[0], camera.Position[1], camera.Position[2]);
                    ImGui::Text("Camera world pos. X: %.1f | Y: %.1f | Z: %.1f", floor(camera.Position[0] / CX), floor(camera.Position[1] / CY), floor(camera.Position[2] / CZ));
                    ImGui::Text("Blocks per chunk: %dx%dx%d", CX, CY, CZ);
                    ImGui::Text("World chunks: %dx%dx%d", SCX, SCY, SCZ);
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Settings")) {
                    ImGui::Checkbox("Wireframe", &wireframeCheckBox);
                    ImGui::Text("Frustum far boundary: ");
                    ImGui::SliderFloat("float", &frustumFarBoundary, 0.1f, 2000.0f);
                    ImGui::Text("World chunk size:");
                    ImGui::SliderInt("SCX", &SCX, 1, 100);
                    ImGui::SliderInt("SCY", &SCY, 1, 100);
                    ImGui::SliderInt("SCZ", &SCZ, 1, 100);
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("World")) {
                    ImGui::Text("World chunk radius:");
                    ImGui::SliderInt("Radius", &worldChunkRadius, 1, 50);
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::End();
        }

        glPolygonMode(GL_FRONT_AND_BACK, wireframeCheckBox ? GL_LINE : GL_FILL);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        ImGui::Render();

        glClearColor(55.0f / 255.0f, 184.0f / 255.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("light.ambient", 1.0f, 1.0f, 1.0f);
        shader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, frustumFarBoundary);
        shader.setMat4("projection", projection);

        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);

        glm::mat4 vp = projection * view;
        chunkManager.Render(shader, vp);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
