#include "Canvas.hpp"
#include "FileSystem.hpp"
#include "Color.hpp"

#include "stb_image.h"
#include <cairo/cairo.h>
#include <cairo/cairo-ft.h>

static FT_Library ft_library = nullptr;

static void ensure_ft_library()
{
    if (ft_library == nullptr)
    {
        FT_Init_FreeType(&ft_library);
    }
}

Canvas::Canvas(int width, int height, const Eigen::Vector4f &color)
{
    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    context = cairo_create((cairo_surface_t *)surface);
    cairo_set_source_rgba((cairo_t *)context, color.x(), color.y(), color.z(), color.w());
    cairo_fill((cairo_t *)context);
}

Canvas::Canvas(const std::string &uri)
{
    Blob blob = FileSystem::get_entry(uri)->read();
    int width, height, channels_in_file;
    uint8_t *stbi_data = stbi_load_from_memory(blob.as<const stbi_uc *>(), blob.get_size(), &width, &height, &channels_in_file, 4);
    if (stbi_data == nullptr)
    {
        throw std::runtime_error("Failed to load image '" + uri + "'");
    }

// convert stbi rgba to cairo argb
#pragma omp parallel for
    for (int i = 0; i < width * height; i++)
    {
        uint8_t *pixel = stbi_data + i * 4;
        uint8_t tmp = pixel[0];
        pixel[0] = pixel[2];
        pixel[2] = tmp;
    }

    surface = cairo_image_surface_create_for_data(stbi_data, CAIRO_FORMAT_ARGB32, width, height, width * 4);
    context = cairo_create((cairo_surface_t *)surface);

    // don't free stbi_data, cairo_surface_t owns it now
}

Canvas::Canvas(Canvas &&image)
{
    surface = image.surface;
    context = image.context;
    font_face = image.font_face;
    image.surface = nullptr;
    image.context = nullptr;
    image.font_face = nullptr;
}

Canvas::~Canvas()
{

    if (font_face != nullptr)
    {
        cairo_font_face_destroy(font_face);
    }
    if (context != nullptr)
    {
        cairo_destroy((cairo_t *)context);
    }
    if (surface != nullptr)
    {
        cairo_surface_destroy((cairo_surface_t *)surface);
    }
}

Eigen::Vector2i Canvas::get_size() const
{
    return Eigen::Vector2i(
        cairo_image_surface_get_width(surface),
        cairo_image_surface_get_height(surface));
}

void Canvas::fill_color(uint32_t color)
{
    fill = Color::rgba32_to_floats(color);
}

void Canvas::stroke_color(uint32_t color)
{
    stroke = Color::rgba32_to_floats(color);
}

void Canvas::font(const std::string &font, int size)
{
    ensure_ft_library();
    if (font_face != nullptr)
    {
        cairo_font_face_destroy(font_face);
    }
    FT_Face ft_face;
    if (FT_New_Face(ft_library, font.c_str(), 0, &ft_face))
    {
        throw std::runtime_error("Failed to load font '" + font + "'");
    }
    font_face = cairo_ft_font_face_create_for_ft_face(ft_face, 0);
    cairo_set_font_face(context, font_face);
    cairo_set_font_size(context, size);
}

void Canvas::line(int x0, int y0, int x1, int y1)
{
    cairo_set_source_rgba(context, stroke.x(), stroke.y(), stroke.z(), stroke.w());
    cairo_move_to(context, x0, y0);
    cairo_line_to(context, x1, y1);
    cairo_stroke(context);
}

void Canvas::circle(int x, int y, int r)
{
    cairo_set_source_rgba(context, fill.x(), fill.y(), fill.z(), fill.w());
    cairo_arc(context, x, y, r, 0, 2 * M_PI);
    cairo_fill(context);
}

void Canvas::rect(int x, int y, int w, int h)
{
    cairo_set_source_rgba(context, fill.x(), fill.y(), fill.z(), fill.w());
    cairo_rectangle(context, x, y, w, h);
    cairo_fill(context);
}

void Canvas::triangle(int x0, int y0, int x1, int y1, int x2, int y2)
{
    cairo_set_source_rgba(context, fill.x(), fill.y(), fill.z(), fill.w());
    cairo_move_to(context, x0, y0);
    cairo_line_to(context, x1, y1);
    cairo_line_to(context, x2, y2);
    cairo_close_path(context);
    cairo_fill(context);
}

