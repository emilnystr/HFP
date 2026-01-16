#include "plotter.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>
#include <iomanip>

TemperatureAnimator::TemperatureAnimator(int w, int h) 
    : width(w), height(h), window(nullptr), padding(80.0f),
      current_frame(0), is_playing(true), animation_speed(0.1f),
      minX(0), maxX(0), minY(0), maxY(0), animation_complete(false) {}

TemperatureAnimator::~TemperatureAnimator() {
    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}

void TemperatureAnimator::initGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }
    
    window = glfwCreateWindow(width, height, "HFP Engine", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    glfwSwapInterval(1); // VSync
    
    // Grundläggande OpenGL-inställningar
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.0f);
    glPointSize(5.0f);
}

void TemperatureAnimator::calculateBounds() {
    if (positions_mm.empty() || all_temperatures.empty()) return;
    
    // Hitta globala min/max
    minX = positions_mm.front();
    maxX = positions_mm.back();
    minY = all_temperatures[0][0];
    maxY = all_temperatures[0][0];
    
    for (const auto& temps : all_temperatures) {
        for (double temp : temps) {
            if (temp < minY) minY = temp;
            if (temp > maxY) maxY = temp;
        }
    }
    
    // Lägg till marginaler
    float xRange = maxX - minX;
    float yRange = maxY - minY;
    
    minX -= xRange * 0.05f;
    maxX += xRange * 0.05f;
    
    if (minY >= 0) minY = 0;
    else minY -= yRange * 0.1f;
    
    maxY += yRange * 0.1f;
    
    if (yRange < 1.0f) {
        minY -= 10.0f;
        maxY += 10.0f;
    }
}

void TemperatureAnimator::setData(const std::vector<double>& positions,
                                 const std::vector<std::vector<double>>& temperatures,
                                 const std::vector<double>& times) {
    positions_mm = positions;
    all_temperatures = temperatures;
    time_steps = times;
    
    if (!positions_mm.empty() && !all_temperatures.empty()) {
        calculateBounds();
    }
}

void TemperatureAnimator::renderGrid() {
    glColor4f(0.3f, 0.3f, 0.4f, 0.3f);
    glLineWidth(1.0f);
    
    // Vertikala linjer
    int numXGrid = 10;
    for (int i = 0; i <= numXGrid; i++) {
        float x = padding + (width - 2 * padding) * i / numXGrid;
        glBegin(GL_LINES);
        glVertex2f(x, padding);
        glVertex2f(x, height - padding);
        glEnd();
    }
    
    // Horisontella linjer
    int numYGrid = 8;
    for (int i = 0; i <= numYGrid; i++) {
        float y = padding + (height - 2 * padding) * i / numYGrid;
        glBegin(GL_LINES);
        glVertex2f(padding, y);
        glVertex2f(width - padding, y);
        glEnd();
    }
}

void TemperatureAnimator::renderAxes() {
    glColor3f(0.7f, 0.7f, 0.7f);
    glLineWidth(1.5f);
    
    // X-axel
    glBegin(GL_LINES);
    glVertex2f(padding, padding);
    glVertex2f(width - padding, padding);
    glEnd();
    
    // Y-axel
    glBegin(GL_LINES);
    glVertex2f(padding, padding);
    glVertex2f(padding, height - padding);
    glEnd();
}

void TemperatureAnimator::renderTemperatureCurve(const std::vector<double>& temperatures) {
    if (positions_mm.empty() || temperatures.empty()) return;
    
    float plotWidth = width - 2 * padding;
    float plotHeight = height - 2 * padding;
    float xRange = maxX - minX;
    float yRange = maxY - minY;
    
    if (plotWidth <= 0 || plotHeight <= 0 || xRange <= 0 || yRange <= 0) return;
    
    // Rita linjen
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    
    for (size_t i = 0; i < positions_mm.size(); i++) {
        float x = padding + ((positions_mm[i] - minX) / xRange) * plotWidth;
        float y = padding + ((temperatures[i] - minY) / yRange) * plotHeight;
        
        // Färggradient baserad på temperatur
        float tempRatio = (temperatures[i] - minY) / yRange;
        
        if (tempRatio < 0.33f) {
            glColor3f(0.0f, 0.5f, 1.0f); // Ljusblå
        } else if (tempRatio < 0.66f) {
            glColor3f(1.0f, 0.8f, 0.0f); // Guldgul
        } else {
            glColor3f(1.0f, 0.2f, 0.0f); // Orange-röd
        }
        
        glVertex2f(x, y);
    }
    
    glEnd();
    
    // Rita punkterna
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    
    for (size_t i = 0; i < positions_mm.size(); i++) {
        float x = padding + ((positions_mm[i] - minX) / xRange) * plotWidth;
        float y = padding + ((temperatures[i] - minY) / yRange) * plotHeight;
        
        float tempRatio = (temperatures[i] - minY) / yRange;
        
        if (tempRatio < 0.33f) {
            glColor3f(0.0f, 0.0f, 1.0f);
        } else if (tempRatio < 0.66f) {
            glColor3f(1.0f, 1.0f, 0.0f);
        } else {
            glColor3f(1.0f, 0.0f, 0.0f);
        }
        
        glVertex2f(x, y);
    }
    
    glEnd();
}

