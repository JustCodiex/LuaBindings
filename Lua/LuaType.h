#pragma once
#include "lua/lua.hpp"

namespace Lua {

	/// <summary>
	/// Enum representing all valid Lua types.
	/// </summary>
	public enum class LuaType : int {
	
		/// <summary>
		/// The nil type
		/// </summary>
		Nil = LUA_TNIL,

		/// <summary>
		/// The boolean type
		/// </summary>
		Boolean = LUA_TBOOLEAN,

		/// <summary>
		/// The ligh userdata type
		/// </summary>
		LightUserData = LUA_TLIGHTUSERDATA,

		/// <summary>
		/// The number type
		/// </summary>
		Number = LUA_TNUMBER,

		/// <summary>
		/// The string type
		/// </summary>
		String = LUA_TSTRING,

		/// <summary>
		/// The table type
		/// </summary>
		Table = LUA_TTABLE,

		/// <summary>
		/// The function type
		/// </summary>
		Function = LUA_TFUNCTION,

		/// <summary>
		/// The userdata type
		/// </summary>
		UserData = LUA_TUSERDATA,

		/// <summary>
		/// The thread/coroutine type
		/// </summary>
		Thread = LUA_TTHREAD,

	};

}
