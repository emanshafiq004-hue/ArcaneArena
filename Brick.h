#pragma once
#include <SFML/Graphics.hpp>

//  BASE BRICK CLASS, Every brick in the game inherits from this
//  It has a shape, health, points, destroyed flag
class Brick
{
public:
    sf::RectangleShape shape;   // the visual rectangle
    int  health;                // how many hits it can take
    int  points;                // score you get when destroyed
    bool destroyed;             // true = remove from game

    // Constructor: position (x,y), size (w,h), health, points, color
    Brick(float x, float y, float w, float h, int hp, int pts, sf::Color color);

    // Virtual destructor - important when using inheritance
    virtual ~Brick() = default;

    // Called when the ball hits this brick Returns true if the brick is now destroyed
    virtual bool hit();

    // Called every frame - base brick doesn't move Derived classes (MovingBrick) will override this
    virtual void update(float dt);

    // Returns the area of this brick on screen
    sf::FloatRect getBounds() const;
};

//  ARMORED BRICK  (inherits from Brick) 
//  Takes 2 hits before destroyed, color blue, points 20
class ArmoredBrick : public Brick
{
public:
    ArmoredBrick(float x, float y, float w, float h);
    // No need to override hit() - base class handles 2 hits fine
};


//  EXPLOSIVE BRICK  (inherits from Brick)
//  Takes only 1 hit, color red, points 50
//  hit() overridden: ready for explosion effect later
class ExplosiveBrick : public Brick
{
public:
    ExplosiveBrick(float x, float y, float w, float h);

    // Overriding hit() - this is POLYMORPHISM
    bool hit() override;
};


//  MOVING BRICK  (inherits from Brick)
//  Slides left and right automatically, color green, points 30
//  update() overridden to move every frame
class MovingBrick : public Brick
{
private:
    float speed;   // how fast it moves
    float minX;    // left boundary
    float maxX;    // right boundary

public:
    MovingBrick(float x, float y, float w, float h, float leftBound, float rightBound);

    // Overriding update() - this is POLYMORPHISM
    void update(float dt) override;
};
