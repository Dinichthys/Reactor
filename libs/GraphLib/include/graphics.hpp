#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <stdlib.h>

#include "vector.hpp"

namespace graphics {

    class Color : public Coordinates {
        uint8_t brightness_;

        public:
            explicit Color(float red, float green, float blue, uint8_t brightness = 255)
                :Coordinates(3, red, green, blue) {
                brightness_ = brightness;
            };

            uint8_t GetRedPart() const {return static_cast<uint8_t>(GetCoordinate(0));};
            uint8_t GetGreenPart() const {return static_cast<uint8_t>(GetCoordinate(1));};
            uint8_t GetBluePart() const {return static_cast<uint8_t>(GetCoordinate(2));};

            uint8_t GetBrightness() const {return brightness_;};

            void SetBrightness(uint8_t brightness) {brightness_ = brightness;};
    };

    const Color kColorWhite(255, 255, 255);
    const Color kColorRed(255, 0, 0);
    const Color kColorGreen(0, 255, 0);
    const Color kColorBlue(0, 0, 255);
    const Color kColorBrown(150, 75, 0);
    const Color kColorCyan(0, 255, 255);
    const Color kColorYellow(255, 255, 0);
    const Color kColorBlack(0, 0, 0);

    class Text {
        private:
            void* font_;
            void* text_;

        public:
            explicit Text(const char* text, const char* font_file_name, unsigned char height);

            ~Text();

            void SetPosition(const Coordinates& lt_corner) const;

            void* GetText() const {return text_;};
    };

    class VertexArray {
        private:
            void* vertex_array_;

        public:
            explicit VertexArray(size_t size);

            ~VertexArray();

            void SetPixelPosition(size_t index, const Coordinates& pos) const;
            void SetPixelColor(size_t index, const Color& color) const;

            void* GetVertexArray() const {return vertex_array_;};
    };

    class RectangleShape {
        private:
            void* rectangle_;

        public:
            explicit RectangleShape(float width, float height);

            ~RectangleShape();

            void* GetRectangle() const {return rectangle_;};

            void SetSize(float width, float height) const;

            void SetPosition(const Coordinates& lt_corner) const;

            void SetRotation(float angle) const;
            void Rotate(float angle) const;

            void SetOutlineColor(const graphics::Color color) const;
            void SetFillColor(const graphics::Color color) const;
    };

    enum EventType {
        kNoneEvent = -1,
        kClosed = 0,
        kMouseButtonPressed,
        kMouseButtonReleased,
        kMouseMoved,
        kMaxEventType,
    };

    class Event {
        private:
            enum EventType type_;

        public:
            explicit Event(enum EventType type = kNoneEvent) {
                type_ = type;
            };

            void SetType(enum EventType type) {type_ = type;};
            enum EventType GetType() const {return type_;};
    };

    class RenderWindow {
        private:
            void* window_;

            float width_;
            float height_;

        public:
            explicit RenderWindow(size_t width, size_t height, const char* window_name);

            ~RenderWindow();

            float GetStartWidth() const {return width_;};
            float GetStartHeight() const {return height_;};

            float GetWidth() const;
            float GetHeight() const;

            bool PollEvent(graphics::Event& event) const;

            Coordinates GetMousePos() const;

            void Draw(const graphics::RectangleShape& rect) const;
            void Draw(const graphics::VertexArray& arr) const;
            void Draw(const graphics::Text& text) const;

            void Display() const;

            bool IsOpen() const;

            void Close() const;
    };

}

#endif // GRAPHICS_HPP
