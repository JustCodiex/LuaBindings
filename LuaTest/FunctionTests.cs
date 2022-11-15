namespace LuaTest;

using Lua;

public class FunctionTests {

    [NotNull]
    LuaState state;

    [SetUp]
    public void CreateState() {
        this.state = LuaState.NewState();
    }

    [TearDown]
    public void CleanupState() {
        this.state.Dispose();
    }

    [Test]
    public void CanInvokeDelegate() {

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
            Assert.That(state.DoString("test()"), Is.EqualTo(CallResult.Ok));

            // Verify
            Assert.That(wasInvoked, Is.True);

        });

    }

    [Test]
    public void CanInvokeDelegateWithReturns() {

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

    [Test]
    public void CanInvokeAddFunction() {

        // Do a file with a function
        Assert.That(state.DoFile("Sample\\funky.lua"), Is.EqualTo(CallResult.Ok));

        // Get global
        LuaFunction add = state.GetGlobal<LuaFunction>("add");

        // Invoke it
        double result = add.Invoke<double>(1.0, 2.0);

        // Assert result
        Assert.That(result, Is.EqualTo(3));

    }

    [Test]
    public void CanInvokeSumFunction() {

        // Do a file with a function
        Assert.That(state.DoFile("Sample\\funky.lua"), Is.EqualTo(CallResult.Ok));

        // Get global
        LuaFunction sum = state.GetGlobal<LuaFunction>("sum");

        // Invoke it
        double result = sum.Invoke<double>(1.0, 2.0, 3.5, 9.0);

        // Assert result
        Assert.That(result, Is.EqualTo(15.5));

    }

    [Test]
    public void CanInvokeFunctionWithTupleReturn() {

        // Do a file with a function
        Assert.That(state.DoFile("Sample\\gcd.lua"), Is.EqualTo(CallResult.Ok));

        // Get global
        LuaFunction gcd = state.GetGlobal<LuaFunction>("gcd");

        // Invoke it
        (double v, double x, double y) = gcd.Invoke<(double, double, double)>(5.0, 2.0);

        // Assert
        Assert.Multiple(() => {

            Assert.That(v, Is.EqualTo(1));
            Assert.That(x, Is.EqualTo(2));
            Assert.That(y, Is.EqualTo(-2.5));
        });

    }

    [Test]
    public void CanInvokeCallbacksInCSharpFunction() {

        // Import the math functions
        Assert.That(state.DoFile("Sample\\funky.lua"), Is.EqualTo(CallResult.Ok));

        // Define caller
        state.PushCSharpFunction("caller", L => {

            // Bring mul up (since sum = -1, mul = -2);
            L.Insert(-2);

            // Grab
            double mulResult = LuaFunction.FromTop(L).Invoke<double>(5.0, 7.0);

            // Assert
            Assert.Multiple(() => {
                Assert.That(mulResult, Is.EqualTo(35.0));
                Assert.That(L.Top, Is.EqualTo(1));
            });

            // Invoke it and Push it back (TODO: Leave it as an option to keep the result(s) on the stack)
            L.PushNumber(LuaFunction.FromTop(L).Invoke<double>(mulResult, 5.0, 1.0, 0.5, 0.25, 0.125));

            // Return top
            return 1;

        });

        // Invoke it
        double d = state.DoString<double>("return caller(mul, sum)");

        // Assert correct result
        Assert.That(d, Is.EqualTo(41.875));

    }

}
