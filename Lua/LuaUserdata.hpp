#pragma once
#include "LuaState.h"

namespace Lua {

	/// <summary>
	/// Attribute marking a method as visible in the Lua environment.
	/// </summary>
	[System::AttributeUsageAttribute(System::AttributeTargets::Method, AllowMultiple = false)]
	public ref class LuaFunctionAttribute : public System::Attribute {
	public:
		/// <summary>
		/// Get or set the Lua name for the method. If <see langword="null"/> or empty, the method name is used.
		/// </summary>
		property System::String^ Name;
	};

	/// <summary>
	/// Attribute marking a property as visible in the Lua environment.
	/// </summary>
	[System::AttributeUsageAttribute(System::AttributeTargets::Property, AllowMultiple = false)]
	public ref class LuaFieldAttribute : public System::Attribute {
	public:
		/// <summary>
		/// Get or set the Lua name for the property. If <see langword="null"/> or empty, the property name is used.
		/// </summary>
		property System::String^ Name;
		/// <summary>
		/// Get or set if Lua can only read the value.
		/// </summary>
		property bool Readonly;
		/// <summary>
		/// Get or set if Lua can only write to the value.
		/// </summary>
		property bool Writonly;
	};

	/// <summary>
	/// Static class providing helper methods for extended functionality of Userdata.
	/// </summary>
	public ref class LuaUserdata abstract sealed {

	public:

		/// <summary>
		/// Create a new Userdata instance in the Lua environment of the specified type.
		/// </summary>
		/// <param name="state">The state to create new userdata object in.</param>
		/// <param name="type">The .NET userdata type to create.</param>
		/// <returns>The instance that was created.</returns>
		static System::Object^ NewUserdata(LuaState^ state, System::Type^ type);

		/// <summary>
		/// Create a new <typeparamref name="T"/> Userdata instance in the Lua environment.
		/// </summary>
		/// <typeparam name="T">The specific userdata type.</typeparam>
		/// <param name="state">The state to create new userdata object in.</param>
		/// <returns>The instance that was created.</returns>
		generic<class T> where T : ref class 
		static T NewUserdata(LuaState^ state) { return safe_cast<T>(NewUserdata(state, T::typeid)); }

	public:

		/// <summary>
		/// Get or set if the runtime should trigger an error when attempting to index a field not found on a C# userdata object.
		/// </summary>
		static property bool ErrorOnIndexNotFound;

	private:

		// Creates a Lua metatable for the specified type
		static void CreateTypeMetatable(lua_State* L, System::Type^ type, const char* tName);

	};

}
