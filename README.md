# Lua Bindings
Bring the powerful scripting language Lua into your C# applications!

With Lua bindings it's possible to host the Lua runtime in a C# application and interact with Lua the same way C/C++ would interact with the Lua runtime. The Lua Bindings project brings additional functionalities to make working with the Lua Bindings feel more C#-ish.

All use of this project falls under the [MIT license](LICENSE.txt) and can be freely used. Credits for the Lua C implementation code included within this project go the original authors of Lua. The [Lua license](LUA_LICENSE.txt) is also MIT.

The most recent version of Lua Bindings use Lua v5.4.4 as the runtime.

## Getting Started
Using Lua Bindings is easy and it works almost exactly like the C API for Lua. A basic hello world program using Lua Bindings would be:
```csharp
using Lua;

// Create a LuaState object with the complete Lua STL
LuaState L = LuaState.NewState();

// Print Hello World to the console using Lua
L.DoString("print \"Hello World\"");
```
Lua Bindings also offer a seamless way of interacting with the Lua runtime, while also maintaining the familiarities of the C API. Meaning Lua Bindings offer the same stack manipulation capabilities:
```csharp
// Push a number value 'x'
L.PushNumber(2.0);
L.SetGlobal("x"); // Pops 2 from the stack

// Push a second number value 'y'
L.PushNumber(5.0);
L.SetGlobal("y"); // Pops 5 from the stack

// Compute x * y in Lua and get the numeric value in C#
double z = L.DoString<double>("return x * y"); // 10.0
```
Lua Bindings also offer an easy way to define custom Lua functions that can be called by the Lua runtime.
```csharp
// Push a C# function into the Lua runtime
L.PushCSharpFunction(L => {

    // Load arguments
    double x = L.GetNumber(-2); // Get number at stack index -2
    double y = L.GetNumber(-1); // Get number at stack index -1 (Top stack value)

    // Push number result onto the stack
    L.PushNumber(x - y);

    // Return how many values we return (In this case one)
    return 1;

});

// Give our function a global name
L.SetGlobal("Sub");

// Call our function
if (L.DoString<double>("return Sub(5, 2.5)") == 2.5) {
    Console.WriteLine("Lua says: 5.0 - 2.5 == 2.5");
}
```
## Other Features
Lua Bindings also include other features such as:
* LuaTable value type offering C#-friendly functions for reading and manipulating tables stored on the stack.
* LuaFunction value type offering C#-friendly functions for invoking a Lua function stored on the stack.
* An easy tuple-based solution for handling Lua functions that return multiple values.
* Support for C# classes/structs as Lua Userdata - with extensions to make C# objects behave as Lua objects.