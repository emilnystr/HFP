#ifndef PLOTTER_H
#define PLOTTER_H

#include <vector>
#include <string>
#include <GLFW/glfw3.h>

class TemperatureAnimator {
private:
    GLFWwindow* window; 
    int width;
    int height;
    
    std::vector<std::vector<double>> all_temperatures; 
    std::vector<double> positions_mm;                 
    std::vector<double> time_steps;                    
    
    // Animation state
    int current_frame;
    bool is_playing;
    float animation_speed;    
    float minX, maxX, minY, maxY;
    float padding;
    
    void initGLFW();
    void calculateBounds();
    void renderFrame();
    void renderGrid();
    void renderAxes();
    void renderTemperatureCurve(const std::vector<double>& temperatures);
    
public:
    TemperatureAnimator(int w = 1000, int h = 700);
    ~TemperatureAnimator();
    
    void setData(const std::vector<double>& positions, 
                 const std::vector<std::vector<double>>& temperatures,
                 const std::vector<double>& times);
    
    void animate(const std::string& title = "HFP Engine - Temperature Animation");
    
    
    std::vector<double> getFinalTemperatures() const;
    std::vector<double> getPositions() const;
    double getFinalTime() const;
    
    static void framebuffer_size_callback(struct GLFWwindow* window, int width, int height);
};

#endif 