void Canvas::text(int x, int y, const std::string &text)
{
    ensure_ft_library();
    if (font_face == nullptr)
    {
        font("Arial", 12);
    }
    cairo_set_source_rgba(context, fill.x(), fill.y(), fill.z(), fill.w());
    cairo_move_to(context, x, y);
    cairo_show_text(context, text.c_str());
}

void Canvas::image(int x, int y, const std::string &path)
{
    Canvas image(path);
    cairo_set_source_surface(context, (cairo_surface_t *)image.surface, x, y);
    cairo_paint(context);
}

void Canvas::clear()
{
    cairo_set_source_rgba(context, fill.x(), fill.y(), fill.z(), fill.w());
    cairo_paint(context);
}

void Canvas::dump(int channels, std::function<void(void *chunk, int offset, int len, int total)> callback)
{
    cairo_surface_flush((cairo_surface_t *)surface);
    int stride = cairo_image_surface_get_stride((cairo_surface_t *)surface);
    int height = cairo_image_surface_get_height((cairo_surface_t *)surface);
    int width = cairo_image_surface_get_width((cairo_surface_t *)surface);
    uint8_t *data = cairo_image_surface_get_data((cairo_surface_t *)surface);

    // the total number of bytes that we need to dump
    int len = width * height * channels;

    if (channels == 1)
    {
        uint8_t chunk[4096];
// for each argb pixel, compute the grayscale value, and dump it
#pragma omp parallel for
        for (int i = 0; i < len; i++)
        {
            int a = data[i * 4 + 3];
            int r = data[i * 4 + 2];
            int g = data[i * 4 + 1];
            int b = data[i * 4 + 0];
            chunk[i % 4096] = r * 0.299 + g * 0.587 + b * 0.114;
            if (i % 4096 == 4095)
            {
                callback(chunk, i - 4095, 4096, len);
            }
        }
        if (len % 4096 != 0)
        {
            callback(chunk, len - (len % 4096), len % 4096, len);
        }
    }
    else if (channels == 3)
    {
        uint8_t chunk[4096 * 3];
#pragma omp parallel for
        for (int i = 0; i < len; i++)
        {
            int a = data[i * 4 + 3];
            int r = data[i * 4 + 2];
            int g = data[i * 4 + 1];
            int b = data[i * 4 + 0];
            chunk[(i % 4096) * 3 + 0] = r;
            chunk[(i % 4096) * 3 + 1] = g;
            chunk[(i % 4096) * 3 + 2] = b;
            if (i % 4096 == 4095)
            {
                callback(chunk, i - 4095, 4096 * 3, len);
            }
        }
        if (len % 4096 != 0)
        {
            callback(chunk, len - (len % 4096), len % 4096, len);
        }
    }
    else if (channels == 4)
    {
        uint8_t chunk[4096 * 4];
#pragma omp parallel for
        for (int i = 0; i < len; i++)
        {
            int a = data[i * 4 + 3];
            int r = data[i * 4 + 2];
            int g = data[i * 4 + 1];
            int b = data[i * 4 + 0];
            chunk[(i % 4096) * 4 + 0] = r;
            chunk[(i % 4096) * 4 + 1] = g;
            chunk[(i % 4096) * 4 + 2] = b;
            chunk[(i % 4096) * 4 + 3] = a;
            if (i % 4096 == 4095)
            {
                callback(chunk, i - 4095, 4096 * 4, len);
            }
        }
        if (len % 4096 != 0)
        {
            callback(chunk, len - (len % 4096), len % 4096, len);
        }
    }
    else
    {
        throw std::runtime_error("Unsupported number of channels");
    }
}

Eigen::Vector2i Canvas::measure_text(const std::string &text)
{
    ensure_ft_library();
    if (font_face == nullptr)
    {
        font("Arial", 12);
    }
    cairo_text_extents_t extents;
    cairo_text_extents(context, text.c_str(), &extents);
    return Eigen::Vector2i(extents.width, extents.height);
}

void Canvas::save(const std::string &path)
{
    if (FileSystem::is_filesystem_uri(path))
    {

        cairo_write_func_t write_func = [](void *closure, const unsigned char *data, unsigned int length) -> cairo_status_t
        {
            std::vector<uint8_t> *buffer = (std::vector<uint8_t> *)closure;
            buffer->insert(buffer->end(), data, data + length);
            return CAIRO_STATUS_SUCCESS;
        };

        std::vector<uint8_t> buffer;
        cairo_surface_write_to_png_stream((cairo_surface_t *)surface, write_func, &buffer);
        FileSystem::get_entry(path)->write(buffer.data(), buffer.size());
    }
    else
    {
        cairo_surface_write_to_png((cairo_surface_t *)surface, path.c_str());
    }
}