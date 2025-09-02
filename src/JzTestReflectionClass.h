#pragma once

#include "JzObject.h"
#include "JzReflectable.h"
#include "CommonTypes.h"

// 标记此文件包含反射代码
JZREFLECTION_FILE()

namespace JzRE {

/**
 * @brief 测试反射功能的示例类
 */
JZCLASS(meta=(DisplayName="Test Reflection Class", Category="Testing"))
class JzTestReflectionClass : public JzObject {
public:
    /**
     * @brief 构造函数
     */
    JzTestReflectionClass();

    /**
     * @brief 析构函数
     */
    virtual ~JzTestReflectionClass() = default;

    // 反射属性
    JZPROPERTY(meta=(DisplayName="Health Points", Category="Stats", Tooltip="Character's health"))
    I32 health = 100;

    JZPROPERTY(meta=(DisplayName="Player Name", Category="Identity", Tooltip="The player's display name"))
    String playerName = "DefaultPlayer";

    JZPROPERTY(meta=(DisplayName="Movement Speed", Category="Stats", Tooltip="How fast the character moves"))
    F32 movementSpeed = 5.0f;

    JZPROPERTY(meta=(DisplayName="Is Active", Category="State", Tooltip="Whether the character is active"))
    Bool isActive = true;

    JZPROPERTY(meta=(DisplayName="Experience Points", Category="Stats", Tooltip="Total experience gained"))
    U64 experiencePoints = 0;

    // 反射方法
    JZMETHOD(meta=(DisplayName="Take Damage", Category="Combat"))
    void TakeDamage(I32 damage);

    JZMETHOD(meta=(DisplayName="Heal", Category="Combat")) 
    void Heal(I32 amount);

    JZMETHOD(meta=(DisplayName="Get Info", Category="Utility"))
    String GetInfo() const;

    JZMETHOD(meta=(DisplayName="Set Speed", Category="Movement"))
    void SetMovementSpeed(F32 speed);

private:
    // 私有成员不会被反射
    I32 m_internalCounter = 0;
};

} // namespace JzRE
