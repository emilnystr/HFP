#include "plotter.h"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>

TemperatureAnimator::TemperatureAnimator(int w, int h) 
    : width(w), height(h), window(nullptr), padding(80.0f),
      current_frame(0), is_playing(true), animation_speed(0.1f),
      minX(0), maxX(0), minY(0), maxY(0) {}

TemperatureAnimator::~TemperatureAnimator() {
    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}

void TemperatureAnimator::initGLFW() {
    if (!glfwInit()) {
        return;
    }
    
    window = glfwCreateWindow(width, height, "HFP Engine", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.0f);
    glPointSize(5.0f);
}

void TemperatureAnimator::calculateBounds() {
    if (positions_mm.empty() || all_temperatures.empty()) return;
    
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
    
    int numXGrid = 10;
    for (int i = 0; i <= numXGrid; i++) {
        float x = padding + (width - 2 * padding) * i / numXGrid;
        glBegin(GL_LINES);
        glVertex2f(x, padding);
        glVertex2f(x, height - padding);
        glEnd();
    }
    
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
    
    glBegin(GL_LINES);
    glVertex2f(padding, padding);
    glVertex2f(width - padding, padding);
    glEnd();
    
    glBegin(GL_LINES);
    glVertex2f(padding, padding);
    glVertex2f(padding, height - padding);
    glEnd();
}

//colors
static void colorFromRatio(float ratio) {
    if (ratio < 0.25f) {
        float t = ratio / 0.25f;
        glColor3f(0.0f, t, 1.0f - t);        
    } else if (ratio < 0.5f) {
        float t = (ratio - 0.25f) / 0.25f;
        glColor3f(t, 1.0f, 0.0f);            
    } else if (ratio < 0.75f) {
        float t = (ratio - 0.5f) / 0.25f;
        glColor3f(1.0f, 1.0f - t, 0.0f);     
    } else {
        float t = (ratio - 0.75f) / 0.25f;
        glColor3f(1.0f, 0.0f, t);            
    }
}

void TemperatureAnimator::renderTemperatureCurve(const std::vector<double>& temperatures) {
    if (positions_mm.empty() || temperatures.empty()) return;
    
    float plotWidth = width - 2 * padding;
    float plotHeight = height - 2 * padding;
    float xRange = maxX - minX;
    float yRange = maxY - minY;
    
    if (plotWidth <= 0 || plotHeight <= 0 || xRange <= 0 || yRange <= 0) return;
    
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    
    for (size_t i = 0; i < positions_mm.size(); i++) {
        float x = padding + ((positions_mm[i] - minX) / xRange) * plotWidth;
        float y = padding + ((temperatures[i] - minY) / yRange) * plotHeight;
        
        float tempRatio = (temperatures[i] - minY) / yRange;
        colorFromRatio(tempRatio);
        glVertex2f(x, y);
    }
    
    glEnd();
    
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    
    for (size_t i = 0; i < positions_mm.size(); i++) {
        float x = padding + ((positions_mm[i] - minX) / xRange) * plotWidth;
        float y = padding + ((temperatures[i] - minY) / yRange) * plotHeight;
        
        float tempRatio = (temperatures[i] - minY) / yRange;
        colorFromRatio(tempRatio);
        glVertex2f(x, y);
    }
    
    glEnd();
}

void TemperatureAnimator::renderFrame() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
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
    if (all_temperatures.empty()) return;
    
    initGLFW();
    glfwSetWindowTitle(window, title.c_str());
    
    auto last_frame_time = std::chrono::steady_clock::now();
    
    while (!glfwWindowShouldClose(window)) {
        auto current_time = std::chrono::steady_clock::now();
        double delta_time = std::chrono::duration<double>(current_time - last_frame_time).count();
        last_frame_time = current_time;
        
        if (is_playing && !all_temperatures.empty()) {
            double frames_to_advance = delta_time * animation_speed * 1000.0;
            
            if (frames_to_advance >= 1.0) {
                int frames_int = static_cast<int>(frames_to_advance);
                int last_frame = static_cast<int>(all_temperatures.size()) - 1;
                current_frame = std::min(current_frame + frames_int, last_frame);
                
                if (current_frame == last_frame) {
                    is_playing = false;
                }
            }
        }
        
        renderFrame();
        
        glfwPollEvents();
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    
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
