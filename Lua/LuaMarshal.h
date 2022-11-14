#pragma once
#include "LuaType.h"
#include "LuaFunction.hpp"
#include "LuaState.h"
#include <stdint.h>
#include <map>

#define CPPCLI_NAMEOF(n) #n

namespace Lua {

	/// <summary>
	/// Static utility class for marshalling between C++/CLI, C#, C, and Lua values.
	/// </summary>
	public ref class LuaMarshal sealed abstract {
	
	public:

		/// <summary>
		/// Get the specified tuple type using the given array of values.
		/// </summary>
		/// <typeparam name="T">Tuple type.</typeparam>
		/// <param name="values">The tuple entries. Element count must match the amount of tuple entries.</param>
		/// <returns>The requested tuple with the specified values.</returns>
		generic<class T>
		static T ArrayToTuple(array<System::Object^>^ values);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="dataId"></param>
		/// <returns></returns>
		static System::Object^ GetUserdata(uint64_t dataId);

		/// <summary>
		/// Create a <see cref="LuaFunctionDelegate"/> instance to the given method.
		/// </summary>
		/// <param name="method">The method to create Lua-compatible wrapper for.</param>
		/// <returns>A <see cref="LuaFunctionDelegate"/> that invokes the given method info.</returns>
		static LuaFunctionDelegate^ CreateLuaDelegate(System::Reflection::MethodInfo^ method);

		/// <summary>
		/// Create a <see cref="LuaFunctionDelegate"/> instance invoking the given delegate.
		/// </summary>
		/// <param name="func">The delegate to create Lua-compatible wrapper for.</param>
		/// <returns>A <see cref="LuaFunctionDelegate"/> that invokes the given delegate.</returns>
		static LuaFunctionDelegate^ CreateLuaDelegate(System::Delegate^ func) {
			return CreateLuaDelegate(func->Method);
		}

	internal:

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
		/// <param name="L"></param>
		/// <param name="list"></param>
		static void MarshalListToStack(lua_State* L, System::Collections::IList^ list);

		/// <summary>
		/// 
		/// </summary>
		/// <remarks>
		/// This will return an existing pointer if the object is already registered.
		/// </remarks>
		/// <param name="obj"></param>
		/// <returns></returns>
		static uint64_t* CreateUserdata(System::Object^ obj);

		/// <summary>
		/// 
		/// </summary>
		static void CreateCSharpLuaFunction(lua_State* L, LuaFunctionDelegate^ delegate);

		/// <summary>
		/// 
		/// </summary>
		static LuaType ToLuaType(lua_State* L, int idx);

	private:

		static System::Object^ MarshalStackValueFromNetState(LuaState^ L, int idx) {
			return MarshalStackValue(L->get_state(), idx);
		}

		static void MarshalNestStateValue(LuaState^ L, System::Object^ obj) {
			MarshalToStack(L->get_state(), obj);
		}

	public:

		static LuaMarshal() {
			auto flags = System::Reflection::BindingFlags::NonPublic | System::Reflection::BindingFlags::Static;
			__netStackMarshal = (LuaMarshal::typeid)->GetMethod(CPPCLI_NAMEOF(MarshalStackValueFromNetState), flags);
			__netToStackMarshal = (LuaMarshal::typeid)->GetMethod(CPPCLI_NAMEOF(MarshalNestStateValue), flags);
			__userdataToIdentifier = gcnew System::Collections::Hashtable();
			__identifierToUserdata = gcnew System::Collections::Hashtable();
			__userdataIdentifierCounter = 0;
			__managedPointers = new std::map<uint64_t, uint64_t*>();
		}

		initonly static System::Reflection::MethodInfo^ __netStackMarshal;
		initonly static System::Reflection::MethodInfo^ __netToStackMarshal;

	private:

		initonly static System::Collections::Hashtable^ __userdataToIdentifier;
		initonly static System::Collections::Hashtable^ __identifierToUserdata;
		static uint64_t __userdataIdentifierCounter;

		static std::map<uint64_t, uint64_t*>* __managedPointers;

	};

}
