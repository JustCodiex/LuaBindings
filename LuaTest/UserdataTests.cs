using Lua;

namespace LuaTest;

public class UserdataTests {

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

    class SimpleUserdata {
        public string StringValue { get; set; } = "";
    }

    [Test]
    public void CanPushUserdata() {

        // Create userdata
        SimpleUserdata data = new SimpleUserdata() { StringValue = "Hello" };

        // Push userdata
        state.PushLightUserdata(data);
        state.SetGlobal("data");

        // Push function to test
        state.PushCSharpFunction("Test", L => {

            // Get data
            SimpleUserdata d = L.GetUserdata<SimpleUserdata>(-1);

            // Assert
            Assert.Multiple(() => {
                Assert.That(d, Is.Not.Null);
                Assert.That(d.StringValue, Is.EqualTo("Hello"));
            });

            // Return nothing
            return 0;

        });

        // Call it
        Assert.That(state.DoString("Test(data)"), Is.True);

    }

    class ComplexUserdata {
        public bool SomethingCalled { get; set; }
        [LuaField] public string? ValA { get; set; }
        [LuaFunction] public double SomeFunc(double k) { this.SomethingCalled = true; return k + 1; }
        [LuaFunction] public bool SomeOtherFunc(bool i) { this.SomethingCalled = true; return !i; }
    }

    [Test]
    public void CanPushComplexUserdata() {

        // Create the complex userdata instance through Lua
        ComplexUserdata cu = state.NewUserdata<ComplexUserdata>();

        // Make multiple assertions
        Assert.Multiple(() => {

            // Assert basics
            Assert.That(cu, Is.Not.Null);
            Assert.That(state.Top, Is.EqualTo(1));

        });

        // Name it
        state.SetGlobal("c");

        // Assert we get an exception
        Assert.That(state.DoString("return c:SomeFunc(5.0);"), Is.False);

    }

    [Test]
    public void CanPushComplexUserdataAndInvokeMethod() {

        // Create new complex userdata
        ComplexUserdata cu = LuaUserdata.NewUserdata<ComplexUserdata>(state);

        // Make multiple assertions
        Assert.Multiple(() => {

            // Assert basics
            Assert.That(cu, Is.Not.Null);
            Assert.That(state.Top, Is.EqualTo(1));

        });

        // Name it
        state.SetGlobal("c");

        // Invoke do string
        bool v = state.DoString("return c:SomeFunc(5.0);");

        // Make assertions
        Assert.Multiple(() => {
            Assert.That(v, Is.True); // Assert we dont get an exception
            Assert.That(state.Top, Is.EqualTo(1)); // Assert there is a top value
            Assert.That(cu.SomethingCalled, Is.True); // Assert was called
            Assert.That(state.GetNumber(), Is.EqualTo(6.0)); // Assert value of stack top
        });

    }

    [Test]
    public void CanPushComplexUserdataAndGetValue() {

        // Create new complex userdata and name it
        ComplexUserdata cu = LuaUserdata.NewUserdata<ComplexUserdata>(state);
        cu.ValA = "hello";

        state.SetGlobal("c");

        // Update value
        bool isUpdated = state.DoString("return c.ValA");

        // Assert
        Assert.Multiple(() => {
            Assert.That(isUpdated, Is.True); // Assert true
            Assert.That(state.GetString(), Is.EqualTo(cu.ValA)); // Assert was updated on our instance
        });

    }

    [Test]
    public void CanPushComplexUserdataAndSetValue() {

        // Create new complex userdata and name it
        ComplexUserdata cu = LuaUserdata.NewUserdata<ComplexUserdata>(state);
        state.SetGlobal("c");

        // Update value
        bool isUpdated = state.DoString("c.ValA = \"Set by Lua\"");

        // Assert
        Assert.Multiple(() => {
            Assert.That(isUpdated, Is.True); // Assert true
            Assert.That(cu.ValA, Is.EqualTo("Set by Lua")); // Assert was updated on our instance
        });

    }

    [Test]
    public void CanNotAccessSomethingCalled() {

        // Create new complex userdata and name it
        ComplexUserdata cu = LuaUserdata.NewUserdata<ComplexUserdata>(state);
        state.SetGlobal("c");

        // Update value
        bool isUpdated = state.DoString($"return c.{nameof(ComplexUserdata.SomethingCalled)}");

        // Assert
        Assert.Multiple(() => {
            Assert.That(isUpdated, Is.True); // Assert true
            Assert.That(state.Type(), Is.EqualTo(LuaType.Nil)); // Assert we get nil return value
        });

    }

    [Test]
    public void CanNotAccessSomethingCalledWithError() {

        // Set error flag
        LuaUserdata.ErrorOnIndexNotFound = true;

        // Create new complex userdata and name it
        ComplexUserdata cu = LuaUserdata.NewUserdata<ComplexUserdata>(state);
        state.SetGlobal("c");

        // Update value
        bool isUpdated = state.DoString($"return c.{nameof(ComplexUserdata.SomethingCalled)}");

        // Assert
        Assert.Multiple(() => {
            Assert.That(isUpdated, Is.False); // Assert true
            Assert.That(state.GetString(), Is.EqualTo($"attempt to index {nameof(ComplexUserdata.SomethingCalled)} on a userdata value")); // Assert we get correct error string
        });

    }

    public class Vector {
        [LuaField(Name = "x")] public double X { get; set; }
        [LuaField(Name = "y")] public double Y { get; set; }
        [LuaField(Name = "z")] public double Z { get; set; }
        [LuaMetamethod(LuaMetamethod.Addition)] public Vector Add(Vector other) => new Vector() { X = this.X + other.X, Y = this.Y + other.Y, Z = this.Z + other.Z };
    }

    [Test]
    public void CanExecuteVectorScript() {

        // Register userdata type
        LuaUserdata.RegisterType<Vector>(state);

        // Define vector3 function
        state.PushCSharpFunction("vector3", L => {
            var (x, y, z) = (L.GetNumber(1), L.GetNumber(2), L.GetNumber(3)); // Grab vals
            var v = LuaUserdata.NewUserdata<Vector>(L); // Create
            v.X = x;
            v.Y = y;
            v.Z = z;            
            return 1; // Retur v (one value)
        });

        // Do vector file
        var result = state.DoFile("Sample\\vec.lua");

        // Assert success
        Assert.That(result, Is.True);

    }

}
