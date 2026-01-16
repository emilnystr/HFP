#ifndef PLOTTER_H
#define PLOTTER_H

#include <vector>
#include <string>

class TemperatureAnimator {
private:
    struct GLFWwindow* window;
    int width, height;
    
    // Animation data
    std::vector<std::vector<double>> all_temperatures; // Temperaturer för varje frame
    std::vector<double> positions_mm;                  // Positioner (samma för alla frames)
    std::vector<double> time_steps;                    // Tider för varje frame
    
    // Animation state
    int current_frame;
    bool is_playing;
    float animation_speed;
    bool animation_complete;  // NY: håller koll på om animationen är klar
    
    // Grafikparametrar
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
    
    // Funktioner för att hämta slutresultat
    bool isComplete() const;
    std::vector<double> getFinalTemperatures() const;
    std::vector<double> getPositions() const;
    double getFinalTime() const;
    
    static void framebuffer_size_callback(struct GLFWwindow* window, int width, int height);
};

#endif // PLOTTER_H