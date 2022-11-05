namespace LuaTest;

using Lua;

public class FunctionTests {

    [Test]
    public void CanInvokeDelegate() {

        // Create new state
        using var state = LuaState.NewState();

        // Flag
        bool wasInvoked = false;

        // Define our function
        int test(LuaState _) {
            wasInvoked = true;
            return 0;
        }

        // Push
        state.PushCSharpFunction(test);

        // Name it
        state.SetGlobal("test");

        // Assert remaining
        Assert.Multiple(() => {

            // Invoke it
            Assert.That(state.DoString("test()"), Is.True);

            // Verify
            Assert.That(wasInvoked, Is.True);

        });

    }

    [Test]
    public void CanInvokeDelegateWithReturns() {

        // Create new state
        using var state = LuaState.NewState();

        // Define our function
        int test(LuaState L) {
            
            // Push string
            L.PushString("Test");
            
            // Return amount of arguments returned
            return 1;

        }

        // Push
        state.PushCSharpFunction(test);

        // Name it
        state.SetGlobal("test");

        // Invoke it
        Assert.That(state.DoString<string>("return test()"), Is.EqualTo("Test"));

    }

    [Test]
    public void CanInvokeDelegateWithReturnsAndArgs() {

        // Create new state
        using var state = LuaState.NewState();

        // Push
        state.PushCSharpFunction("test", L => {

            // Read args
            var arg2 = L.GetNumber(-1);
            var arg1 = L.GetNumber(-2);

            // Push
            L.PushNumber(arg1 - arg2);

            // Return amount of arguments returned
            return 1;

        });

        // Invoke it
        Assert.That(state.DoString<double>("return test(10, 5)"), Is.EqualTo(5));

    }

}
