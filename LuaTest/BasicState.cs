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
        var result = state.DoString("print \"Hello World\"");

        // Assert success
        Assert.That(result, Is.EqualTo(CallResult.Ok));

    }

    [Test]
    public void CanDoFile() {

        // Create state
        using var state = LuaState.NewState(LuaLib.Base);

        // Do hello file
        var result = state.DoFile("Sample\\hello.lua");

        // Assert success
        Assert.That(result, Is.EqualTo(CallResult.Ok));

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
            Assert.That(state.DoString("v = 5.0"), Is.EqualTo(CallResult.Ok));

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
            Assert.That(state.DoString("s = \"Hello\""), Is.EqualTo(CallResult.Ok));

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
            Assert.That(state.DoString("s = \"Hello World\""), Is.EqualTo(CallResult.Ok));

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

    [Test]
    public void CanLoadStringAndCallIt() {

        // Create state
        using var state = LuaState.NewState();

        // Do load
        state.LoadString("return 5 + 5");

        // Call it
        state.Call(0, LuaState.MultiReturn);

        // Retrieve it
        Assert.That(state.GetNumber(), Is.EqualTo(10.0));

    }

    [Test]
    public void CanLoadFromStream() {

        // Create state
        using var state = LuaState.NewState();

        // Load
        using var fs = File.Open("Sample\\hello.lua", FileMode.Open);

        // Load
        var loaded = state.LoadStream(fs);

        // Assert success
        Assert.That(loaded, Is.EqualTo(CallResult.Ok));

    }

    [Test]
    public void CanLoadNamedFromStream() {

        // Create state
        using var state = LuaState.NewState();

        // Load
        using var fs = File.Open("Sample\\error.lua", FileMode.Open);

        // Load
        var loaded = state.LoadStream(fs, "test");

        // Assert success
        Assert.That(loaded, Is.EqualTo(CallResult.Ok));

        // Call it
        var ex = Assert.Throws<LuaRuntimeException>(() => state.Call(0, LuaState.MultiReturn));

        // Assert error contents
        Assert.That(ex.Message, Is.EqualTo("[string \"test\"]:1: this is an error"));

    }

    [Test]
    public void CanTransferFunction() {

        // Create states
        using var state1 = LuaState.NewState();
        using var state2 = LuaState.NewState();

        // Make state1 do file
        Assert.That(state1.DoFile("Sample\\funky.lua"), Is.EqualTo(CallResult.Ok));

        // Get func
        state1.GetGlobal("add");

        // Dump it
        var dumpstate = state1.Dump(out byte[] luacode);
        Assert.Multiple(() => {
            Assert.That(dumpstate, Is.EqualTo(CallResult.Ok));
            Assert.That(luacode, Is.Not.Null);
            Assert.That(luacode, Is.Not.Empty);
        });
        
        // Now push it into state 2
        var loaded = state2.LoadStream(new MemoryStream(luacode), "add");
        Assert.That(loaded, Is.EqualTo(CallResult.Ok));

        // Give it a name
        state2.SetGlobal("add");

        // Invoke add in state 2
        double v = state2.DoString<double>("return add(1,2)");
        Assert.That(v, Is.EqualTo(3));

    }

    [Test]
    public void CanTransferFunctionUsingLoad() {

        // Create states
        using var state1 = LuaState.NewState();
        using var state2 = LuaState.NewState();

        // Make state1 do file
        Assert.That(state1.DoFile("Sample\\funky.lua"), Is.EqualTo(CallResult.Ok));

        // Get func
        state1.GetGlobal("mul");

        // Dump it
        var dumpstate = state1.Dump(out byte[] luacode);
        Assert.Multiple(() => {
            Assert.That(dumpstate, Is.EqualTo(CallResult.Ok));
            Assert.That(luacode, Is.Not.Null);
            Assert.That(luacode, Is.Not.Empty);
        });

        // Now push it into state 2
        var loaded = state2.Load(luacode, "mul");
        Assert.That(loaded, Is.EqualTo(CallResult.Ok));

        // Give it a name
        state2.SetGlobal("mul");

        // Invoke mul in state 2
        double v = state2.DoString<double>("return mul(2,2)");
        Assert.That(v, Is.EqualTo(4));

    }

}
