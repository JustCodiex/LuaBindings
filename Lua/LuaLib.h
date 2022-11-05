#pragma once

namespace Lua {

	/// <summary>
	/// Enum representing a standard library.
	/// </summary>
	public enum class LuaLib : int {

		None = 0,

		Base = 1,

		Coroutine = 2,

		Table = 4,

		IO = 8,

		OS = 16,

		String = 32,

		UTF8 = 64,

		Math = 128,

		Debug = 256,

		Package = 512,

		All = Base | Coroutine | Table | IO | OS | String | UTF8 | Math | Debug | Package

	};

}
