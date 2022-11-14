namespace LuaTest;

using Lua;

public class TableTest {

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
    public void CanCreateTableWithFields() {

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

        // Do string
        Assert.That(state.DoString("return { 1, 2, 3, 4 }"), Is.True);

        Assert.Multiple(() => {
            
            // Get as table
            LuaTable? table = null;

            Assert.DoesNotThrow(() => {
                table = LuaTable.FromTop(state);
            });

            // Grab count
            int count = table!.Value.Count;

            // Assert top has count of 4 elements
            Assert.That(count, Is.EqualTo(4));

        });
    }

    [Test]
    public void CanIterateOverTop() {

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

        // Create table
        Hashtable table = new() {
            { "A", 1.0 },
            { "B", 2.0 },
            { "C", 3.0 },
            { "D", 4.0 },
            { "E", 5.0 }
        };

        // Push it
        state.CreateTable(table);
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
        state.CreateTable(table);
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

    [Test]
    public void CanGetSecondTopStack() {

        // Push table
        state.CreateTable(new Hashtable() {
            { "A", 1.0 },
            { "B", 2.0 }
        });

        // Push string
        state.PushString("Not a table");

        // Get table
        var table = LuaTable.FromStack(state, -2);

        // Make assertions
        Assert.Multiple(() => {

            // Assert field values
            Assert.That(table.GetField<double>("A"), Is.EqualTo(1));
            Assert.That(table.GetField<double>("B"), Is.EqualTo(2));

        });

    }

    [Test]
    public void CanSetMetatable() {

        // Create meta table and assert it's new
        LuaTable meta = state.NewMetatable("MetaTest", out bool exists);
        Assert.That(exists, Is.False);

        // Push metatable member
        meta.SetField(LuaTable.METATABLE_MUL, L => {

            // Grab ref to both
            var lhs = LuaTable.FromStack(L, -2);
            var rhs = LuaTable.FromStack(L, -1);

            // Grab vals
            var (x1, y1, z1) = (lhs.GetField<double>("x"), lhs.GetField<double>("y"), lhs.GetField<double>("z"));
            var (x2, y2, z2) = (rhs.GetField<double>("x"), rhs.GetField<double>("y"), rhs.GetField<double>("z"));

            // Compute dot
            var dot = x1 * x2 + y1 * y2 + z1 * z2;

            // Push
            L.PushNumber(dot);

            // Return one result
            return 1;

        });

        // Pop metatable
        state.Pop();

        // Ensure stack is now empty
        Assert.That(state.Top, Is.EqualTo(0));

        // Create table a
        state.CreateTable(new Hashtable() { { "x", 1.0 }, { "y", 2.0 }, { "z", 3.0 } });
        state.SetMetatable("MetaTest");
        state.SetGlobal("a");

        // Ensure stack is now empty
        Assert.That(state.Top, Is.EqualTo(0));

        // Create table b
        state.CreateTable(new Hashtable() { { "x", 1.0 }, { "y", 2.0 }, { "z", 3.0 } });
        state.SetMetatable("MetaTest");
        state.SetGlobal("b");

        // Ensure stack is now empty
        Assert.That(state.Top, Is.EqualTo(0));

        // Invoke
        double result = state.DoString<double>("return a * b");

        // Assert result
        Assert.That(result, Is.EqualTo(14));

    }

    [Test]
    public void CanPushArray() {

        // Create string array
        string[] strs = new string[4] { "a", "b", "c", "d" };
        bool[] check = new bool[4];

        // Push it
        LuaTable t = state.CreateTable(strs);

        // Iterate it
        foreach (LuaTable.KeyValue kv in t) {
            int i = (int)(double)kv.Key;
            check[i - 1] = strs[i - 1].Equals(kv.Value);
        }

        // Verify all good
        Assert.That(check.All(x => x), Is.True);

    }

    [Test]
    public void CanGetByIndex() {

        // Define array
        string[] strs = new string[4] { "a", "b", "c", "d" };

        // Push it
        LuaTable t = state.CreateTable(strs);

        // Assert it
        Assert.Multiple(() => {

            // Assert the count matches the amount of elements we pushed
            Assert.That(t.Count, Is.EqualTo(4));

            // Assert values match
            for (int i = 0; i < strs.Length; i++)
                Assert.That(t.Get(i + 1), Is.EqualTo(strs[i]));

        });

    }

    [Test]
    public void CanGetByIndexer() {

        // Define array
        string[] strs = new string[4] { "a", "b", "c", "d" };

        // Push it
        LuaTable t = state.CreateTable(strs);

        // Assert it
        Assert.Multiple(() => {

            // Assert values match
            for (int i = 0; i < strs.Length; i++)
                Assert.That(t[i + 1], Is.EqualTo(strs[i]));

        });

    }

    [Test]
    public void CanSetByIndexer() {

        // Create table and populate it
        LuaTable t = state.CreateTable(4, 0);
        t[1] = "a";
        t[2] = "b";
        t[3] = "c";
        t[4] = "d";

        // Convert to hashtable
        var table = t.ToHashtable();

        // Assert
        Assert.Multiple(() => {
            Assert.That(table[1.0], Is.EqualTo("a"));
            Assert.That(table[2.0], Is.EqualTo("b"));
            Assert.That(table[3.0], Is.EqualTo("c"));
            Assert.That(table[4.0], Is.EqualTo("d"));
        });

    }

}
