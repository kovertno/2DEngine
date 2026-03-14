#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <deque>
#include <iostream>

#include "../Logger/Logger.h"

const unsigned int MAX_COMPONENTS = 32;

//Signature
// We ue bitset (1s and 0s) to keep track of which components an entity has,
// also helps keet track of which entities a system is interested in.
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent{
    protected:
        static int nextId;
};

// Used to assign a unique id to a component type.
template <typename TComponent>
class Component: public IComponent{
    // Return the unique id of Component<T>
    public:
        Component() = default;
        ~Component() = default;

        static int GetId(){
            static auto id = nextId++;
            return id;
        }
};

class Entity{
    private:
        int id;

    public:
        Entity(int id): id(id) {};
        Entity(const Entity& other) = default;
        void Kill();
        int GetId() const;

        // Manage entity tags and groups
        void Tag(const std::string& tag);
        bool HasTag(const std::string& tag) const;
        void Group(const std::string& group);
        bool BelongsToGroup(const std::string& group) const;

        // Operator overloading for entity objects
        Entity& operator =(const Entity& other) = default;
        bool operator ==(const Entity& other) const{ return id == other.id; }
        bool operator !=(const Entity& other) const{ return id != other.id; }
        bool operator >(const Entity& other) const{ return id > other.id; }
        bool operator <(const Entity& other) const{ return id < other.id; }

        // Manage entity components
        template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
        template <typename TComponent> void RemoveComponent();
        template <typename TComponent> bool HasComponent() const;
        template <typename TComponent> TComponent& GetComponent() const;

        // Hold a pointer to the entity's owner registry.
        class Registry* registry;
};

//System
// The system processes entities that contain a specific signature.

class System{
    private:
        Signature componentSignature;
        std::vector<Entity> entities;

    public:
        System() = default;
        ~System() = default;

        void AddEntityToSystem(Entity entity);
        void RemoveEntityFromSystem(Entity entity);
        std::vector<Entity> GetSystemEntities() const;
        const Signature& GetComponentSignature() const;


        // Defines the component type that the entities must have to be considered by the system.
        template <typename TComponent> void RequireComponent();
};

//Pool
// A pool is just a vector (contiguous data) of objects of type T (we want the data as contiguous as possible for fast access).
class IPool {
    public:
        virtual ~IPool() = default;
        virtual void RemoveEntityFromPool(int entityId) = 0;
};

template <typename T>
class Pool: public IPool {
    private :
        // We keep track of the vector of objects and current number of elements
        std::vector<T> data;
        int size;

        // Helper maps to keep track of entity ids per index, so the vector is always packed
        std::unordered_map<int, int> entityIdToIndex;
        std::unordered_map<int, int> indexToEntityId;

    public:
        Pool(int capacity = 100) {
            size = 0;
            data.resize(capacity);
        }

        virtual ~Pool() = default;

        bool IsEmpty() const {
            return size == 0;
        }

        int GetSize() const {
            return size;
        }

        void Resize(int n) {
            data.resize(n);
        }

        void Clear() {
            data.clear();
            size = 0;
        }

        void Add(T object) {
            data.push_back(object);
        }

        void Set(int entityId, T object) {
            if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
                // If the element already exists, simply replace the component object
                int index = entityIdToIndex[entityId];
                data[index] = object;
            }
            else {
                // When adding a new object, we keep track of the entity ids and their vector index
                int index = size;
                entityIdToIndex.emplace(entityId, index);
                indexToEntityId.emplace(index, entityId);
                if (index >= data.capacity()) {
                    // If necessary, we resize by always doubling the current capacity
                    data.resize(size * 2);
                }
                data[index] = object;
                size++;
            }
        }

        void Remove(int entityId) {
            // Copy the last element to the deleted position to keep the array packed
            int indexOfRemoved = entityIdToIndex[entityId];
            int indexOfLast = size - 1;
            data[indexOfRemoved] = data[indexOfLast];

            // Update the index-entity maps to point to the correct elements
            int entityIdOfLastElement = indexToEntityId[indexOfLast];
            entityIdToIndex[entityIdOfLastElement] = indexOfRemoved;
            indexToEntityId[indexOfRemoved] = entityIdOfLastElement;

            entityIdToIndex.erase(entityId);
            indexToEntityId.erase(indexOfLast);

            size--;
        }

        void RemoveEntityFromPool(int entityId) override {
            if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
                Remove(entityId);
            }
        }

        T& Get(int entityId) {
            int index = entityIdToIndex[entityId];
            return static_cast<T&>(data[index]);
        }

        T& operator [](unsigned int index) {
            return data[index];
        }
};


