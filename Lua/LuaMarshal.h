#pragma once
#include "LuaType.h"

struct lua_State;

namespace Lua {

	/// <summary>
	/// Static utility class for marshalling between C++/CLI, C#, C, and Lua
	/// </summary>
	ref class LuaMarshal sealed abstract {
	
	public:

		/// <summary>
		/// Marshal the current stack value into a managed (C#) object.
		/// </summary>
		/// <param name="L">The lua state to retrieve stack value from.</param>
		/// <param name="idx">The stack offset to retrieve value from</param>
		/// <returns></returns>
		static System::Object^ MarshalStackValue(lua_State* L, int idx);

		/// <summary>
		/// Marshal the current stack value into a managed (C#) object.
		/// </summary>
		/// <param name="L">The lua state to retrieve stack value from.</param>
		/// <param name="t">The lua type to marshal.</param>
		/// <param name="idx">The stack offset to retrieve value from</param>
		/// <returns></returns>
		static System::Object^ MarshalStackValue(lua_State* L, LuaType t, int idx);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="L"></param>
		/// <returns></returns>
		static System::Object^ MarshalTopStackAndPop(lua_State* L);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="L"></param>
		/// <param name="obj"></param>
		static void MarshalToStack(lua_State* L, System::Object^ obj);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="L"></param>
		static void MarshalHashTableToStack(lua_State* L, System::Collections::Hashtable^ table);

		/// <summary>
		/// 
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="values"></param>
		/// <returns></returns>
		generic<class T>
		static T ArrayToTuple(array<System::Object^>^ values);

	};

}
