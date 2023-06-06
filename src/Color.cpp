#include "Color.hpp"
#include <regex>
#include "strutil.hpp"

Eigen::Vector4f Color::rgba32_to_floats(uint32_t color) {
    int r = (color >> 24) & 0xFF;
    int g = (color >> 16) & 0xFF;
    int b = (color >> 8) & 0xFF;
    int a = (color >> 0) & 0xFF;
    return Eigen::Vector4f(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

uint32_t Color::floats_to_rgba32(const Eigen::Vector4f &color) {
    return floats_to_rgba32(color.x(), color.y(), color.z(), color.w());
}

uint32_t Color::floats_to_rgba32(float r, float g, float b, float a) {
    int _r = (int)(r * 255.0f);
    int _g = (int)(g * 255.0f);
    int _b = (int)(b * 255.0f);
    int _a = (int)(a * 255.0f);
    return (_r << 24) | (_g << 16) | (_b << 8) | (_a << 0);
}

Eigen::Vector4f Color::hsl_to_rgb(const Eigen::Vector3f &hsl) {
    float h = hsl.x();
    float s = hsl.y();
    float l = hsl.z();
    float c = (1.0f - std::abs(2.0f * l - 1.0f)) * s;
    float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = l - c / 2.0f;
    float r, g, b;
    if (h < 60.0f) {
        r = c;
        g = x;
        b = 0.0f;
    } else if (h < 120.0f) {
        r = x;
        g = c;
        b = 0.0f;
    } else if (h < 180.0f) {
        r = 0.0f;
        g = c;
        b = x;
    } else if (h < 240.0f) {
        r = 0.0f;
        g = x;
        b = c;
    } else if (h < 300.0f) {
        r = x;
        g = 0.0f;
        b = c;
    } else {
        r = c;
        g = 0.0f;
        b = x;
    }
    return Eigen::Vector4f(r + m, g + m, b + m, 1.0f);
}

Eigen::Vector3f Color::rgb_to_hsl(const Eigen::Vector4f &rgb) {
    float r = rgb.x();
    float g = rgb.y();
    float b = rgb.z();
    float max = std::max(r, std::max(g, b));
    float min = std::min(r, std::min(g, b));
    float h, s, l = (max + min) / 2.0f;
    if (max == min) {
        h = s = 0.0f;
    } else {
        float d = max - min;
        s = l > 0.5f ? d / (2.0f - max - min) : d / (max + min);
        if (max == r) {
            h = (g - b) / d + (g < b ? 6.0f : 0.0f);
        } else if (max == g) {
            h = (b - r) / d + 2.0f;
        } else {
            h = (r - g) / d + 4.0f;
        }
        h /= 6.0f;
    }
    return Eigen::Vector3f(h * 360.0f, s, l);
}

static const std::map<std::string, Eigen::Vector3i> color_map{
{"snow", {255, 250, 250}},
{"ghostwhite", {248, 248, 255}},
{"whitesmoke", {245, 245, 245}},
{"gainsboro", {220, 220, 220}},
{"floralwhite", {255, 250, 240}},
{"oldlace", {253, 245, 230}},
{"linen", {250, 240, 230}},
{"antiquewhite", {250, 235, 215}},
{"papayawhip", {255, 239, 213}},
{"blanchedalmond", {255, 235, 205}},
{"bisque", {255, 228, 196}},
{"peachpuff", {255, 218, 185}},
{"navajowhite", {255, 222, 173}},
{"moccasin", {255, 228, 181}},
{"cornsilk", {255, 248, 220}},
{"ivory", {255, 255, 240}},
{"lemonchiffon", {255, 250, 205}},
{"seashell", {255, 245, 238}},
{"honeydew", {240, 255, 240}},
{"mintcream", {245, 255, 250}},
{"azure", {240, 255, 255}},
{"aliceblue", {240, 248, 255}},
{"lavender", {230, 230, 250}},
{"lavenderblush", {255, 240, 245}},
{"mistyrose", {255, 228, 225}},
{"white", {255, 255, 255}},
{"black", {0, 0, 0}},
{"darkslategray", {47, 79, 79}},
{"darkslategrey", {47, 79, 79}},
{"dimgray", {105, 105, 105}},
{"dimgrey", {105, 105, 105}},
{"slategray", {112, 128, 144}},
{"slategrey", {112, 128, 144}},
{"lightslategray", {119, 136, 153}},
{"lightslategrey", {119, 136, 153}},
{"gray", {190, 190, 190}},
{"grey", {190, 190, 190}},
{"lightgrey", {211, 211, 211}},
{"lightgray", {211, 211, 211}},
{"midnightblue", {25, 25, 112}},
{"navy", {0, 0, 128}},
{"navyblue", {0, 0, 128}},
{"cornflowerblue", {100, 149, 237}},
{"darkslateblue", {72, 61, 139}},
{"slateblue", {106, 90, 205}},
{"mediumslateblue", {123, 104, 238}},
{"lightslateblue", {132, 112, 255}},
{"mediumblue", {0, 0, 205}},
{"royalblue", {65, 105, 225}},
{"blue", {0, 0, 255}},
{"dodgerblue", {30, 144, 255}},
{"deepskyblue", {0, 191, 255}},
{"skyblue", {135, 206, 235}},
{"lightskyblue", {135, 206, 250}},
{"steelblue", {70, 130, 180}},
{"lightsteelblue", {176, 196, 222}},
{"lightblue", {173, 216, 230}},
{"powderblue", {176, 224, 230}},
{"paleturquoise", {175, 238, 238}},
{"darkturquoise", {0, 206, 209}},
{"mediumturquoise", {72, 209, 204}},
{"turquoise", {64, 224, 208}},
{"cyan", {0, 255, 255}},
{"lightcyan", {224, 255, 255}},
{"cadetblue", {95, 158, 160}},
{"mediumaquamarine", {102, 205, 170}},
{"aquamarine", {127, 255, 212}},
{"darkgreen", {0, 100, 0}},
{"darkolivegreen", {85, 107, 47}},
{"darkseagreen", {143, 188, 143}},
{"seagreen", {46, 139, 87}},
{"mediumseagreen", {60, 179, 113}},
{"lightseagreen", {32, 178, 170}},
{"palegreen", {152, 251, 152}},
{"springgreen", {0, 255, 127}},
{"lawngreen", {124, 252, 0}},
{"green", {0, 255, 0}},
{"chartreuse", {127, 255, 0}},
{"mediumspringgreen", {0, 250, 154}},
{"greenyellow", {173, 255, 47}},
{"limegreen", {50, 205, 50}},
{"yellowgreen", {154, 205, 50}},
{"forestgreen", {34, 139, 34}},
{"olivedrab", {107, 142, 35}},
{"darkkhaki", {189, 183, 107}},
{"khaki", {240, 230, 140}},
{"palegoldenrod", {238, 232, 170}},
{"lightgoldenrodyellow", {250, 250, 210}},
{"lightyellow", {255, 255, 224}},
{"yellow", {255, 255, 0}},
{"gold", {255, 215, 0}},
{"lightgoldenrod", {238, 221, 130}},
{"goldenrod", {218, 165, 32}},
{"darkgoldenrod", {184, 134, 11}},
{"rosybrown", {188, 143, 143}},
{"indianred", {205, 92, 92}},
{"saddlebrown", {139, 69, 19}},
{"sienna", {160, 82, 45}},
{"peru", {205, 133, 63}},
{"burlywood", {222, 184, 135}},
{"beige", {245, 245, 220}},
{"wheat", {245, 222, 179}},
{"sandybrown", {244, 164, 96}},
{"tan", {210, 180, 140}},
{"chocolate", {210, 105, 30}},
{"firebrick", {178, 34, 34}},
{"brown", {165, 42, 42}},
{"darksalmon", {233, 150, 122}},
{"salmon", {250, 128, 114}},
{"lightsalmon", {255, 160, 122}},
{"orange", {255, 165, 0}},
{"darkorange", {255, 140, 0}},
{"coral", {255, 127, 80}},
{"lightcoral", {240, 128, 128}},
{"tomato", {255, 99, 71}},
{"orangered", {255, 69, 0}},
{"red", {255, 0, 0}},
{"hotpink", {255, 105, 180}},
{"deeppink", {255, 20, 147}},
{"pink", {255, 192, 203}},
{"lightpink", {255, 182, 193}},
{"palevioletred", {219, 112, 147}},
{"maroon", {176, 48, 96}},
{"mediumvioletred", {199, 21, 133}},
{"violetred", {208, 32, 144}},
{"magenta", {255, 0, 255}},
{"violet", {238, 130, 238}},
{"plum", {221, 160, 221}},
{"orchid", {218, 112, 214}},
{"mediumorchid", {186, 85, 211}},
{"darkorchid", {153, 50, 204}},
{"darkviolet", {148, 0, 211}},
{"blueviolet", {138, 43, 226}},
{"purple", {160, 32, 240}},
{"mediumpurple", {147, 112, 219}},
{"thistle", {216, 191, 216}},
{"debianred", {215, 7, 81}},
{"darkgrey", {169, 169, 169}},
{"darkgray", {169, 169, 169}},
{"darkblue", {0, 0, 139}},
{"darkcyan", {0, 139, 139}},
{"darkmagenta", {139, 0, 139}},
{"darkred", {139, 0, 0}},
{"lightgreen", {144, 238, 144}}
};

Eigen::Vector4f Color::for_name(const std::string &name) {

    static std::regex hex_color("#([0-9a-fA-F]{6})([0-9a-fA-F]{2})?");
    static std::regex rgb_color("rgb\\((\\d+),\\s*(\\d+),\\s*(\\d+)\\)");
    static std::regex hsl_color("hsl\\((\\d+)(deg)?,\\s*(\\d+)%,\\s*(\\d+)%\\)");
    std::smatch match;

    auto it = color_map.find(strutil::to_lower(strutil::remove_all(name, " ")));
    if (it != color_map.end()) {
        return Eigen::Vector4f(it->second[0] / 255.0f, it->second[1] / 255.0f, it->second[2] / 255.0f, 1.0f);
    }

    else if (std::regex_match(name, match, hex_color)) {
        std::string hex = match[1];
        std::string alpha = match[2];
        if (alpha.empty()) {
            alpha = "FF";
        }
        int r = std::stoi(hex.substr(0, 2), nullptr, 16);
        int g = std::stoi(hex.substr(2, 2), nullptr, 16);
        int b = std::stoi(hex.substr(4, 2), nullptr, 16);
        int a = std::stoi(alpha, nullptr, 16);
        return Eigen::Vector4f(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    }

    else if(std::regex_match(name, match, rgb_color)) {
        int r = std::stoi(match[1]);
        int g = std::stoi(match[2]);
        int b = std::stoi(match[3]);
        return Eigen::Vector4f(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
    }

    else if(std::regex_match(name, match, hsl_color)) {
        int h = std::stoi(match[1]);
        int s = std::stoi(match[3]);
        int l = std::stoi(match[4]);
        float r, g, b;
        return Color::hsl_to_rgb(Eigen::Vector3f(h, s, l));
    }
    throw std::runtime_error("color not found: " + name);
}