#ifndef MY_WINDOW_HPP
#define MY_WINDOW_HPP

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "vector.hpp"
#include "logging.h"

class Widget {
    private:
        Coordinates lt_corner_;
        float width_;
        float height_;
        Widget* parent_;

        bool controlled;

    public:
        explicit Widget(const Coordinates& lt_corner, const float width, float height, Widget* parent = NULL)
            :lt_corner_(lt_corner) {
            width_ = width;
            height_ = height;
            parent_ = parent;
            controlled = false;
        };

        virtual ~Widget() {};

        virtual Coordinates GetLTCornerLoc() const {
            return Coordinates(lt_corner_);
        };
        virtual Coordinates GetLTCornerAbs() const {
            return (parent_ == NULL) ? lt_corner_ : lt_corner_ + parent_->GetLTCornerAbs();
        };
        virtual Coordinates GetRBCornerLoc() const {return GetLTCornerLoc() + Coordinates(2, width_, height_);};
        virtual Coordinates GetRBCornerAbs() const {return GetLTCornerAbs() + Coordinates(2, width_, height_);};
        virtual float GetWidth() const {return width_;};
        virtual float GetHeight() const {return height_;};

        virtual bool GetControllingInfo() const {return controlled;};

        virtual Widget* GetParent() const {return parent_;};

        virtual void SetLTCorner(const Coordinates& coors) {lt_corner_ = coors;};
        void SetParent(Widget* parent) {parent_ = parent;};

        virtual void Draw(sf::RenderWindow* window) = 0;
        virtual void Move(float shift_x, float shift_y) {
            lt_corner_.SetCoordinate(0, lt_corner_[0] + shift_x);
            lt_corner_.SetCoordinate(1, lt_corner_[1] + shift_y);
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos) {
            return false;
        };

        virtual bool OnIdle() {
            return false;
        };
};

class WidgetContainer : public Widget {
    private:
        std::vector<Widget*> children_;

    public:
        explicit WidgetContainer(const Coordinates& lt_corner, const float width, float height,
                                 const std::vector<Widget*>* children = NULL, Widget* parent = NULL)
            :Widget(lt_corner, width, height, parent) {
            if (children == NULL) {
                return;
            }

            size_t children_num = children->size();
            for (size_t i = 0; i < children_num; i++) {
                children_.push_back((*children)[i]);
            }
        };

        ~WidgetContainer() {
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                delete children_.back();
                children_.pop_back();
            }
        };

        std::vector<Widget*>& GetChildren() {return children_;};
        size_t GetChildrenNum() const {return children_.size();};
        void AddChild(Widget* child) {children_.push_back(child);};
        Widget* GetChild(size_t index) const {return children_[index];};
        void SetChild(size_t index, Widget* child) {
            children_[index] = child;
            child->SetParent(this);
        };

        void ResetChildren() {children_ = {};};

        void SetParentToChildren() {
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                children_[i]->SetParent(this);
            }
        }

        virtual void Draw(sf::RenderWindow* window) override {
            ASSERT(window != NULL, "");

            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                children_[i]->Draw(window);
            }
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos) {
            Coordinates lt_corner(Widget::GetLTCornerLoc());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                if (children_[i]->OnMousePress(mouse_pos - lt_corner)) {
                    return true;
                }
            }

            return false;
        };

        virtual bool OnIdle() {
            Coordinates lt_corner(Widget::GetLTCornerLoc());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                children_[i]->OnIdle();
            }

            return false;
        };
};

#endif // MY_WINDOW_HPP
