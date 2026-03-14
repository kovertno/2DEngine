#ifndef RENDER_GUI_SYSTEM_H
#define RENDER_GUI_SYSTEM_H

#include "../ECS/ECS.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>

class RenderGUISystem : public System {
    private:

    public:
    void Update(SDL_Renderer* renderer, const SDL_Rect& camera, const std::unique_ptr<Registry>& registry) {

        ImGui_ImplSDLRenderer2_NewFrame();

        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (ImGui:: Begin("Spawn enemies")) {
            // Static variables to store input values
            static int posX = 0;
            static int posY = 0;
            static int scaleX = 1;
            static int scaleY = 1;
            static int velX = 0;
            static int velY = 0;
            static int health = 100;
            static float rotation = 0.0;
            static float projAngle = 0.0;
            static float projSpeed = 100.0;
            static int projRepeat = 10;
            static int projDuration = 10;
            const char* sprites[] = {"tank-texture", "truck-texture"};
            static int selectedSpriteIndex = 0;

            // Section to input enemy sprite texture id
            if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Combo("texture id", &selectedSpriteIndex, sprites, IM_ARRAYSIZE(sprites));
            }
            ImGui::Spacing();

            // Section to input enemy transform values
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::InputInt("position x", &posX);
                ImGui::InputInt("position y", &posY);
                ImGui::SliderInt("scale x", &scaleX, 1, 10);
                ImGui::SliderInt("scale y", &scaleY, 1, 10);
                ImGui::SliderAngle("rotation (deg)", &rotation, 0, 360);
            }
            ImGui::Spacing();

            // Section to input enemy rigid body values
            if (ImGui::CollapsingHeader("Rigid body", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::InputInt("velocity x", &velX);
                ImGui::InputInt("velocity y", &velY);
            }
            ImGui::Spacing();

            // Section to input enemy projectile emitter values
            if (ImGui::CollapsingHeader("Projectile emitter", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderAngle("angle (deg)", &projAngle, 0, 360);
                ImGui::SliderFloat("speed (px/sec)", &projSpeed, 10, 500);
                ImGui::InputInt("repeat (sec)", &projRepeat);
                ImGui::InputInt("duration (sec)", &projDuration);
            }
            ImGui::Spacing();

            // Section to input enemy health values
            if (ImGui::CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderInt("%", &health, 0, 100);
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // A button to create enemy
            if (ImGui::Button("Create new enemy")) {
                Entity enemy = registry->CreateEntity();
                enemy.Group("enemies");
                enemy.AddComponent<TransformComponent>(glm::vec2(posX, posY), glm::vec2(scaleX, scaleY), glm::degrees(rotation));
                enemy.AddComponent<RigidBodyComponent>(glm::vec2(velX, velY));
                enemy.AddComponent<SpriteComponent>(sprites[selectedSpriteIndex], 32, 32, 3);
                enemy.AddComponent<BoxColliderComponent>(32 * scaleX, 32 * scaleY);
                double projVelX = cos(projAngle) * projSpeed; // cos = x/distance -> x = cos * distance
                double projVelY = sin(projAngle) * projSpeed; // sin = y/distance -> y = sin * distance
                enemy.AddComponent<ProjectileEmitterComponent>(glm::vec2(projVelX, projVelY), projRepeat * 1000, projDuration * 1000, 10, false);
                enemy.AddComponent<HealthComponent>(health);

                // Reset all input values after we create an enemy
                posX = posY = rotation = projAngle = 0;
                scaleX = scaleY = 1;
                projRepeat = projDuration = 10;
                projSpeed = health =  100;
            }
        }
        ImGui::End();

        // Display a small overlay window to display the map position of the mouse
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always, ImVec2(0, 0));
        ImGui::SetNextWindowBgAlpha(0.9f);
        if (ImGui::Begin("Map coordinates", NULL, windowFlags)) {
            ImGui::Text(
                "Map coordinates (x=%.1f, y=%.1f)",
                ImGui::GetIO().MousePos.x + camera.x,
                ImGui::GetIO().MousePos.y + camera.y
            );
        }
        ImGui::End();

        ImGui:: Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    }
};

#endif