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
    public void CanPushComplexUserdataWithAttributes() {

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

}
