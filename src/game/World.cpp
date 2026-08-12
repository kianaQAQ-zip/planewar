#include "World.h"
#include "core/AssetManager.h"
#include "core/AudioManager.h"
#include "core/Renderer.h"
#include <cmath>

void World::LoadAssets(AssetManager& assets)
{
    // 资源登记集中在此：缺图不报错（Renderer 用圆形占位），缺字体不报错（HUD 跳过文本）。
    assets.LoadFont("assets/font.ttf");

    assets.LoadTexture(SpriteId::Background,   "assets/background.png");
    assets.LoadTexture(SpriteId::Player,       "assets/player.png");
    assets.LoadTexture(SpriteId::EnemyBasic,   "assets/enemy-basic.png");
    assets.LoadTexture(SpriteId::EnemyFast,    "assets/enemy-fast.png");
    assets.LoadTexture(SpriteId::EnemyTank,    "assets/enemy-tank.png");
    assets.LoadTexture(SpriteId::BulletPlayer, "assets/bullet.png");
    assets.LoadTexture(SpriteId::BulletEnemy,  "assets/bullet-enemy.png");
    assets.LoadTexture(SpriteId::Boss,         "assets/boss.png");
    assets.LoadTexture(SpriteId::PowerUp,      "assets/powerup.png");

    // 音效（M3）：缺失则静默降级，不影响玩法。需要 SFML 音频 + openal32.dll。
    auto& audio = AudioManager::Instance();
    audio.LoadSound("shoot",       "assets/sfx/shoot.ogg");
    audio.LoadSound("explosion",   "assets/sfx/explosion.ogg");
    audio.LoadSound("player_hit",  "assets/sfx/player-hit.ogg");
    audio.LoadSound("powerup",     "assets/sfx/powerup.ogg");
    audio.LoadSound("boss_spawn",  "assets/sfx/boss-spawn.ogg");
    audio.LoadSound("boss_hit",    "assets/sfx/boss-hit.ogg");
    audio.LoadSound("boss_death",  "assets/sfx/boss-death.ogg");
}

void World::Reset()
{
    player_.Reset({GameConfig::WORLD_WIDTH * 0.5f, GameConfig::WORLD_HEIGHT - 60.f});

    bullets_.ForEachActive([](Bullet& b) { b.SetActive(false); });
    enemyBullets_.ForEachActive([](Bullet& b) { b.SetActive(false); });
    enemies_.ForEachActive([](Enemy& e)  { e.SetActive(false);  });
    items_.ForEachActive([](PowerUp& it){ it.SetActive(false); });
    particles_.ForEachActive([](Particle& p){ p.SetActive(false); });
    boss_.Deactivate();

    scoreManager_.Reset();
    waveManager_.Reset();
    gameOver_    = false;
    won_        = false;
    bossSpawned_ = false;
    itemTimer_   = GameConfig::ITEM_SPAWN_INTERVAL;
}

void World::Update(float dt, const InputState& input)
{
    if (gameOver_ || won_)
        return;

    // 1) 玩家输入 + 自身移动（HandleInput 返回是否开火，用于触发音效）
    const bool fired = player_.HandleInput(input, bullets_);
    if (fired)
        AudioManager::Instance().Play("shoot");
    player_.Update(dt);

    // 2) 池内对象各自推进
    bullets_.ForEachActive([dt](Bullet& b) { b.Update(dt); });
    enemyBullets_.ForEachActive([dt](Bullet& b) { b.Update(dt); });
    enemies_.ForEachActive([dt](Enemy& e)  { e.Update(dt);  });

    // 3) 波次 → 清空后召唤 Boss（只召唤一次）
    if (!waveManager_.IsFinished())
    {
        waveManager_.Update(dt, enemies_);
    }
    else if (!bossSpawned_)
    {
        boss_.Activate();
        bossSpawned_ = true;
        bus_.Emit(BossSpawnedEvent{});
        AudioManager::Instance().Play("boss_spawn");
    }

    // 4) Boss 推进（弹幕通过事件解耦给 World 生成敌弹）
    if (boss_.IsActive())
        boss_.Update(dt, player_.GetPosition(), bus_);

    // 5) 道具周期掉落（波次阶段与 Boss 阶段都会掉）
    itemTimer_ -= dt;
    if (itemTimer_ <= 0.f)
    {
        SpawnItem();
        itemTimer_ = GameConfig::ITEM_SPAWN_INTERVAL +
                     (static_cast<float>(std::rand()) / RAND_MAX) * 4.f;
    }
    items_.ForEachActive([dt](PowerUp& it) { it.Update(dt); });

    // 6) 粒子推进
    particles_.ForEachActive([dt](Particle& p) { p.Update(dt); });

    // 7) 碰撞结算
    HandleCollisions();
}

