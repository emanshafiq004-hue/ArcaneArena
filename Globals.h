#pragma once

//  Contains:
//    • Global constants & colour palette
//    • Vec2  (Operator Overloading)
//    • randRange<T>  (Template Function)
//    • GameStats  (Static Members)
//    • ObjectPool<T>  (Template Class)
//    • IDrawable / IUpdatable  (Abstract Classes)
//    • GameObject  (Multiple Inheritance)
//    • SFML view helpers

#define _CRT_SECURE_NO_WARNINGS

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <random>
#include <cmath>
#include <ctime>
#include <type_traits>


//_______________GLOBAL CONSTANTS________________
inline constexpr float BASE_W = 800.f;
inline constexpr float BASE_H = 600.f;
inline constexpr float GRAVITY = 900.f;
inline constexpr float THRUST_FORCE = -400.f;
inline constexpr float DRONE_X = 140.f;
inline constexpr float BARRIER_W = 22.f;
inline constexpr int   TOP5 = 5;

inline const std::string SCORE_FILE = "highscores.txt";
inline const std::string SETTING_FILE = "settings.txt";

//__________________Colour Palette__________________
inline const sf::Color COL_BG_TOP(4, 8, 40, 255);
inline const sf::Color COL_BG_BOT(6, 22, 80, 255);
inline const sf::Color COL_BARRIER(0, 210, 255, 255);
inline const sf::Color COL_BGLOW(0, 210, 255, 60);
inline const sf::Color COL_CRYSTAL(255, 220, 0, 255);
inline const sf::Color COL_SHIELD(80, 255, 180, 255);
inline const sf::Color COL_MULTI(255, 100, 255, 255);
inline const sf::Color COL_TEXT(220, 240, 255, 255);
inline const sf::Color COL_HL(0, 230, 255, 255);
inline const sf::Color COL_DEAD(255, 60, 60, 255);
inline const sf::Color COL_GRID(30, 60, 130, 50);
inline const sf::Color COL_BLDG(10, 20, 70, 255);
inline const sf::Color COL_WINLIT(255, 230, 100, 180);
inline const sf::Color COL_MISSILE(255, 80, 20, 255);

//_____________TEMPLATE FUNCTION: randRange<T>_________________
//________Works for both int and float via if constexpr________
extern std::mt19937 gRng;

template<typename T>
T randRange(T lo, T hi) {
    if constexpr (std::is_integral_v<T>)
        return std::uniform_int_distribution<T>(lo, hi)(gRng);
    else
        return std::uniform_real_distribution<T>(lo, hi)(gRng);
}

//__________________SFML HELPERS____________________
inline sf::FloatRect makeRect(float x, float y, float w, float h) {
    return sf::FloatRect(sf::Vector2f(x, y), sf::Vector2f(w, h));
}

sf::View getLetterboxView(float winW, float winH);

//________________OPERATOR OVERLOADING: Vec2________________
//________Custom 2D vector with full operator support_______
struct Vec2 {
    float x = 0.f, y = 0.f;

    Vec2() : x(0.f), y(0.f) {}                        //Default constructor
    Vec2(float x_, float y_) : x(x_), y(y_) {}        //Parameterized constructor
    Vec2(const sf::Vector2f& v) : x(v.x), y(v.y) {}   //Conversion constructor

    operator sf::Vector2f() const { return sf::Vector2f(x, y); }

    Vec2  operator+ (const Vec2& o) const { return Vec2(x + o.x, y + o.y); }
    Vec2  operator- (const Vec2& o) const { return Vec2(x - o.x, y - o.y); }
    Vec2  operator* (float s)       const { return Vec2(x * s, y * s); }
    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }
    bool  operator==(const Vec2& o) const { return x == o.x && y == o.y; }
    bool  operator!=(const Vec2& o) const { return !(*this == o); }

    float length()     const { return std::sqrt(x * x + y * y); }
    Vec2  normalized() const { float l = length(); return l > 0 ? Vec2(x / l, y / l) : Vec2(); }
};

//___________STATIC MEMBERS & METHODS: GameStats___________
//___Tracks session-wide counters via static class members___
class GameStats {
public:
    static int totalBarriersPassed;
    static int totalCrystalsCollected;
    static int totalDeaths;

    static void reset();
    static std::string summary();
};

//______________TEMPLATE CLASS: ObjectPool<T>________________
//__Generic reusable pool for any type with an 'active' field__
template<typename T>
class ObjectPool {
    std::vector<T> pool_;
public:
    explicit ObjectPool(int n) : pool_(n) {}   //Parameterized constructor

    T* acquire() {
        for (auto& obj : pool_)
            if (!obj.active) return &obj;
        return nullptr;
    }
    std::vector<T>& all() { return pool_; }
    const std::vector<T>& all() const { return pool_; }
    int capacity() const { return static_cast<int>(pool_.size()); }
};

//________ABSTRACT BASE CLASSES (pure virtual interfaces)____
class IDrawable {
public:
    virtual void draw(sf::RenderTarget& rt) = 0;
    virtual ~IDrawable() = default;
};

class IUpdatable {
public:
    virtual void update(float dt) = 0;
    virtual ~IUpdatable() = default;
};

//__________MULTIPLE INHERITANCE + ABSTRACT CLASS_____________
//  GameObject inherits both IDrawable and IUpdatable
//  and is itself abstract (draw/update still pure virtual)
class GameObject : public IDrawable, public IUpdatable {
protected:
    Vec2 pos_;
    bool active_;

public:
    //________Parameterized constructor + initializer list______
    explicit GameObject(Vec2 pos = Vec2(), bool active = true)
        : pos_(pos), active_(active) {
    }

    //________Virtual destructor__________
    virtual ~GameObject() = default;

    //_________Polymorphism: subclasses override these_________
    virtual void draw(sf::RenderTarget& rt) override = 0;
    virtual void update(float dt)           override = 0;

    virtual sf::FloatRect getBounds() const {
        return makeRect(pos_.x - 8.f, pos_.y - 8.f, 16.f, 16.f);
    }

    //_________Encapsulation: private data accessed via getters/setters________
    Vec2  getPos()    const { return pos_; }
    bool  isActive()  const { return active_; }
    void  setActive(bool a) { active_ = a; }
    void  setPos(Vec2 p) { pos_ = p; }
};

//Globals.h provides :
//   Constants  -> used by every file            
//   Colours    -> used by every drawing file    
//   Vec2       -> used for every position / vel   
//   randRange  -> used for all randomness       
//   makeRect   -> used for all collisions       
//   ObjectPool -> used by ParticleSystem        
//   GameStats  -> updated by Drone + Score      
//   IDrawable  -> interface for all objects     
//   IUpdatable -> interface for all objects     
//   GameObject -> base for Drone, Barrier, etc    
//
