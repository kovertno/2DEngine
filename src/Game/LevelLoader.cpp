#include "LevelLoader.h"
// Maybe in the future store all components in one header file + cpp files?
#include "Game.h"
#include <fstream>
#include <sstream>
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/KeyboardControlledComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/ScriptComponent.h"
#include "../Components/TextLabelComponent.h"

LevelLoader::LevelLoader() {
    Logger::Log("LevelLoader constructor called!");
}

LevelLoader::~LevelLoader() {
    Logger::Log("LevelLoader destructor called!");
}

void LevelLoader::LoadLevel(sol::state& lua, const std::unique_ptr<Registry>& registry, const std::unique_ptr<AssetManager>& assetManager, SDL_Renderer* renderer, int levelNumber) {

    // This checks the syntax of the script, but does not execute it
    sol::load_result script = lua.load_file("./assets/scripts/Level" + std::to_string(levelNumber) + ".lua");
    if (!script.valid()) {
        sol::error err = script;
        std::string errorMessage = err.what();
        Logger::Err("Error loading Level: " + errorMessage);
        return;
    }

    // Load the entities and components from Level lua script
    lua.script_file("./assets/scripts/Level" + std::to_string(levelNumber) + ".lua");

    // Read the big table for the whole level
    sol::table level = lua["Level"];

    // Read the level assets
    sol::table assets = level["assets"];

    int i = 0;
    while (true) {
        sol::optional<sol::table> hasAsset = assets[i];
        if (hasAsset == sol::nullopt) {
            break;
        }
        sol::table asset = assets[i];
        std::string assetType = asset["type"];
        if (assetType == "texture") {
            assetManager->AddTexture(renderer, asset["id"], asset["file"]);
            std::string assetId = asset["id"];
            Logger::Log("A new texture asset was added to the asset store: " + assetId);
        }
        if (assetType == "font") {
            assetManager->AddFont(asset["id"], asset["file"], asset["font_size"]);
            std::string assetId = asset["id"];
            Logger::Log("A new font  asset was added to the asset store: " + assetId);
        }
        i++;
    }

    // Read the level tilemap info
    sol::table map = level["tilemap"];
    std::string mapFilePath = map["map_file"];
    std::string mapTextureAssetId = map["texture_asset_id"];
    int mapNumRows = map["num_rows"];
    int mapNumCols = map["num_cols"];
    int tileSize = map["tile_size"];
    double mapScale = map["scale"];
    std::fstream mapFile;
    mapFile.open(mapFilePath);
    for(int row = 0; row <mapNumRows; row++) {
        for(int col = 0; col < mapNumCols; col++){
            char ch;
            mapFile.get(ch);
            int srcRectY = std::atoi(&ch) * tileSize;
            mapFile.get(ch);
            int srcRectX = std::atoi(&ch) * tileSize;
            mapFile.ignore();


            Entity tile = registry->CreateEntity();
            tile.Group("tiles");
            tile.AddComponent<TransformComponent>(glm::vec2(tileSize*mapScale*col, tileSize*mapScale*row), glm::vec2(mapScale, mapScale), 0);
            tile.AddComponent<SpriteComponent>(mapTextureAssetId, tileSize, tileSize, 0, false, srcRectX, srcRectY);
        }
    }
    mapFile.close();
    Game::mapWidth = mapNumCols * tileSize * mapScale;
    Game::mapHeight = mapNumRows * tileSize * mapScale;

    // Read entities and their components
    sol::table entities = level["entities"];
    i = 0;
    while (true) {
        sol::optional<sol::table> hasEntity = entities[i];
        if (hasEntity == sol::nullopt) {
            break;
        }

        sol::table entity = entities[i];
        Entity newEntity = registry->CreateEntity();

        // Tag
        sol::optional<std::string> tag = entity["tag"];
        if (tag != sol::nullopt) {
            newEntity.Tag(entity["tag"]);
        }

        // Group
        sol::optional<std::string> group= entity["group"];
        if (group != sol::nullopt) {
            newEntity.Group(entity["group"]);
        }

        // Components
        sol::optional<sol::table> hasComponents = entity["components"];
        if (hasComponents != sol::nullopt) {
            // Transform
            sol::optional<sol::table> transform = entity["components"]["transform"];
            if (transform != sol::nullopt) {
                newEntity.AddComponent<TransformComponent>(
                    glm::vec2 (
                        entity["components"]["transform"]["position"]["x"],
                        entity["components"]["transform"]["position"]["y"]
                    ),
                    glm::vec2 (
                        entity["components"]["transform"]["scale"]["x"].get_or(1.0),
                        entity["components"]["transform"]["scale"]["y"].get_or(1.0)
                    ),
                    entity["components"]["transform"]["rotation"].get_or(0.0)
                );
            }

            // RigidBody
            sol::optional<sol::table> rigidbody = entity["components"]["rigidbody"];
            if (rigidbody != sol::nullopt) {
                newEntity.AddComponent<RigidBodyComponent>(
                    glm::vec2(
                        entity["components"]["rigidbody"]["velocity"]["x"].get_or(0.0),
                        entity["components"]["rigidbody"]["velocity"]["y"].get_or(0.0)
                    )
                );
            }

            // Sprite
            sol::optional<sol::table> sprite = entity["components"]["sprite"];
            if (sprite != sol::nullopt) {
                newEntity.AddComponent<SpriteComponent>(
                    entity["components"]["sprite"]["texture_asset_id"],
                    entity["components"]["sprite"]["width"],
                    entity["components"]["sprite"]["height"],
                    entity["components"]["sprite"]["z_index"].get_or(1),
                    entity["components"]["sprite"]["fixed"].get_or(false),
                    entity["components"]["sprite"]["src_rect_x"].get_or(0),
                    entity["components"]["sprite"]["src_rect_y"].get_or(0)
                );
            }

            // Animation
            sol::optional<sol::table> animation = entity["components"]["animation"];
            if (animation != sol::nullopt) {
                newEntity.AddComponent<AnimationComponent>(
                    entity["components"]["animation"]["num_frames"].get_or(1),
                    entity["components"]["animation"]["speed_rate"].get_or(1)
                );
            }

            // BoxCollider
            sol::optional<sol::table> collider = entity["components"]["boxcollider"];
            if (collider != sol::nullopt) {
                newEntity.AddComponent<BoxColliderComponent>(
                    entity["components"]["boxcollider"]["width"],
                    entity["components"]["boxcollider"]["height"],
                    glm::vec2(
                        entity["components"]["boxcollider"]["offset"]["x"].get_or(0),
                        entity["components"]["boxcollider"]["offset"]["y"].get_or(0)
                    )
                );
            }

            // Health
            sol::optional<sol::table> health = entity["components"]["health"];
            if (health != sol::nullopt) {
                newEntity.AddComponent<HealthComponent>(
                    static_cast<int>(entity["components"]["health"]["health_percentage"].get_or(100))
                );
            }

            // ProjectileEmitter
            sol::optional<sol::table> projectileEmitter = entity["components"]["projectile_emitter"];
            if (projectileEmitter != sol::nullopt) {
                newEntity.AddComponent<ProjectileEmitterComponent>(
                    glm::vec2(
                        entity["components"]["projectile_emitter"]["projectile_velocity"]["x"],
                        entity["components"]["projectile_emitter"]["projectile_velocity"]["y"]
                    ),
                    static_cast<int>(entity["components"]["projectile_emitter"]["repeat_frequency"].get_or(1)) * 1000,
                    static_cast<int>(entity["components"]["projectile_emitter"]["projectile_duration"].get_or(10)) * 1000,
                    static_cast<int>(entity["components"]["projectile_emitter"]["hit_percentage_damage"].get_or(10)),
                    entity["components"]["projectile_emitter"]["friendly"].get_or(false)
                );
            }

            // CameraFollow
            sol::optional<sol::table> cameraFollow = entity["components"]["camera_follow"];
            if (cameraFollow != sol::nullopt) {
                newEntity.AddComponent<CameraFollowComponent>();
            }

            // KeyboardControlled
            sol::optional<sol::table> keyboardControlled = entity["components"]["keyboard_controller"];
            if (keyboardControlled != sol::nullopt) {
                newEntity.AddComponent<KeyboardControlledComponent>(
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["up_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["up_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["right_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["right_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["down_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["down_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["left_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["left_velocity"]["y"]
                    )
                );
            }

            // Script
            sol::optional<sol::table> script = entity["components"]["on_update_script"];
            if (script != sol::nullopt) {
                sol::function func = entity["components"]["on_update_script"][0];
                newEntity.AddComponent<ScriptComponent>(func);
            }
        }
        i++;
    }
}







    // // Adding the assets to the asset manager.
    // assetManager->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
    // assetManager->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-down.png");
    // assetManager->AddTexture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
    // assetManager->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
    // assetManager->AddTexture(renderer, "bullet-image", "./assets/images/bullet.png");
    // assetManager->AddTexture(renderer, "tree-image", "./assets/images/tree.png");
    // assetManager->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");
    // assetManager->AddFont("charriot-font", "./assets/fonts/charriot.ttf", 20);
    // assetManager->AddFont("arial-font-5", "./assets/fonts/arial.ttf", 5);
    // assetManager->AddFont("arial-font-10", "./assets/fonts/arial.ttf", 10);
    //
    //
    // // Load the tile map
    // int mapNumCols = 25;
    // int mapNumRows = 20;
    // int tileSize = 32;
    // double tileScale = 2.0;
    //
    // std::string line;
    // std::ifstream readFile("./assets/tilemaps/jungle.map");
    // for(int row = 0; std::getline(readFile, line); row++) {
    //     std::string value;
    //     std::stringstream ss(line);
    //     for(int col = 0; std::getline(ss, value, ','); col++){
    //         int id = std::stoi(value);
    //         int srcRectX = tileSize*(id%10);
    //         int srcRectY = tileSize*(id/10);
    //
    //         Entity tile = registry->CreateEntity();
    //         tile.Group("tiles");
    //         tile.AddComponent<TransformComponent>(glm::vec2(tileSize*tileScale*col, tileSize*tileScale*row), glm::vec2(tileScale, tileScale), 0);
    //         tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, 0, srcRectX, srcRectY);
    //     }
    // }
    // readFile.close();
    //
    // Game::mapWidth = mapNumCols * tileSize * tileScale;
    // Game::mapHeight = mapNumRows * tileSize * tileScale;
    //
    // // Create some entities
    // Entity chopper = registry->CreateEntity();
    // chopper.Tag("player");
    // chopper.AddComponent<TransformComponent>(glm::vec2(100.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
    // chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    // chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 2);
    // chopper.AddComponent<AnimationComponent>(2, 7, true);
    // chopper.AddComponent<BoxColliderComponent>(32, 32);
    // chopper.AddComponent<KeyboardControlledComponent>(glm::vec2(0.0, -80.0), glm::vec2(80.0, 0), glm::vec2(0.0, 80.0), glm::vec2(-80.0, 0.0));
    // chopper.AddComponent<CameraFollowComponent>();
    // chopper.AddComponent<HealthComponent>(100);
    // chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(150.0, 150.0), 0, 10000, 10, true);
    //
    // Entity tank = registry->CreateEntity();
    // tank.Group("enemies");
    // tank.AddComponent<TransformComponent>(glm::vec2(500.0, 500.0), glm::vec2(1.0, 1.0), 0.0);
    // tank.AddComponent<RigidBodyComponent>(glm::vec2(20.0, 0.0));
    // tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 1);
    // tank.AddComponent<BoxColliderComponent>(32, 32);
    // tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0.0), 2000, 8000, 10, false);
    // tank.AddComponent<HealthComponent>(100);
    //
    // // Entity truck = registry->CreateEntity();
    // // truck.Group("enemies");
    // // truck.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
    // // truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    // // truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
    // // truck.AddComponent<BoxColliderComponent>(32, 32);
    // // truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0.0), 5000, 3000, 10, false);
    // // truck.AddComponent<HealthComponent>(100);
    //
    // Entity radar = registry->CreateEntity();
    // radar.AddComponent<TransformComponent>(glm::vec2(Game::windowWidth-74, 10.0), glm::vec2(1.0, 1.0), 0.0);
    // radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 3, true);
    // radar.AddComponent<AnimationComponent>(8, 5, true);
    //
    // Entity treeA = registry->CreateEntity();
    // treeA.Group("obstacles");
    // treeA.AddComponent<TransformComponent>(glm::vec2(600.0, 495.0), glm::vec2(1.0, 1.0), 0.0);
    // treeA.AddComponent<SpriteComponent>("tree-image", 16, 32, 1);
    // treeA.AddComponent<BoxColliderComponent>(16, 32);
    //
    // Entity treeB= registry->CreateEntity();
    // treeB.Group("obstacles");
    // treeB.AddComponent<TransformComponent>(glm::vec2(400.0, 495.0), glm::vec2(1.0, 1.0), 0.0);
    // treeB.AddComponent<SpriteComponent>("tree-image", 16, 32, 1);
    // treeB.AddComponent<BoxColliderComponent>(16, 32);

