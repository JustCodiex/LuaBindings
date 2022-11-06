#pragma once

#include "lua/luabind.hpp"
#include "LuaType.h"
#include "LuaLib.h"

#include <stdint.h>

namespace Lua {

	/// <summary>
	/// Delegate representing a function that can be invoked directly by lua.
	/// </summary>
	/// <param name="state">The Lua state the function was invoked in.</param>
	public delegate int LuaFunctionDelegate(ref class LuaState^ state);

	ref class LuaTable;

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
		/// Push a C# method on to the top of the stack.
		/// </summary>
		/// <param name="func">The C# function to push onto the stack.</param>
		void PushCSharpFunction(LuaFunctionDelegate^ func);

		/// <summary>
		/// 
		/// </summary>
		void PushNil();

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		LuaTable^ CreateTable() {
			return this->CreateTable(0, 0);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="arraySize"></param>
		/// <param name="dictionarySize"></param>
		/// <returns></returns>
		LuaTable^ CreateTable(int arraySize, int dictionarySize);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="table"></param>
		/// <returns></returns>
		LuaTable^ CreateTable(System::Collections::Hashtable^ table);

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
		/// 
		/// </summary>
		/// <returns></returns>
		int GetTop();

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
