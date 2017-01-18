#gmock

gmock是在开发过程中测试接口的调用情况。比如调用多少次。

##时间
你的功能完成了，其他人的接口还没实现好。

##学习资料
[gitbud](https://github.com/google/googletest/tree/master/googlemock)

##使用方法：
	1. Include gmock/gmock.h
	2. 创建一个集成接口的类，所以的接口都是virtual的
	3. 使用宏实现这些接口
	4. 测试，具体语法看
[github CheatSheet](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md)

##测试代码
下面这段代码是抄的拉，具体看docs...

    #include "path/to/mock-turtle.h"
	#include "gmock/gmock.h"
	#include "gtest/gtest.h"
	using ::testing::AtLeast;                     // #1
	
	TEST(PainterTest, CanDrawSomething) {
	  MockTurtle turtle;                          // #2
	  EXPECT_CALL(turtle, PenDown())              // #3
	      .Times(AtLeast(1));
	
	  Painter painter(&turtle);                   // #4
	
	  EXPECT_TRUE(painter.DrawCircle(0, 0, 10));
	}                                             // #5
	
	int main(int argc, char** argv) {
	  // The following line must be executed to initialize Google Mock
	  // (and Google Test) before running the tests.
	  ::testing::InitGoogleMock(&argc, argv);
	  return RUN_ALL_TESTS();
	}

