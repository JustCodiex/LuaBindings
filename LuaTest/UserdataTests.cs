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

}