void TemperatureAnimator::renderFrame() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Sätt upp viewport och ortogonal projektion
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    renderGrid();
    renderAxes();
    
    if (current_frame < all_temperatures.size()) {
        renderTemperatureCurve(all_temperatures[current_frame]);
    }
    
    glfwSwapBuffers(window);
}

void TemperatureAnimator::animate(const std::string& title) {
    if (all_temperatures.empty()) {
        std::cerr << "No animation data to display" << std::endl;
        return;
    }
    
    initGLFW();
    glfwSetWindowTitle(window, title.c_str());
    
    std::cout << "\n=== Animation Controls ===\n";
    std::cout << "SPACE: Play/Pause\n";
    std::cout << "RIGHT: Next frame\n";
    std::cout << "LEFT: Previous frame\n";
    std::cout << "UP: Increase speed (hold for faster)\n";
    std::cout << "DOWN: Decrease speed\n";
    std::cout << "F: Fast forward to end\n";
    std::cout << "ESC: Exit animation and show results\n";
    std::cout << "=========================\n\n";
    
    auto last_frame_time = std::chrono::steady_clock::now();
    
    while (!glfwWindowShouldClose(window)) {
        // Hantera input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            animation_complete = true;
            glfwSetWindowShouldClose(window, true);
            break;
        }
        
        static bool space_was_up = true;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && space_was_up) {
            is_playing = !is_playing;
            space_was_up = false;
        } else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            space_was_up = true;
        }
        
        // HÖGER nästa frame (håll inne för snabbare)
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            current_frame = std::min(current_frame + 1, (int)all_temperatures.size() - 1);
        }
        
        // VÄNSTER föregående frame
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            current_frame = std::max(current_frame - 1, 0);
        }
        
        // UP öka hastighet dramatiskt
        static bool up_was_up = true;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && up_was_up) {
            animation_speed = std::min(animation_speed * 5.0f, 1000.0f); // Upp till 1000x!
            up_was_up = false;
        } else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE) {
            up_was_up = true;
        }
        
        // DOWN minska hastighet
        static bool down_was_up = true;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && down_was_up) {
            animation_speed = std::max(animation_speed / 2.0f, 0.1f);
            down_was_up = false;
        } else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE) {
            down_was_up = true;
        }
        
        // F Fast forward direkt till slutet
        static bool f_was_up = true;
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && f_was_up) {
            current_frame = all_temperatures.size() - 1;
            is_playing = false;
            f_was_up = false;
            std::cout << "\nFast forward to end.\n";
        } else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
            f_was_up = true;
        }
        
        // Uppdatera animation MED HÖG HASTIGHET
        auto current_time = std::chrono::steady_clock::now();
        double delta_time = std::chrono::duration<double>(current_time - last_frame_time).count();
        last_frame_time = current_time;
        
        if (is_playing && !all_temperatures.empty()) {
            // Direkt frame hopping baserat på hastighet - ANVÄND DOUBLE
            double frames_to_advance = delta_time * animation_speed * 1000.0; // 1000x multiplier
            
            if (frames_to_advance >= 1.0) {
                int frames_int = static_cast<int>(frames_to_advance);
                current_frame = std::min(current_frame + frames_int, (int)all_temperatures.size() - 1);
                
                // Om vi når slutet, stoppa
                if (current_frame == all_temperatures.size() - 1) {
                    is_playing = false;
                    std::cout << "\nAnimationen har nått slutförandet.\n";
                }
            }
        }
        
        // Visa info i terminalen
        if (current_frame < time_steps.size()) {
            std::cout << "\rFrame: " << current_frame + 1 << "/" << all_temperatures.size()
                      << " | Time: " << std::fixed << std::setprecision(1) << time_steps[current_frame] << " s"
                      << " | Speed: " << animation_speed << "x"
                      << " | Playing: " << (is_playing ? "Yes" : "No") 
                      << "          " << std::flush;  // Extra spaces för att rensa tidigare text
        }
        
        // Rendera
        renderFrame();
        
        glfwPollEvents();
        // Mycket kort delay för maximal hastighet
        std::this_thread::sleep_for(std::chrono::microseconds(100)); // 0.1ms
    }
    
    animation_complete = true;
    std::cout << "\n\nAnimation avslutad.\n";
}

bool TemperatureAnimator::isComplete() const {
    return animation_complete;
}

std::vector<double> TemperatureAnimator::getFinalTemperatures() const {
    if (all_temperatures.empty()) return {};
    return all_temperatures.back();
}

std::vector<double> TemperatureAnimator::getPositions() const {
    return positions_mm;
}

double TemperatureAnimator::getFinalTime() const {
    if (time_steps.empty()) return 0.0;
    return time_steps.back();
}

void TemperatureAnimator::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}