void World::SpawnBossBullets(const BossFireEvent& e)
{
    const float speed = GameConfig::ENEMY_BULLET_SPEED;
    auto spawn = [&](float vx, float vy)
    {
        if (Bullet* b = enemyBullets_.Acquire())
        {
            b->SetSprite(SpriteId::BulletEnemy);
            b->SetFallbackColor(0xFF66CCFF);   // 粉红敌弹
            b->SetPosition(e.origin);
            b->SetVelocity({vx, vy});
        }
    };

    if (e.pattern == 1)
    {
        // 五路扇形（以正下方为基准）
        for (int i = -2; i <= 2; ++i)
        {
            const float a = i * 0.18f;         // 弧度偏转
            spawn(std::sin(a) * speed, std::cos(a) * speed);
        }
    }
    else
    {
        // 八路环形爆发
        for (int i = 0; i < 8; ++i)
        {
            const float a = i * (6.2831853f / 8.f);
            spawn(std::cos(a) * speed, std::sin(a) * speed);
        }
    }
}

void World::SpawnExplosion(const Vec2& pos, unsigned int color, int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (Particle* p = particles_.Acquire())
        {
            const float ang = (static_cast<float>(std::rand()) / RAND_MAX) * 6.2831853f;
            const float spd = 40.f + (static_cast<float>(std::rand()) / RAND_MAX) * 160.f;
            p->Init(pos, {std::cos(ang) * spd, std::sin(ang) * spd},
                    0.4f + (static_cast<float>(std::rand()) / RAND_MAX) * 0.4f, color);
        }
    }
}

void World::SpawnItem()
{
    if (PowerUp* it = items_.Acquire())
    {
        it->RandomInit();                      // 随机挑一种效果（含精灵/颜色）
        const float r = it->GetRadius();
        const float x = r + (static_cast<float>(std::rand()) / RAND_MAX) *
                            (GameConfig::WORLD_WIDTH - 2.f * r);
        it->SetPosition({x, -r});
        it->SetVelocity({0.f, GameConfig::ITEM_SPEED});
    }
}

