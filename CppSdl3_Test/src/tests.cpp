#include <gtest/gtest.h>

#include <sdl/sdlexception.h>
#include <sdl/window.h>

#include <imgui.h>

namespace {

	class ImmediateQuitWindow final : public sdl::Window {
	private:
		void preLoop() override {
			quit();
		}
	};

}

class Test : public ::testing::Test {
protected:

	Test() {}

	~Test() override {}

	void SetUp() override {}

	void TearDown() override {}
};

TEST_F(Test, someTest) {
	// Given.
	

	// Then.
	EXPECT_TRUE(true);
	EXPECT_EQ(0, 0);
}

TEST(WindowLifecycleTest, destroysImGuiContextBetweenSequentialWindows) {
	EXPECT_EQ(ImGui::GetCurrentContext(), nullptr);
	for (int iteration = 0; iteration < 2; ++iteration) {
		try {
			{
				ImmediateQuitWindow window;
				window.startLoop();
				EXPECT_NE(ImGui::GetCurrentContext(), nullptr);
			}
		} catch (const sdl::SdlException& exception) {
			GTEST_SKIP() << "SDL GPU window unavailable: " << exception.what();
		}
		EXPECT_EQ(ImGui::GetCurrentContext(), nullptr);
	}
}
