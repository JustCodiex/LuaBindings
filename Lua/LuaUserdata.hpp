#pragma once
#include "LuaState.h"
#include "LuaAttributes.hpp"

namespace Lua {

	/// <summary>
	/// Static class providing helper methods for extended functionality of Userdata.
	/// </summary>
	public ref class LuaUserdata abstract sealed {

	public:

		/// <summary>
		/// Create a new Userdata instance in the Lua environment of the specified type.
		/// </summary>
		/// <remarks>
		/// Ignores <see cref="Lua::LuaMetamethodAttribute"/> attributes where the method is <see cref="Lua::LuaMetamethod::Index"/> and 
		/// <see cref="Lua::LuaMetamethod::NewIndex"/>
		/// </remarks>
		/// <param name="state">The state to create new userdata object in.</param>
		/// <param name="type">The .NET userdata type to create.</param>
		/// <returns>The instance that was created.</returns>
		static System::Object^ NewUserdata(LuaState^ state, System::Type^ type);

		/// <summary>
		/// Create a new <typeparamref name="T"/> Userdata instance in the Lua environment.
		/// </summary>
		/// <remarks>
		/// Ignores <see cref="Lua::LuaMetamethodAttribute"/> attributes where the method is <see cref="Lua::LuaMetamethod::Index"/> and 
		/// <see cref="Lua::LuaMetamethod::NewIndex"/>
		/// </remarks>
		/// <typeparam name="T">The specific userdata type.</typeparam>
		/// <param name="state">The state to create new userdata object in.</param>
		/// <returns>The instance that was created.</returns>
		generic<class T> where T : ref class 
		static T NewUserdata(LuaState^ state) { return safe_cast<T>(NewUserdata(state, T::typeid)); }

		/// <summary>
		/// Register the <typeparamref name="T"/> as a valid userdata type.
		/// </summary>
		/// <typeparam name="T">The type of userdata to create metadata for.</typeparam>
		/// <param name="state">The state to register new userdata type in.</param>
		generic <class T> where T : ref class
		static void RegisterType(LuaState^ state);

		/// <summary>
		/// Gets if the specified type is a registered userdata type.
		/// </summary>
		/// <param name="type"></param>
		/// <returns></returns>
		static bool IsUserdataType(System::Type^ type) {
			return  __userdatatypes->Contains(type);
		}

	public:

		/// <summary>
		/// Get or set if the runtime should trigger an error when attempting to index a field not found on a C# userdata object.
		/// </summary>
		static property bool ErrorOnIndexNotFound;

	private:

		// Creates a Lua metatable for the specified type
		static void CreateTypeMetatable(lua_State* L, System::Type^ type, const char* tName);

	private:

		initonly static System::Collections::Generic::HashSet<System::Type^>^ __userdatatypes
			= gcnew System::Collections::Generic::HashSet<System::Type^>();

	};

}