void World::HandleCollisions()
{
    auto& bs  = bullets_.Raw();
    auto& ebs = enemyBullets_.Raw();
    auto& es  = enemies_.Raw();
    auto& its = items_.Raw();

    // 玩家子弹 × 敌机：命中则双方回收，发击杀事件（ScoreManager 订阅加分）
    for (auto& b : bs)
    {
        if (!b.IsActive()) continue;
        for (auto& e : es)
        {
            if (!e.IsActive()) continue;
            if (CircleIntersect(b.GetPosition(), b.GetRadius(),
                                e.GetPosition(), e.GetRadius()))
            {
                b.SetActive(false);
                e.SetActive(false);
                bus_.Emit(EnemyKilledEvent{ GameConfig::SCORE_PER_KILL });
                AudioManager::Instance().Play("explosion");
                SpawnExplosion(e.GetPosition(), 0xFF8844FF, 10);
                break;
            }
        }
    }

    // 玩家子弹 × Boss：命中扣血；致死则触发胜利与大爆炸
    if (boss_.IsActive())
    {
        for (auto& b : bs)
        {
            if (!b.IsActive()) continue;
            if (!boss_.IsActive()) break;      // 已被上一发打死
            if (CircleIntersect(b.GetPosition(), b.GetRadius(),
                                boss_.GetPosition(), boss_.GetRadius()))
            {
                b.SetActive(false);
                const bool died = boss_.TakeDamage(1.f);
                AudioManager::Instance().Play("boss_hit");
                SpawnExplosion(b.GetPosition(), 0xFFCC33FF, 4);
                if (died)
                {
                    bus_.Emit(BossDefeatedEvent{});
                    bus_.Emit(EnemyKilledEvent{ GameConfig::BOSS_SCORE });
                    AudioManager::Instance().Play("boss_death");
                    SpawnExplosion(boss_.GetPosition(), 0xFFCC33FF, 60);
                    won_ = true;
                }
            }
        }
    }

    // 敌弹 × 玩家：可受击时才扣命；无敌/护盾期间仅吸收子弹
    if (player_.CanBeHit())
    {
        for (auto& eb : ebs)
        {
            if (!eb.IsActive()) continue;
            if (CircleIntersect(eb.GetPosition(), eb.GetRadius(),
                                player_.GetPosition(), player_.GetRadius()))
            {
                eb.SetActive(false);
                player_.LoseLife();
                bus_.Emit(PlayerHitEvent{ player_.GetLives() });
                AudioManager::Instance().Play("player_hit");
                SpawnExplosion(player_.GetPosition(), 0x44CCFFFF, 12);
                if (player_.IsDead()) gameOver_ = true;
                break;
            }
        }
    }
    else
    {
        for (auto& eb : ebs)
        {
            if (!eb.IsActive()) continue;
            if (CircleIntersect(eb.GetPosition(), eb.GetRadius(),
                                player_.GetPosition(), player_.GetRadius()))
                eb.SetActive(false);           // 护盾/无敌吸收
        }
    }

    // 玩家 × 敌机：命中扣命（事件留给后续闪屏/音效）
    if (player_.CanBeHit())
    {
        for (auto& e : es)
        {
            if (!e.IsActive()) continue;
            if (CircleIntersect(player_.GetPosition(), player_.GetRadius(),
                                e.GetPosition(), e.GetRadius()))
            {
                e.SetActive(false);
                player_.LoseLife();
                bus_.Emit(PlayerHitEvent{ player_.GetLives() });
                AudioManager::Instance().Play("player_hit");
                SpawnExplosion(player_.GetPosition(), 0x44CCFFFF, 12);
                if (player_.IsDead()) gameOver_ = true;
                break;
            }
        }
    }

    // 玩家 × Boss 本体：撞上扣命（只扣一次，靠无敌帧避免持续掉血）
    if (boss_.IsActive() && player_.CanBeHit())
    {
        if (CircleIntersect(player_.GetPosition(), player_.GetRadius(),
                            boss_.GetPosition(), boss_.GetRadius()))
        {
            player_.LoseLife();
            bus_.Emit(PlayerHitEvent{ player_.GetLives() });
            AudioManager::Instance().Play("player_hit");
            SpawnExplosion(player_.GetPosition(), 0x44CCFFFF, 12);
            if (player_.IsDead()) gameOver_ = true;
        }
    }

    // 玩家 × 道具：拾取执行策略效果
    for (auto& it : its)
    {
        if (!it.IsActive()) continue;
        if (CircleIntersect(player_.GetPosition(), player_.GetRadius(),
                            it.GetPosition(), it.GetRadius()))
        {
            it.ApplyTo(player_);               // 委托给策略对象
            it.SetActive(false);
            bus_.Emit(ItemPickupEvent{});
            AudioManager::Instance().Play("powerup");
            SpawnExplosion(it.GetPosition(), it.GetFallbackColor(), 8);
        }
    }
}

void World::Render(Renderer& renderer)
{
    if (boss_.IsActive()) boss_.Draw(renderer);
    enemies_.ForEachActive([&](const Enemy&  e) { e.Draw(renderer);  });
    player_.Draw(renderer);
    bullets_.ForEachActive([&](const Bullet& b) { b.Draw(renderer); });
    enemyBullets_.ForEachActive([&](const Bullet& b) { b.Draw(renderer); });
    items_.ForEachActive([&](const PowerUp& it) { it.Draw(renderer); });
    particles_.ForEachActive([&](const Particle& p) { p.Draw(renderer); });

    // HUD：分数 / 生命 / 波次 / Boss 血条 / 玩家状态
    const int bossHp = boss_.IsActive()
        ? static_cast<int>(boss_.GetHealthRatio() * 100.f) : -1;
    hud_.Draw(renderer, scoreManager_.GetScore(), player_.GetLives(),
              waveManager_.GetCurrentWave(), bossHp,
              player_.GetWeaponLevel(), player_.IsShielded());
}
