namespace LuaTest;

using Lua;

public class BasicState {
    
    [Test]
    public void CanCreateState() {

        // Test we can create a new lua state with all libraries.
        Assert.DoesNotThrow(() => {
            LuaState state = LuaState.NewState();
        });

    }

    [Test]
    public void CanCreateStateWithoutLibraries() {

        // Test we can load the lua state without any libraries, without issue.
        Assert.DoesNotThrow(() => {
            using LuaState state = LuaState.NewState(LuaLib.None);
        });

    }

    [Test]
    public void CanDoString() {

        // Create state
        using var state = LuaState.NewState(LuaLib.Base);

        // Do simple string
        bool result = state.DoString("print \"Hello World\"");

        // Assert success
        Assert.That(result, Is.True);

    }

    [Test]
    public void CanDoFile() {

        // Create state
        using var state = LuaState.NewState(LuaLib.Base);

        // Do hello file
        bool result = state.DoFile("Sample\\hello.lua");

        // Assert success
        Assert.That(result, Is.True);

    }

    [Test]
    public void CanGetNumber() {

        // Create state
        using var state = LuaState.NewState(LuaLib.Base);

        // Do simple string
        double result = state.DoString<double>("return 5.0 + 5.0");

        // Check
        Assert.That(result, Is.EqualTo(10));

    }

    [Test]
    public void CanGetGlobalNumber() {

        // Create state
        using var state = LuaState.NewState();
        Assert.Multiple(() => {

            // Do simple string
            Assert.That(state.DoString("v = 5.0"), Is.True);

            // Put 'v' on stack
            Assert.That(state.GetGlobal("v"), Is.EqualTo(LuaType.Number));

        });

        // Extract it
        double v = state.GetNumber();

        // Assert it
        Assert.That(v, Is.EqualTo(5));

    }

    [Test]
    public void CanGetGlobalString() {

        // Create state
        using var state = LuaState.NewState();
        Assert.Multiple(() => {

            // Do simple string
            Assert.That(state.DoString("s = \"Hello\""), Is.True);

            // Put 'v' on stack
            Assert.That(state.GetGlobal("s"), Is.EqualTo(LuaType.String));

        });

        // Extract it
        string s = state.GetString();

        // Assert it
        Assert.That(s, Is.EqualTo("Hello"));

    }

    [Test]
    public void CanGetGlobalStringSimple() {

        // Create state
        using var state = LuaState.NewState();

        // Assert all
        Assert.Multiple(() => {

            // Assert set
            Assert.That(state.DoString("s = \"Hello World\""), Is.True);

            // Assert get
            Assert.That(state.GetGlobal<string>("s"), Is.EqualTo("Hello World"));

        });

    }

    [Test]
    public void CanStoreGlobalValue() {

        // Create state
        using var state = LuaState.NewState();

        // Push string
        state.PushString("Hello World");

        // Store it
        state.SetGlobal("v");

        // Assert get
        Assert.That(state.GetGlobal<string>("v"), Is.EqualTo("Hello World"));

    }

}
