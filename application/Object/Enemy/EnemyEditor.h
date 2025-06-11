#pragma once
#include <list>
#include <memory>
#include <string>
#include "Enemy.h"
#include "struct.h"

#ifdef _DEBUG
#include "imgui.h"
#endif

class EnemyEditor {
public:
    EnemyEditor(std::list<std::list<std::unique_ptr<Enemy>>>* enemies);

    void DrawEditorUI();

    void AddGroup();
    void RemoveGroup(int index);
    void AddEnemyToGroup(int groupIdx, const Vector3& pos);
    void RemoveEnemyFromGroup(int groupIdx, int enemyIdx);

    void Save(const std::string& filename);
    void Load(const std::string& filename);

private:
    std::list<std::list<std::unique_ptr<Enemy>>>* enemies_ = nullptr;
    int selectedGroup_ = 0;
};
