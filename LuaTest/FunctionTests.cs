namespace LuaTest;

using Lua;

using System.Diagnostics.CodeAnalysis;
using System.Reflection;

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
            Assert.That(state.DoString("test()"), Is.True);

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
        Assert.That(state.DoFile("Sample\\funky.lua"), Is.True);

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
        Assert.That(state.DoFile("Sample\\funky.lua"), Is.True);

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
        Assert.That(state.DoFile("Sample\\gcd.lua"), Is.True);

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

}
