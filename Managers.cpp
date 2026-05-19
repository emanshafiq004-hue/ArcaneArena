#include "Managers.h"
#include <ctime>
#include <algorithm>


void ScoreManager::reset(int best) {
    score_ = 0; coins_ = 0; multiplier_ = false;
    multTimer_ = stormSlow_ = 0.f;
    bestScore_ = best; shield_ = false;
}

void ScoreManager::addBarrier() {
    score_ += (multiplier_ ? 2 : 1);
    GameStats::totalBarriersPassed++;
}

void ScoreManager::addCrystal() {
    score_ += (multiplier_ ? 10 : 5);
    coins_++;
    GameStats::totalCrystalsCollected++;
}

void ScoreManager::activateMulti() { multiplier_ = true; multTimer_ = 10.f; }
void ScoreManager::activateStorm() { stormSlow_ = 3.f; }
void ScoreManager::setShield(bool v) { shield_ = v; }

void ScoreManager::update(float dt) {
    if (multiplier_) { multTimer_ -= dt; if (multTimer_ <= 0.f) multiplier_ = false; }
    if (stormSlow_ > 0.f) stormSlow_ -= dt;
}

int ScoreManager::tier() const {
    if (score_ <= 20) return 0;
    if (score_ <= 25) return 1;
    if (score_ <= 50) return 2;
    return 3;
}

float ScoreManager::scrollSpeed() const {
    static const float spds[] = { 180.f,240.f,310.f,400.f };
    return spds[tier()] * (stormSlow_ > 0.f ? 0.55f : 1.f);
}

float ScoreManager::gapSize() const {
    static const float gaps[] = { 210.f,180.f,150.f,128.f };
    return gaps[tier()];
}

std::vector<ScoreEntry> FileManager_SkySurge::loadScores() {
    std::vector<ScoreEntry> entries;
    try {
        std::ifstream f(SCORE_FILE);
        if (!f) throw std::runtime_error("Score file not found");
        std::string line;
        while (std::getline(f, line)) {
            std::istringstream ss(line);
            ScoreEntry e; std::string sc;
            if (std::getline(ss, e.name, ',') &&
                std::getline(ss, sc, ',') &&
                std::getline(ss, e.date))
            {
                e.score = std::stoi(sc);
                entries.push_back(e);
            }
        }
    }
    catch (const std::exception&) { /* file missing — return empty */ }
    return entries;
}

void FileManager_SkySurge::saveScores(const std::vector<ScoreEntry>& entries) {
    try {
        std::ofstream f(SCORE_FILE);
        if (!f) throw std::runtime_error("Cannot write score file");
        for (auto& e : entries)
            f << e.name << "," << e.score << "," << e.date << "\n";
    }
    catch (const std::exception& ex) { (void)ex; }
}

void FileManager_SkySurge::addScore(const std::string& name, int score) {
    auto entries = loadScores();
    std::time_t t = std::time(nullptr);
    std::tm tmBuf{};
#ifdef _MSC_VER
    localtime_s(&tmBuf, &t);
#else
    localtime_r(&t, &tmBuf);
#endif
    std::ostringstream ds;
    ds << (tmBuf.tm_year + 1900) << "-"
        << std::setw(2) << std::setfill('0') << (tmBuf.tm_mon + 1) << "-"
        << std::setw(2) << std::setfill('0') << tmBuf.tm_mday;
    entries.push_back(ScoreEntry(name, score, ds.str()));
    std::sort(entries.begin(), entries.end(),
        [](const ScoreEntry& a, const ScoreEntry& b) { return a > b; });
    if ((int)entries.size() > TOP5) entries.resize(TOP5);
    saveScores(entries);
}

int FileManager_SkySurge::getBestScoreFromFile() {
    auto e = loadScores();
    return e.empty() ? 0 : e[0].score;
}

FileManager_SkySurge::Settings FileManager_SkySurge::loadSettings() {
    Settings s;
    try {
        std::ifstream f(SETTING_FILE);
        if (!f) throw std::runtime_error("Settings file missing");
        int v = 1; f >> v; s.soundOn = (v != 0);
    }
    catch (...) {}
    return s;
}

void FileManager_SkySurge::saveSettings(const Settings& s) {
    std::ofstream f(SETTING_FILE);
    f << (s.soundOn ? 1 : 0) << "\n";
}

//  GameWorld

void GameWorld::clear() {
    barriers.clear(); obstacles.clear(); collectibles.clear();
    barrierSpawnT = obstacleSpawnT = collectSpawnT = 0.f;
}

void GameWorld::spawnObstacle(float x, float spd, int tier) {
    Vec2 pos(x, randRange(50.f, BASE_H - 80.f));
    if (tier >= 1 && randRange(0, 2) == 0)
        obstacles.push_back(std::make_unique<Missile>(pos, spd * 0.9f + 20.f));
    else
        obstacles.push_back(std::make_unique<StormCloud>(pos, spd * 0.9f + 20.f));
}

void GameWorld::spawnCollectible(float x, float spd) {
    Vec2 pos(x, randRange(60.f, BASE_H - 60.f));
    int r = randRange(0, 4);
    if (r == 0)      collectibles.push_back(std::make_unique<ShieldPowerUp>(pos, spd));
    else if (r == 1) collectibles.push_back(std::make_unique<MultiplierPowerUp>(pos, spd));
    else           collectibles.push_back(std::make_unique<Crystal>(pos, spd));
}

