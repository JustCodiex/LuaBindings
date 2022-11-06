namespace LuaTest;

using Lua;

public class TableTest {

    [Test]
    public void CanCreateTableWithFields() {

        // Create state
        using var state = LuaState.NewState();

        // Assert no exceptions occur
        Assert.DoesNotThrow(() => { 

            // Create new table
            var table = state.CreateTable(0, 4);

            // Set fields
            table.SetField("a", 1);
            table.SetField("b", 2);
            table.SetField("c", 3);
            table.SetField("d", 4);

        });

    }

    [Test]
    public void CanGetArrayFromTop() {

        // Create state
        using var state = LuaState.NewState();

        // Do string
        Assert.That(state.DoString("return { 1, 2, 3, 4 }"), Is.True);

        // Get as table
        LuaTable? table = null;
        Assert.DoesNotThrow(() => {
            table = LuaTable.FromTop(state);
        });

        // Assert top has count of 4 elements
        Assert.That(table!.Count, Is.EqualTo(4));

    }

    [Test]
    public void CanIterateOverTop() {

        // Create state
        using var state = LuaState.NewState();

        // Do string
        Assert.That(state.DoString("return { first = \"Hello\", second = \"World\" }"), Is.True);

        // Capture it
        LuaTable table = LuaTable.FromTop(state);

        // Dic
        Dictionary<string, string> vals = new();

        // Iterate
        foreach (LuaTable.KeyValue val in table) {
            vals.Add(val.Key.ToString()!, val.Value.ToString()!);
        }

        // Assert entries exist
        Assert.Multiple(() => {

            // Assert first and second are in the dictionary
            Assert.That(vals.ContainsKey("first"), Is.True);
            Assert.That(vals.ContainsKey("second"), Is.True);

            // Assert values
            Assert.That(vals["first"], Is.EqualTo("Hello"));
            Assert.That(vals["second"], Is.EqualTo("World"));

        });

    }

    [Test]
    public void CanSetField() {

        // Create state
        using var state = LuaState.NewState();

        // Do string
        Assert.That(state.DoString("return { first = \"Hello\", second = \"World\" }"), Is.True);

        // Capture it
        LuaTable table = LuaTable.FromTop(state);

        // Set table field
        table.SetField("first", "Goodbye");

        // Assert we can retrieve both fields
        Assert.Multiple(() => {
            Assert.That(table.GetField<string>("first", true), Is.EqualTo("Goodbye"));
            Assert.That(table.GetField<string>("second", true), Is.EqualTo("World"));
        });

    }

    [Test]
    public void CanPushHashtable() {

        // Create state
        using var state = LuaState.NewState();

        // Create table
        Hashtable table = new() {
            { "A", 1.0 },
            { "B", 2.0 },
            { "C", 3.0 },
            { "D", 4.0 },
            { "E", 5.0 }
        };

        // Push it
        state.PushTable(table);
        state.SetGlobal("g_test");

        // Assert values
        Assert.Multiple(() => {
            Assert.That(state.DoString<double>("return g_test.A"), Is.EqualTo(1.0));
            Assert.That(state.DoString<double>("return g_test.B"), Is.EqualTo(2.0));
            Assert.That(state.DoString<double>("return g_test.C"), Is.EqualTo(3.0));
            Assert.That(state.DoString<double>("return g_test.D"), Is.EqualTo(4.0));
            Assert.That(state.DoString<double>("return g_test.E"), Is.EqualTo(5.0));
        });

    }

    [Test]
    public void CanPushNestedHashTable() {

        // Create state
        using var state = LuaState.NewState();

        // Create table
        Hashtable table = new() {
            { 
                "A", new Hashtable() {
                    { "1", 1.0 },
                    { "2", 2.0 }
                } 
            },
            { 
                "B", new Hashtable() {
                    { "1", 1.0 },
                    { "2", 2.0 }
                }
            },
        };

        // Push it
        state.PushTable(table);
        state.SetGlobal("g_test");

        // Assert values
        Assert.Multiple(() => {
            Assert.That(state.DoString<double>("return g_test.A[\"1\"]"), Is.EqualTo(1.0));
            Assert.That(state.DoString<double>("return g_test.A[\"2\"]"), Is.EqualTo(2.0));
            Assert.That(state.DoString<double>("return g_test.B[\"1\"]"), Is.EqualTo(1.0));
            Assert.That(state.DoString<double>("return g_test.B[\"2\"]"), Is.EqualTo(2.0));
        });

    }

    [Test]
    public void CanGetTable() {

        // Create state
        using var state = LuaState.NewState();

        // Do string
        Assert.That(state.DoString("return { first = \"Hello\", second = \"World\" }"), Is.True);

        // Capture it
        Hashtable table = LuaTable.FromTop(state).ToHashtable();

        // Asserts
        Assert.Multiple(() => {
            Assert.That(table["first"], Is.EqualTo("Hello"));
            Assert.That(table["second"], Is.EqualTo("World"));
        });

    }

    [Test]
    public void ThrowsIfNotTableOnTop() {

        // Create state
        using var state = LuaState.NewState();

        // Push a string
        state.PushString("Not a table");

        // Get
        var ex = Assert.Throws<LuaTypeExpectedException>(() => {
            var _ = LuaTable.FromTop(state);
        });

        // Assert content
        Assert.Multiple(() => {
            Assert.That(ex.Expected, Is.EqualTo(LuaType.Table));
            Assert.That(ex.Found, Is.EqualTo(LuaType.String));
            Assert.That(ex.Message, Is.EqualTo($"Lua runtime found {LuaType.String} when expecteing a {LuaType.Table}"));
        });

    }

}
