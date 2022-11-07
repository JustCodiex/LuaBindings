#include "LuaMarshal.h"
#include "LuaTable.h"
#include "LuaFunction.hpp"
#include "lua/luabind.hpp"
#include "CLIMacros.h"

using namespace System::Runtime::InteropServices;

System::Object^ Lua::LuaMarshal::MarshalStackValue(lua_State* L, int idx) {
	LuaType lTy = static_cast<LuaType>(lua_type(L, idx));
	return MarshalStackValue(L, lTy, idx);
}

System::Object^ Lua::LuaMarshal::MarshalStackValue(lua_State* L, LuaType t, int idx) {
	switch (t) {
		case Lua::LuaType::Nil:
			return nullptr;
		case Lua::LuaType::Boolean:
			return static_cast<bool>(lua_toboolean(L, idx));
		case Lua::LuaType::LightUserData:
			throw gcnew System::NotImplementedException();
		case Lua::LuaType::Number:
			return static_cast<double>(lua_tonumber(L, idx));
		case Lua::LuaType::String:
			return Marshal::PtrToStringAnsi(static_cast<System::IntPtr>(const_cast<char*>(lua_tostring(L, idx))));
		case Lua::LuaType::Table: {
			LuaTable table = LuaTable::from_top(L, -1);
			return safe_cast<System::Object^>(table.ToHashtable());
		}
		case Lua::LuaType::Function: {
			LuaFunction funk = LuaFunction::from_top(L, -1);
			return safe_cast<System::Object^>(funk);
		}
		case Lua::LuaType::UserData:
			throw gcnew System::NotImplementedException();
		case Lua::LuaType::Thread:
			throw gcnew System::NotImplementedException();
		default:
			throw gcnew System::NotSupportedException();
	}

}

System::Object^ Lua::LuaMarshal::MarshalTopStackAndPop(lua_State* L) {

	// Marshal current top
	auto result = MarshalStackValue(L, -1);
	
	// Pop it
	lua_pop(L, 1);

	// Return result
	return result;

}

void Lua::LuaMarshal::MarshalToStack(lua_State* L, System::Object^ obj) {

	// Grab type
	auto ty = obj->GetType();
	if (ty == System::String::typeid) {

		// Get unmanaged string
		__UnmanagedString(tmp, safe_cast<System::String^>(obj), pStr);

		// Push it
		lua_pushstring(L, pStr);

		// Free it
		__UnmangedFreeString(tmp);

	} else if (ty == System::Double::typeid) {
		lua_pushnumber(L, safe_cast<double>(obj));
	} else if (ty == System::Single::typeid) {
		lua_pushnumber(L, static_cast<lua_Number>(safe_cast<float>(obj)));
	} else if (ty == System::Int32::typeid) {
		lua_pushinteger(L, static_cast<lua_Integer>(safe_cast<int>(obj)));
	} else if (ty == System::Collections::Hashtable::typeid) {
		MarshalHashTableToStack(L, safe_cast<System::Collections::Hashtable^>(obj));
	} else {
		throw gcnew System::NotSupportedException();
	}

}

void Lua::LuaMarshal::MarshalHashTableToStack(lua_State* L, System::Collections::Hashtable^ table) {

	// Grab count
	int count = table->Count;

	// Push new table
	lua_createtable(L, 0, count);

	// Get iterator
	auto itt = table->GetEnumerator();
	while (itt->MoveNext()) {

		// Get
		System::Collections::DictionaryEntry current = safe_cast<System::Collections::DictionaryEntry>(itt->Current);

		// Push key
		MarshalToStack(L, current.Key);

		// Push value
		MarshalToStack(L, current.Value);

		// Set
		lua_settable(L, -3);

	}

}

generic<class T>
T Lua::LuaMarshal::ArrayToTuple(array<System::Object^>^ values) {

	// Return using activator (a saving grace)
	return safe_cast<T>(System::Activator::CreateInstance(T::typeid, values));

}
