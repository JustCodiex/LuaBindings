#pragma once

#include "lua/luabind.hpp"
#include "LuaType.h"
#include "LuaTable.h"
#include "LuaLib.h"

#include <stdint.h>

namespace Lua {

	/// <summary>
	/// Delegate representing a function that can be invoked directly by lua.
	/// </summary>
	/// <param name="state">The Lua state the function was invoked in.</param>
	public delegate int LuaFunctionDelegate(ref class LuaState^ state);

	/// <summary>
	/// Class representing the Lua thread state. This class cannot be inheritted.
	/// </summary>
	public ref class LuaState sealed {
	private:

		~LuaState();

	public:

		/// <summary>
		/// 
		/// </summary>
		/// <param name="lStr"></param>
		/// <returns></returns>
		bool LoadString(System::String^ lStr);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="filepath"></param>
		/// <returns></returns>
		bool LoadFile(System::String^ filepath);

		/// <summary>
		/// Load and run a string of Lua code.
		/// </summary>
		/// <param name="lStr">The lua code string to execute.</param>
		/// <returns>if string was successfully loaded and exectured <see langword="true"/> is returned; Otherwise <see langword="false"/></returns>
		bool DoString(System::String^ lStr);
		
		/// <summary>
		/// 
		/// </summary>
		/// <param name="filepath"></param>
		/// <returns></returns>
		bool DoFile(System::String^ filepath);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		LuaType Type() {
			return this->Type(-1);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="stackOffset"></param>
		/// <returns></returns>
		LuaType Type(int stackOffset);

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
		/// 
		/// </summary>
		/// <returns></returns>
		double GetNumber() {
			return this->GetNumber(-1);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="stackOffset"></param>
		/// <returns></returns>
		double GetNumber(int stackOffset);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		bool GetBoolean() {
			return this->GetBoolean(-1);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="stackOffset"></param>
		/// <returns></returns>
		bool GetBoolean(int stackOffset);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		int64_t GetInteger() {
			return this->GetInteger(-1);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="stackOffset"></param>
		/// <returns></returns>
		int64_t GetInteger(int stackOffset);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		System::String^ GetString() {
			return this->GetString(-1);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="stackOffset"></param>
		/// <returns></returns>
		System::String^ GetString(int stackOffset);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		System::Object^ GetUserdata() {
			return this->GetUserdata(-1);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="stackOffset"></param>
		/// <returns></returns>
		System::Object^ GetUserdata(int stackOffset);

		/// <summary>
		/// 
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns></returns>
		generic<class T>
		T GetUserdata() {
			return safe_cast<T>(this->GetUserdata(-1));
		}

		/// <summary>
		/// 
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="stackOffset"></param>
		/// <returns></returns>
		generic<class T>
		T GetUserdata(int stackOffset) {
			return safe_cast<T>(this->GetUserdata(stackOffset));
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="name"></param>
		LuaType GetGlobal(System::String^ name);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="name"></param>
		void SetGlobal(System::String^ name);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="value"></param>
		void PushInteger(int32_t value) {
			this->PushInteger(static_cast<int64_t>(value));
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="value"></param>
		void PushInteger(int64_t value);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="value"></param>
		void PushNumber(double value);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="value"></param>
		void PushString(System::String^ value);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="value"></param>
		void PushBoolean(bool value);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="table"></param>
		void PushTable(System::Collections::Hashtable^ table);

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
		/// 
		/// </summary>
		/// <param name="obj"></param>
		void NewUserdata(System::Object^ obj);

		/// <summary>
		/// 
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="value"></param>
		generic<class T> where T : ref class
		void NewUserdata(T value) {
			this->NewUserdata(safe_cast<System::Object^>(value));
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="arraySize"></param>
		/// <param name="dictionarySize"></param>
		/// <returns></returns>
		LuaTable CreateTable(int arraySize, int dictionarySize);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="table"></param>
		/// <returns></returns>
		LuaTable CreateTable(System::Collections::Hashtable^ table);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
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
		/// 
		/// </summary>
		/// <param name="name"></param>
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
		/// 
		/// </summary>
		/// <param name="argc"></param>
		/// <param name="retc"></param>
		void Call(int argc, int retc);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="argc"></param>
		/// <param name="retc"></param>
		/// <returns></returns>
		int PCall(int argc, int retc);

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

	private:

		lua_State* pState;
		bool bIgnoreClose;

	internal:

		LuaState(lua_State* pL, bool shouldClose);

		lua_State* get_state() { return this->pState; }

	};

}
