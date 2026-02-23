/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include <gtest/gtest.h>

#include "JzRE/Runtime/Platform/RHI/JzRHIPipeline.h"

namespace {

class JzTestPipeline : public JzRE::JzRHIPipeline {
public:
    explicit JzTestPipeline(const JzRE::JzPipelineDesc &desc) :
        JzRHIPipeline(desc)
    { }

    void CommitParameters() override
    {
        ++m_commitCount;
        MarkParametersCommitted();
    }

    JzRE::Size GetCommitCount() const
    {
        return m_commitCount;
    }

private:
    JzRE::Size m_commitCount = 0;
};

} // namespace

TEST(JzShaderParameterCache, DirtyFlagLifecycle)
{
    JzRE::JzPipelineDesc desc;
    desc.debugName = "UnitTestPipeline";

    JzTestPipeline pipeline(desc);

    EXPECT_FALSE(pipeline.HasDirtyParameters());

    pipeline.SetUniform("uInt", static_cast<JzRE::I32>(7));
    EXPECT_TRUE(pipeline.HasDirtyParameters());

    pipeline.CommitParameters();
    EXPECT_FALSE(pipeline.HasDirtyParameters());
    EXPECT_EQ(pipeline.GetCommitCount(), 1U);
}

TEST(JzShaderParameterCache, SupportsAllUniformTypes)
{
    JzRE::JzPipelineDesc desc;
    desc.debugName = "UnitTestPipeline";

    JzTestPipeline pipeline(desc);

    pipeline.SetUniform("i", static_cast<JzRE::I32>(3));
    pipeline.SetUniform("f", 1.5f);
    pipeline.SetUniform("v2", JzRE::JzVec2(1.0f, 2.0f));
    pipeline.SetUniform("v3", JzRE::JzVec3(1.0f, 2.0f, 3.0f));
    pipeline.SetUniform("v4", JzRE::JzVec4(1.0f, 2.0f, 3.0f, 4.0f));
    pipeline.SetUniform("m3", JzRE::JzMat3x3::Identity());
    pipeline.SetUniform("m4", JzRE::JzMat4x4::Identity());

    EXPECT_EQ(pipeline.GetParameterCache().size(), 7U);
    EXPECT_TRUE(pipeline.HasDirtyParameters());
}