//Registry
// The registry manages the creation and destruction of entities, add systems and components.
class Registry {
    private:
        private:
            int numEntities = 0;

            // Vector of component pools, each pool contains all the data for a certain component type.
            // [Vector index = component type id]
            // [Pool index = entity id]
            std::vector<std::shared_ptr<IPool>> componentPools;

            // Vector of component signatures per entity, saying which component is turned "on" for a given entity.
            // [Vector index = entity id]
            std::vector<Signature> entityComponentSignatures;

            std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

            // Set of entities to be flagged to be added or removed in the next registry Update() .
            std::set<Entity> entitiesToBeAdded;
            std::set<Entity> entitiesToBeKilled;

            // Entity tags(one tag name per entity)
            std::unordered_map<std::string, Entity> entityPerTag;
            std::unordered_map<int, std::string> tagPerEntity;

            // Entity groups(a set of entities per group name
            std::unordered_map<std::string, std::set<Entity>> entitiesPerGroup;
            std::unordered_map<int, std::string> groupPerEntity;

            // List of free(available) ids that were previously removed.
            std::deque<int> freeIds;

    public:
        Registry() = default;
        ~Registry() = default;

        void Update();

        // Entity management
        Entity CreateEntity();
        void KillEntity(Entity entity);

        // Tag management
        void TagEntity(Entity entity, const std::string& tag);
        bool EntityHasTag(Entity entity, const std::string& tag) const;
        Entity GetEntityByTag(const std::string& tag) const;
        void RemoveEntityTag(Entity entity);

        // Group management
        void GroupEntity(Entity entity, const std::string& group);
        bool EntityBelongsToGroup(Entity entity, const std::string& group) const;
        std::vector<Entity> GetEntitiesByGroup(const std::string& group) const;
        void RemoveEntityGroup(Entity entity);

        // Component management
        template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
        template <typename TComponent> void RemoveComponent(Entity entity);
        template <typename TComponent> bool HasComponent(Entity entity) const;
        template <typename TComponent> TComponent& GetComponent(Entity entity) const;

        // System management
        template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
        template <typename TSystem> void RemoveSystem();
        template <typename TSystem> bool HasSystem() const;
        template <typename TSystem> TSystem& GetSystem() const;

        // Add and remove entities from their systems
        void AddEntityToSystems(Entity entity);
        void RemoveEntityFromSystems(Entity entity);
};

template <typename TComponent>
void System::RequireComponent(){
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
}

template <typename TComponent, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args) {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    if(componentId >= componentPools.size()) {
        componentPools.resize(componentId + 1, nullptr);
    }

    if(!componentPools[componentId]) {
        std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
        componentPools[componentId] = newComponentPool;
    }

    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    if (entityId >= componentPool->GetSize()) {
        componentPool -> Resize(numEntities);
    }

    TComponent newComponent(std::forward<TArgs>(args)...);

    componentPool->Set(entityId, newComponent);
    entityComponentSignatures[entityId].set(componentId);

    Logger::Log("Component id: " + std::to_string(componentId) + " was ADDED to entity id: " + std::to_string(entityId));
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity) {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    // Remove the component from the component list for that entity
    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    componentPool->Remove(entityId);

    // Set this component signature for that entity to false
    entityComponentSignatures[entityId].set(componentId, false);

    Logger::Log("Component id: " + std::to_string(componentId) + " was REMOVED from entity id: " + std::to_string(entityId));
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent>
TComponent& Registry::GetComponent(Entity entity) const {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();
    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    return componentPool->Get(entityId);
}

template <typename TSystem, typename ...TArgs>
void Registry::AddSystem(TArgs&& ...args) {
    std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);

    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem>
void Registry::RemoveSystem() {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(system);
}

template <typename TSystem>
bool Registry::HasSystem() const {
    return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem& Registry::GetSystem() const {
    auto system = systems.find(std::type_index(typeid(TSystem)));

    // system.first for key, system.second for value (-> because cast to a pointer??)
    return *(std::static_pointer_cast<TSystem>(system->second));
}

template <typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args) {
    registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::RemoveComponent() {
    registry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent>
bool Entity::HasComponent() const {
    return registry->HasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::GetComponent() const {
    return registry->GetComponent<TComponent>(*this);
}

#endif