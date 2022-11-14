#include "LuaUserdata.hpp"
#include "LuaMarshal.h"
#include "CLIMacros.hpp"

using namespace System;
using namespace System::Reflection;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

Object^ Lua::LuaUserdata::NewUserdata(LuaState^ state, Type^ type) {
	
	// Grab lstate
	lua_State* L = state->get_state();

	// Invoke basic 
	Object^ obj = state->NewUserdata(type);

	// Grab C/C++ string for typename
	__UnmanagedString(tStrPtr, type->FullName, pTypStr);

	// Check if nil
	if (luaL_getmetatable(L, pTypStr) == LUA_TNIL) {
		lua_pop(L, 1); // pop nil
		CreateTypeMetatable(L, type, pTypStr);
	}

	// Set the metatable
	lua_setmetatable(L, -2);

	// Free the unmanaged string
	__UnmangedFreeString(tStrPtr);

	// Get top
	int top = lua_gettop(L);

	// Return resulting userdata instance
	return obj;

}

static int userdata_indexself(lua_State*L) { // Simple "C" function to perform closure upvalue indexing for a userdata metatable.

	// Grab index
	const char* pLookupStr = lua_tostring(L, 2);
	char errbuffer[128];

	// Ensure upvalue exists
	if (!lua_istable(L, lua_upvalueindex(1))) {
		sprintf_s(errbuffer, "failed to invoke %s since lua upvalue is %s", CPPCLI_NAMEOF(userdata_indexself), lua_typename(L, lua_upvalueindex(1)));
		lua_pushstring(L, errbuffer);
		lua_error(L);
		return 0;
	}

	// Get value onto the stack
	if (lua_getfield(L, lua_upvalueindex(1), pLookupStr) <= LUA_TNIL) {

		// Pop nil
		lua_pop(L, 1);

		// Try get from field value
		if (lua_getfield(L, lua_upvalueindex(2), pLookupStr) <= LUA_TNIL) {
			if (!Lua::LuaUserdata::ErrorOnIndexNotFound) {
				return 1;
			}
			lua_pop(L, 1);
			sprintf_s(errbuffer, "attempt to index %s on a userdata value", pLookupStr);
			lua_pushstring(L, errbuffer);
			lua_error(L);
			return 0;
		}

		// Push userdata
		lua_pushvalue(L, 1);

		// Call it
		lua_call(L, 1, 1);

	}

	// Return 1 (the lookup value)
	return 1;

}

static int userdata_newindexself(lua_State* L) {
	
	// Grab index
	const char* pLookupStr = lua_tostring(L, 2);

	// Ensure upvalue exists
	if (!lua_istable(L, lua_upvalueindex(1))) {
		char buffer[128];
		sprintf_s(buffer, "failed to invoke %s since lua upvalue is %s", CPPCLI_NAMEOF(userdata_indexself), lua_typename(L, lua_upvalueindex(1)));
		lua_pushstring(L, buffer);
		lua_error(L);
		return 0;
	}

	// Get value onto the stack
	lua_getfield(L, lua_upvalueindex(1), pLookupStr); // TODO: Do lua error here if nil is returned

	// Push the instance
	lua_pushvalue(L, 1);

	// Push the new value
	lua_pushvalue(L, 3);

	// Call it
	lua_call(L, 2, 0);

	// Return 0 (nothing is pushed)
	return 0;

}

static void PushIndexFunction(lua_State* L, MethodInfo^ method, System::String^ name) {

	// Push it
	Lua::LuaMarshal::CreateCSharpLuaFunction(L, Lua::LuaMarshal::CreateLuaDelegate(method));

	// Get C string
	__UnmanagedString(f, name, pFnName);

	// Name it
	lua_setfield(L, -2, pFnName);

	// Free
	__UnmangedFreeString(f);

}

void Lua::LuaUserdata::CreateTypeMetatable(lua_State* L, Type^ type, const char* tName) {

	// Create type metatable
	luaL_newmetatable(L, tName);

	// Fetch method and property candidates
	auto methods = gcnew List<MethodInfo^>(type->GetMethods(BindingFlags::Instance | BindingFlags::Public));
	auto properties = gcnew List<PropertyInfo^>(type->GetProperties(BindingFlags::Instance | BindingFlags::Public));

	// Create containers for functions and fields
	auto functions = gcnew List<ValueTuple<MethodInfo^, LuaFunctionAttribute^>>();
	auto fields = gcnew List<ValueTuple<PropertyInfo^, LuaFieldAttribute^>>();

	// Filter methods
	for (int i = 0; i < methods->Count; i++) {
		auto attrib = methods[i]->GetCustomAttributes(LuaFunctionAttribute::typeid, true);
		if (attrib->Length == 1) {
			functions->Add(ValueTuple<MethodInfo^, LuaFunctionAttribute^>(methods[i], safe_cast<LuaFunctionAttribute^>(attrib[0])));
		}
	}

	// Filter fields
	for (int i = 0; i < properties->Count; i++) {
		auto attrib = properties[i]->GetCustomAttributes(LuaFieldAttribute::typeid, true);
		if (attrib->Length == 1) {
			fields->Add(ValueTuple<PropertyInfo^, LuaFieldAttribute^>(properties[i], safe_cast<LuaFieldAttribute^>(attrib[0])));
		}
	}

	// Push a new table that will act as an upvalue
	// Will contain function lookup data for the type
	lua_newtable(L);

	// Loop over functions and create lua delegates
	for (int i = 0; i < functions->Count; i++) {
		PushIndexFunction(L, functions[i].Item1, String::IsNullOrEmpty(functions[i].Item2->Name) ? functions[i].Item1->Name : functions[i].Item2->Name);
	}

	// Push a new table that will act as an upvalue
	// Will contain field lookup data for the type
	lua_newtable(L);

	// Loop over fields and create lua delegates for the getters
	for (int i = 0; i < fields->Count; i++) {
		if (fields[i].Item1->GetMethod == nullptr || fields[i].Item2->Writonly)
			continue;
		PushIndexFunction(L, fields[i].Item1->GetMethod, String::IsNullOrEmpty(fields[i].Item2->Name) ? fields[i].Item1->Name : fields[i].Item2->Name);
	}

	// Push a C closure
	lua_pushcclosure(L, userdata_indexself, 2);

	// Save as __index
	lua_setfield(L, -2, "__index");

	// Push a new table that will act as an upvalue
	// Will contain function lookup data for the type when assigning values
	lua_newtable(L);

	// Loop over setters
	for (int i = 0; i < fields->Count; i++) {
		if (fields[i].Item1->SetMethod == nullptr || fields[i].Item2->Readonly)
			continue;
		PushIndexFunction(L, fields[i].Item1->SetMethod, String::IsNullOrEmpty(fields[i].Item2->Name) ? fields[i].Item1->Name : fields[i].Item2->Name);
	}

	// Push a C closure
	lua_pushcclosure(L, userdata_newindexself, 1);

	// Save as __newindex
	lua_setfield(L, -2, "__newindex");

}