void GameWorld::update(float dt, float spd, ScoreManager& sm) {
    //__________________Barriers__________________________
    float interval = std::max(0.9f, 1.8f - sm.tier() * 0.25f);
    barrierSpawnT += dt;
    if (barrierSpawnT >= interval) {
        barrierSpawnT = 0.f;
        float gs = sm.gapSize();
        float gy = randRange(40.f, BASE_H - gs - 40.f);
        barriers.emplace_back(BASE_W + 10.f, gy, gs, sm.tier() >= 3);
    }
    for (auto& b : barriers) { b.update(dt); b.scroll(spd, dt); }
    barriers.erase(std::remove_if(barriers.begin(), barriers.end(),
        [](const Barrier& b) { return b.offScreen(); }), barriers.end());

    // Score barriers passed
    for (auto& b : barriers)
        if (!b.scored && b.getX() + BARRIER_W < DRONE_X)
        {
            b.scored = true; sm.addBarrier();
        }

    //_____________________Obstacles____________________
    if (sm.tier() >= 1) {
        obstacleSpawnT += dt;
        float oi = (sm.tier() < 2) ? 6.f : 3.5f;
        if (obstacleSpawnT >= oi)
        {
            obstacleSpawnT = 0.f; spawnObstacle(BASE_W + 40.f, spd, sm.tier());
        }
    }
    //Polymorphic update via virtual dispatch
    for (auto& o : obstacles) if (o->isActive()) o->update(dt);
    obstacles.erase(std::remove_if(obstacles.begin(), obstacles.end(),
        [](const std::unique_ptr<Obstacle>& o) { return !o->isActive(); }), obstacles.end());

    //Collectibles
    collectSpawnT += dt;
    if (collectSpawnT >= 4.f)
    {
        collectSpawnT = 0.f; spawnCollectible(BASE_W + 20.f, spd);
    }
    for (auto& c : collectibles) if (c->isActive()) c->update(dt);
    collectibles.erase(std::remove_if(collectibles.begin(), collectibles.end(),
        [](const std::unique_ptr<Collectible>& c) { return !c->isActive(); }), collectibles.end());
}

void GameWorld::draw(sf::RenderTarget& rt) {
    //  Each call resolves via vtable to correct subclass draw()
    for (auto& b : barriers)     b.draw(rt);
    for (auto& o : obstacles)    if (o->isActive()) o->draw(rt);
    for (auto& c : collectibles) if (c->isActive()) c->draw(rt);
}

void GameWorld::checkCollisions(Drone& drone, ScoreManager& sm,
    ParticleSystem& ps) {
    coinCollected = false;
    if (drone.isDead()) return;

    // If drone is invincible (just absorbed a hit) skip all damage checks
    if (drone.isInvincible()) {
        // Still collect collectibles while invincible
        sf::FloatRect db = drone.getBounds();
        for (auto& c : collectibles) {
            if (!c->isActive()) continue;
            if (!db.findIntersection(c->getBounds()).has_value()) continue;
            sf::Color pc = c->getColor();

            // Play coin sound only for crystals
            if (dynamic_cast<Crystal*>(c.get()))
                coinCollected = true;

            c->onCollect(sm, drone);
            ps.emit(c->getPos(), pc, 20, 100.f, 0.35f, 3.f);
            c->setActive(false);
        }
        return;  // skip all damage this frame
    }

    sf::FloatRect db = drone.getBounds();

    //Barrier collisions 
    for (auto& b : barriers) {
        bool hitTop = db.findIntersection(b.topBounds()).has_value();
        bool hitBot = db.findIntersection(b.botBounds()).has_value();

        if (hitTop || hitBot) {
            if (drone.hasShield()) {
                drone.breakShield();   // sets invincTimer_ = 1.2f inside
                sm.setShield(false);
                // Bounce drone away from barrier
                Vec2 p = drone.getPos();
                p.x -= 25.f;
                drone.setPos(p);
            }
            else {
                drone.kill();
                ps.emitExplosion(drone.getPos());
            }
            return;  // exit entire collision check this frame
        }
    }

    // Obstacle collisions 
    for (auto& o : obstacles) {
        if (!o->isActive()) continue;
        if (!db.findIntersection(o->getBounds()).has_value()) continue;

        if (drone.hasShield()) {
            drone.breakShield();
            sm.setShield(false);
            o->setActive(false);
        }
        else {
            o->applyEffect(sm, drone);
            if (drone.isDead())
                ps.emitExplosion(drone.getPos());
        }
        return;  // exit entire collision check this frame
    }

    // ── Collectible collisions ───────────────────────────────
    for (auto& c : collectibles) {
        if (!c->isActive()) continue;
        if (!db.findIntersection(c->getBounds()).has_value()) continue;
        sf::Color pc = c->getColor();
        if (dynamic_cast<Crystal*>(c.get()))
            coinCollected = true;
        c->onCollect(sm, drone);
        ps.emit(c->getPos(), pc, 20, 100.f, 0.35f, 3.f);
        c->setActive(false);
    }
}