#pragma once

#include "lua/luabind.hpp"
#include "LuaFunction.hpp"
#include "LuaType.h"
#include "LuaTable.h"
#include "LuaLib.hpp"

#include <stdint.h>

namespace Lua {

	/// <summary>
	/// Enum representing the possible outcomes of a protected call.
	/// </summary>
	enum class ProtectedCallResult {

		/// <summary>
		/// No errors occured.
		/// </summary>
		Ok = LUA_OK,

		/// <summary>
		/// A runtime error occured.
		/// </summary>
		Runtime = LUA_ERRRUN,

		/// <summary>
		/// An allocation error; the error handler was not invoked.
		/// </summary>
		Memory = LUA_ERRMEM,

		/// <summary>
		/// An error occured while running the error handler.
		/// </summary>
		ErrorHandler = LUA_ERRERR

	};

	/// <summary>
	/// Class representing the Lua thread state. This class cannot be inheritted.
	/// </summary>
	public ref class LuaState sealed {
	private:

		~LuaState();

	public:

		/// <summary>
		/// Load a string of Lua code.
		/// </summary>
		/// <param name="lStr">The lua code string to load.</param>
		/// <returns>If string was successfully loaded, <see langword="true"/>; Otherwise <see langword="false"/></returns>
		bool LoadString(System::String^ lStr);

		/// <summary>
		/// Load a file containing Lua code.
		/// </summary>
		/// <param name="filepath">The path to the file to load.</param>
		/// <returns>If file was successfully loaded, <see langword="true"/>; Otherwise <see langword="false"/></returns>
		bool LoadFile(System::String^ filepath);

		/// <summary>
		/// Load and run a string of Lua code.
		/// </summary>
		/// <param name="lStr">The lua code string to execute.</param>
		/// <returns>if string was successfully loaded and exectured <see langword="true"/> is returned; Otherwise <see langword="false"/></returns>
		bool DoString(System::String^ lStr);
		
		/// <summary>
		/// Load and run the file at the specified destination.
		/// </summary>
		/// <param name="filepath">The path to the file that is to be loaded and executed.</param>
		/// <returns>If the file is successfully loaded and executed <see langword="true"/>; Otherwise <see langword="false"/>.</returns>
		bool DoFile(System::String^ filepath);

		/// <summary>
		/// Get the <see cref="LuaType"/> of the top stack element.
		/// </summary>
		/// <returns>The <see cref="LuaType"/> of the top stack element.</returns>
		LuaType Type() {
			return this->Type(-1);
		}

		/// <summary>
		/// Get the <see cref="LuaType"/> of the specified stack element.
		/// </summary>
		/// <param name="index">The stack element to get type of. A value of -1 would return the type of the top stack element.</param>
		/// <returns>The <see cref="LuaType"/> of the specified stack element.</returns>
		LuaType Type(int index);

		/// <summary>
		/// Get the Lua typename of the top element.
		/// </summary>
		/// <returns>The lua typename of the top element.</returns>
		System::String^ Typename() {
			return this->Typename(-1);
		}

		/// <summary>
		/// Get the typename of the specified stack element.
		/// </summary>
		/// <param name="index">The index of the stack value to get typename of.</param>
		/// <returns>The Lua name for specified value type.</returns>
		System::String^ Typename(int index);

		/// <summary>
		/// Get the numeric value of the top stack value.
		/// </summary>
		/// <returns>The numeric value of the top stack value.</returns>
		double GetNumber() {
			return this->GetNumber(-1);
		}

		/// <summary>
		/// Get the numeric value of the specified stack value.
		/// </summary>
		/// <param name="index">The index of the stack value to get numeric value of.</param>
		/// <returns>The numeric value of the specified stack value.</returns>
		double GetNumber(int index);

		/// <summary>
		/// Get the boolean value of the top stack value.
		/// </summary>
		/// <returns>The boolean value of the top stack value.</returns>
		bool GetBoolean() {
			return this->GetBoolean(-1);
		}

		/// <summary>
		/// Get the boolean value of the specified stack value.
		/// </summary>
		/// <param name="index">The index of the stack value to get boolean value of.</param>
		/// <returns>The boolean value of the specified stack value.</returns>
		bool GetBoolean(int index);

		/// <summary>
		/// Get the integer value of the top stack value.
		/// </summary>
		/// <returns>The integer value of the top stack value.</returns>
		int64_t GetInteger() {
			return this->GetInteger(-1);
		}

		/// <summary>
		/// Get the integer value of the specified stack value.
		/// </summary>
		/// <param name="index">The index of the stack value to get integer value of.</param>
		/// <returns>The integer value of the specified stack value.</returns>
		int64_t GetInteger(int index);

		/// <summary>
		/// Get the string value of the top stack value.
		/// </summary>
		/// <returns>The string value of the top stack value.</returns>
		System::String^ GetString() {
			return this->GetString(-1);
		}

		/// <summary>
		/// Get the string value of the specified stack value.
		/// </summary>
		/// <param name="index">The index of the stack value to get the string value of.</param>
		/// <returns>The string value of the specifed stack value.</returns>
		System::String^ GetString(int index);

		/// <summary>
		/// Get the top userdata stack value as its .NET representation.
		/// </summary>
		/// <returns>The .NET instance instance represented by the Lua userdata.</returns>
		System::Object^ GetUserdata() {
			return this->GetUserdata(-1);
		}

		/// <summary>
		/// Get the specified userdata stack value as its .NET representation.
		/// </summary>
		/// <param name="index">The stack index to get userdata instance from.</param>
		/// <returns>The .NET instance represented by the Lua userdata.</returns>
		System::Object^ GetUserdata(int index);

		/// <summary>
		/// Get the top userdata stack value as its <typeparamref name="T"/> representation.
		/// </summary>
		/// <typeparam name="T">The specific userdata type to get.</typeparam>
		/// <returns>The Lua userdata-encapsulated <typeparamref name="T"/> instance</returns>
		generic<class T>
		T GetUserdata() {
			return safe_cast<T>(this->GetUserdata(-1));
		}

		/// <summary>
		/// Get the specified userdata stack value as its <typeparamref name="T"/> representation.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="index">The stack index to get userdata instance from.</param>
		/// <returns>The Lua userdata-encapsulated <typeparamref name="T"/> instance</returns>
		generic<class T>
		T GetUserdata(int index) {
			return safe_cast<T>(this->GetUserdata(index));
		}

		/// <summary>
		/// Push the specified global variable onto the top of the stack.
		/// </summary>
		/// <param name="name">The name of the globale variable.</param>
		/// <returns>The <see cref="LuaType"/> of the value pushed onto the stack. If the global variable does not exist a nil value is pushed onto the stack.</returns>
		LuaType GetGlobal(System::String^ name);

		/// <summary>
		/// Pop the current top value from the stack and assign it the specified global name.
		/// </summary>
		/// <param name="name">The global name of the variable.</param>
		void SetGlobal(System::String^ name);

		/// <summary>
		/// Push an integer onto the stack.
		/// </summary>
		/// <param name="value">The integer value to push onto the stack.</param>
		void PushInteger(int32_t value) {
			this->PushInteger(static_cast<int64_t>(value));
		}

		/// <summary>
		/// Push an integer onto the stack.
		/// </summary>
		/// <param name="value">The integer value to push onto the stack.</param>
		void PushInteger(int64_t value);

		/// <summary>
		/// Push a number onto the stack.
		/// </summary>
		/// <param name="value">The number value to push onto the stack.</param>
		void PushNumber(double value);

		/// <summary>
		/// Push a string onto the stack.
		/// </summary>
		/// <param name="value">The string value to push onto the stack.</param>
		void PushString(System::String^ value);

		/// <summary>
		/// Push a boolean onto the stack.
		/// </summary>
		/// <param name="value">The boolean value to push onto the stack.</param>
		void PushBoolean(bool value);

		/// <summary>
		/// Push a C# method onto the top of the stack.
		/// </summary>
		/// <param name="func">The C# function to push onto the stack.</param>
		void PushCSharpFunction(LuaFunctionDelegate^ func);

		/// <summary>
		/// Push a nil (<see langword="null"/>) value onto the stack.
		/// </summary>
		void PushNil();

		/// <summary>
		/// Push a copy of the top stack element onto the stack at the specified index.
		/// </summary>
		/// <param name="index">The stack index to push copy to.</param>
		void PushValue(int index);

		/// <summary>
		/// Push a light userdata instance onto the stack.
		/// </summary>
		/// <param name="obj">The object to push onto the stack.</param>
		void PushLightUserdata(System::Object^ obj);

		/// <summary>
		/// Push a ligt userdata instance onto the stack.
		/// </summary>
		/// <typeparam name="T">The specific type of object to push</typeparam>
		/// <param name="value">The object to push onto the stack.</param>
		generic<class T>
		void PushLightUserdata(T value) {
			this->PushLightUserdata(safe_cast<System::Object^>(value));
		}

		/// <summary>
		/// Create a new instance of the specified type where the internal Lua pointer is managed by the Lua GC.
		/// </summary>
		/// <param name="type">The type of the object to instantiate and create Lua value of.</param>
		/// <returns>The newly created object.</returns>
		System::Object^ NewUserdata(System::Type^ type);

		/// <summary>
		/// Create a new <typeparamref name="T"/> instance, where the internal Lua pointer is managed by the Lua GC.
		/// </summary>
		/// <typeparam name="T">The type of object to create as user data</typeparam>
		/// <returns>The newly created <typeparamref name="T"/> instance.</returns>
		generic<class T> where T : ref class
		T NewUserdata() {
			return safe_cast<T>(this->NewUserdata(T::typeid));
		}

		/// <summary>
		/// Create a new table with a pre-allocated array and pre-allocated dictionary sections
		/// </summary>
		/// <param name="arraySize">The size of the array section.</param>
		/// <param name="dictionarySize">The size of the dictionary section.</param>
		/// <returns>A <see cref="LuaTable"/> instance representing the table.</returns>
		/// <exception cref="System::ArgumentOutOfRangeException"/>
		LuaTable CreateTable(int arraySize, int dictionarySize);

		/// <summary>
		/// Create a new <see cref="LuaTable"/> from a .NET <see cref="System::Collections::Hashtable"/> instance.
		/// </summary>
		/// <param name="table">The <see cref="System::Collections::Hashtable"/> instance to push onto the Lua stack.</param>
		/// <returns>A <see cref="LuaTable"/> instance representing the pushed <see cref="System::Collections::Hashtable"/>.</returns>
		LuaTable CreateTable(System::Collections::Hashtable^ table);

		/// <summary>
		/// Create a new <see cref="LuaTable"/> from a .NET <see cref="System::Collections::IList"/> instance.
		/// </summary>
		/// <param name="list">The <see cref="System::Collections::IList"/> instance to push onto the Lua stack.</param>
		/// <returns>A <see cref="LuaTable"/> instance representing the pushed <see cref="System::Collections::IList"/>.</returns>
		LuaTable CreateTable(System::Collections::IList^ list);

		/// <summary>
		/// Create a new table with no pre-allocated array or dictionary sections.
		/// </summary>
		/// <remarks>
		/// Equivalent to calling: <c>L.CreateTable(0,0);</c>
		/// </remarks>
		/// <returns>A <see cref="LuaTable"/> instance representing the table.</returns>
		LuaTable CreateTable() {
			return this->CreateTable(0, 0);
		}

		/// <summary>
		/// Create a new metatable associated with the specified name.
		/// </summary>
		/// <param name="name">The name of the metatable.</param>
		/// <param name="alreadyExists">Get if the metatable already exists.</param>
		/// <returns>The <see cref="LuaTable"/> instance that was pushed onto the stack.</returns>
		LuaTable NewMetatable(System::String^ name, [System::Runtime::InteropServices::OutAttribute] bool% alreadyExists);

		/// <summary>
		/// Set the metable of the current stack top element to the specified metatable.
		/// </summary>
		/// <param name="name">The name of the metatable to set.</param>
		void SetMetatable(System::String^ name);

		/// <summary>
		/// Pop the current stack value.
		/// </summary>
		void Pop() {
			this->Pop(1);
		}

		/// <summary>
		/// Pop the specified amount of stack values.
		/// </summary>
		/// <param name="count">The amount of items to pop.</param>
		void Pop(int count);

		/// <summary>
		/// Returns the number of elements on the stack, which is also the index of the top element.
		/// </summary>
		/// <returns>The number of elements on the stack.</returns>
		int GetTop();

		/// <summary>
		/// Set the top index (number of elements) on the stack to the specified value. 
		/// If the new top value is lower that the previous value; the top values are discarded. Otherwise, the stack is populated with nil values to get the new size.
		/// </summary>
		/// <example>
		/// The stack can be cleared with
		/// <code>
		/// L.SetTop(0);
		/// </code>
		/// </example>
		/// <remarks>
		/// Using negative values is allowed and will set the stack position to the specified index. A shortcut for this is <see cref="LuaState::Pop(int count)"/>.
		/// </remarks>
		/// <param name="top">The new amount of stack values.</param>
		void SetTop(int top);

		/// <summary>
		/// Remove the element at the specified index, shifting down all elements on the top of the specified index to fill the gap.
		/// </summary>
		/// <param name="index">The index of the stack element to remove.</param>
		void Remove(int index);

		/// <summary>
		/// Move the top element to the specified stack index, shifting up all elements on top of that position to open space.
		/// </summary>
		/// <param name="index">The index of the place to insert top value into.</param>
		void Insert(int index);

		/// <summary>
		/// Pops a value from the top and sets it as the value at the specified index without moving anything.
		/// </summary>
		/// <param name="index">The stack index to replace with top value.</param>
		void Replace(int index);

		/// <summary>
		/// Call the Lua closure currently stored at the top of the stack, offset by the specified amount of arguments.
		/// </summary>
		/// <param name="argc">The amount of arguments to call the function with.</param>
		/// <param name="retc">The amount of return values. Use <see cref="LuaState::MultiReturn"/> to get all return values.</param>
		void Call(int argc, int retc);

		/// <summary>
		/// Perform a protected call using the Lua closure currently stored at the top of the stack, offset by the specified amount of arguments.
		/// </summary>
		/// <param name="argc">The amount of arguments to call the function with.</param>
		/// <param name="retc">The amount of return values. Use <see cref="LuaState::MultiReturn"/> to get all return values.</param>
		/// <param name="errfunc">If 0 the standard error message is pushed onto the stack; Otherwise the stack index of the error handler function.</param>
		/// <returns>A <see cref="ProtectedCallResult"/> value describing the result of the protected call.</returns>
		ProtectedCallResult PCall(int argc, int retc, int errfunc);

	public:

		/// <summary>
		/// Get the current amount of items on the stack.
		/// </summary>
		/// <remarks>
		/// Equivalent to calling <c>L.GetTop()</c>
		/// </remarks>
		property int Top {
			int get() { return this->GetTop(); }
		}

	public:

		/// <summary>
		/// Create a new Lua state with all standard libraries loaded.
		/// </summary>
		/// <returns>A new <see cref="LuaState"/> instance.</returns>
		static LuaState^ NewState();

		/// <summary>
		/// Create a new Lua state with all the specified libraries loaded.
		/// </summary>
		/// <param name="libraries">The standard libraries to load.</param>
		/// <returns>A new <see cref="LuaState"/> instance.</returns>
		static LuaState^ NewState(LuaLib libraries);

		/// <summary>
		/// Option for multiple returns in calls to <see cref="LuaState::Call"/> and <see cref="LuaState::PCall"/>.
		/// </summary>
		literal int MultiReturn = LUA_MULTRET;

	private:

		lua_State* pState;
		bool bIgnoreClose;

	internal:

		LuaState(lua_State* pL, bool shouldClose);

		lua_State* get_state() { return this->pState; }

		/// <summary>
		/// (C++/CLI) Generic-safe stack pop operation.
		/// </summary>
		/// <param name="L">Lua state</param>
		/// <param name="amount">Elements to pop</param>
		static void pop_generic_safe(lua_State* L, int amount);

	};

}
