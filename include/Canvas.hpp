#pragma once

#include <string>
#include <Eigen/Dense>

#include <cairo/cairo.h>
#include <cairo/cairo-ft.h>

class Canvas
{
private:
    cairo_surface_t *surface;
    cairo_t *context;

    Eigen::Vector4f stroke = Eigen::Vector4f(0, 0, 0, 1);
    Eigen::Vector4f fill = Eigen::Vector4f(0, 0, 0, 1);
    cairo_font_face_t *font_face;
    
public:
    Canvas(int width, int height, const Eigen::Vector4f &color = Eigen::Vector4f(0, 0, 0, 0));
    Canvas(int width, int height, int channels, void *data = nullptr);
    Canvas(const std::string &uri);
    Canvas(const Canvas &image) = delete;
    Canvas(Canvas &&image);
    ~Canvas();
    Eigen::Vector2i get_size() const;
    void fill_color(uint32_t color);
    void stroke_color(uint32_t color);
    void font(const std::string &font, int size = 12);
    Eigen::Vector2i measure_text(const std::string &text);
    void line(int x0, int y0, int x1, int y1);
    void circle(int x, int y, int r);
    void rect(int x, int y, int w, int h);
    void triangle(int x0, int y0, int x1, int y1, int x2, int y2);
    void text(int x, int y, const std::string &text);
    void image(int x, int y, const std::string &path);
    void clear();
    
    /**
     * save as png
    */
    void save(const std::string &path);

    /**
     * dump as raw data.
     * if channels == 3, then the data is RGB (compatible with OpenGL GL_RGB)
     * if channels == 4, then the data is RGBA (compatible with OpenGL GL_RGBA)
     * if channels == 1, then the data is grayscale (compatible with OpenGL GL_RED)
     * if channels == 0, then the data is whatever cairo_image_surface_get_data() returns
    */
    void dump(int channels, std::function<void(void *chunk, int offset, int len, int total)> callback);
};