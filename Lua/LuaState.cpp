#include "LuaState.h"
#include "LuaStateAux.h"
#include "LuaTable.h"
#include "LuaMarshal.h"
#include "LuaException.hpp"
#include "CLIMacros.h"
#include <stdlib.h>

using namespace System::Runtime::InteropServices;

Lua::LuaState::LuaState(lua_State* pL, bool shouldClose) {
	this->pState = pL;
	this->bIgnoreClose = shouldClose;
}

Lua::LuaState::~LuaState() {

	// Bail if already closed
	if (!this->pState || !this->bIgnoreClose) {
		return;
	}

	// Close Lua
	lua_close(this->pState);

	// Nullify (lua_close frees it)
	this->pState = 0;

}

bool Lua::LuaState::LoadString(System::String^ lStr) {
	return false;
}

bool Lua::LuaState::LoadFile(System::String^ filepath) {
	return false;
}

bool Lua::LuaState::DoString(System::String^ lStr) {

	// Grab C++ string
	System::IntPtr strPtr = Marshal::StringToHGlobalAnsi(lStr);
	const char* pLStr = static_cast<const char*>(strPtr.ToPointer());

	// Invoke
	int result = luaL_dostring(this->pState, pLStr);

	// Free unmanaged string
	Marshal::FreeHGlobal(strPtr);

	// Return if success
	return result == LUA_OK;

}

bool Lua::LuaState::DoFile(System::String^ filepath) {

	// Grab C++ string
	System::IntPtr strPtr = Marshal::StringToHGlobalAnsi(filepath);
	const char* pLStr = static_cast<const char*>(strPtr.ToPointer());

	// Invoke
	int result = luaL_dofile(this->pState, pLStr);

	// Free unmanaged string
	Marshal::FreeHGlobal(strPtr);

	// Return if success
	return result == LUA_OK;

}

Lua::LuaType Lua::LuaState::Type(int stackOffset) {
	return static_cast<Lua::LuaType>(lua_type(this->pState, stackOffset));
}

System::String^ Lua::LuaState::Typename(int index) {

	// Grab the typename in C string form
	const char* pStr = lua_typename(this->pState, index);

	// Return a managed version
	return Marshal::PtrToStringAnsi(static_cast<System::IntPtr>(const_cast<char*>(pStr)));

}

double Lua::LuaState::GetNumber(int stackOffset) {
	return lua_tonumber(this->pState, stackOffset);
}

bool Lua::LuaState::GetBoolean(int stackOffset) {
	return lua_toboolean(this->pState, stackOffset);
}

int64_t Lua::LuaState::GetInteger(int stackOffset) {
	return lua_tointeger(this->pState, stackOffset);
}

System::String^ Lua::LuaState::GetString(int stackOffset) {

	// Get C string
	const char* pStr = lua_tostring(this->pState, stackOffset);

	// Get 
	return Marshal::PtrToStringAnsi(static_cast<System::IntPtr>(const_cast<char*>(pStr)));

}

System::Object^ Lua::LuaState::GetUserdata(int stackOffset) {

	// Get userdata
	uint64_t* userdataPtr = static_cast<uint64_t*>(lua_touserdata(this->pState, stackOffset));

	// Get identifier
	uint64_t identifier = *userdataPtr;

	// Ask marshal to get instance
	return LuaMarshal::GetUserdata(identifier);

}

Lua::LuaType Lua::LuaState::GetGlobal(System::String^ name) {

	// Grab C++ string
	__UnmanagedString(strPtr, name, pLStr);

	// Get global on top
	LuaType result = static_cast<LuaType>(lua_getglobal(this->pState, pLStr));

	// Free unmanaged
	__UnmangedFreeString(strPtr);

	// Return result
	return result;

}

void Lua::LuaState::SetGlobal(System::String^ name) {

	// Grab C++ string
	__UnmanagedString(strPtr, name, pLStr);

	// Get global on top
	lua_setglobal(this->pState, pLStr);

	// Free unmanaged
	__UnmangedFreeString(strPtr);

}

void Lua::LuaState::PushNil() {
	lua_pushnil(this->pState);
}

void Lua::LuaState::PushValue(int index) {
	lua_pushvalue(this->pState, index);
}

void Lua::LuaState::PushInteger(int64_t value) {
	lua_pushinteger(this->pState, value);
}

void Lua::LuaState::PushNumber(double value) {
	lua_pushnumber(this->pState, value);
}

void Lua::LuaState::PushBoolean(bool value) {
	lua_pushboolean(this->pState, value);
}

void Lua::LuaState::PushString(System::String^ value) {

	// Grab C++ string
	System::IntPtr strPtr = Marshal::StringToHGlobalAnsi(value);
	const char* pLStr = static_cast<const char*>(strPtr.ToPointer());

	// Get global on top
	lua_pushstring(this->pState, pLStr);

	// Free unmanaged
	Marshal::FreeHGlobal(strPtr);

}

void Lua::LuaState::PushTable(System::Collections::Hashtable^ table) {
	LuaMarshal::MarshalHashTableToStack(this->pState, table);
}

typedef int (*CSLDelegate)(Lua::LuaState^ L);

class CSharpClosure {
public:
	int invoke(lua_State* L) {
		Lua::LuaState^ l = gcnew Lua::LuaState(L, false);
		System::IntPtr ptr = System::IntPtr(static_cast<void*>(this->__funcPtr));
		// This whole thing is dynamic and a pain point... (TODO: Revisit with more optimal solution)
		auto delegate = Marshal::GetDelegateForFunctionPointer(ptr, Lua::LuaFunctionDelegate::typeid);
		return safe_cast<int>(delegate->DynamicInvoke(l));
	}
	void set_func(CSLDelegate ptr) {
		this->__funcPtr = ptr;
	}
private:
	CSLDelegate __funcPtr;
};

int csharp_invoke_luahandle(lua_State* L) {
	
	// Grab delegate information
	CSharpClosure* p = static_cast<CSharpClosure*>(lua_touserdata(L, lua_upvalueindex(1)));

	// Create
	int result = p->invoke(L);

	// Invoke and return
	return result;

}

void Lua::LuaState::PushCSharpFunction(LuaFunctionDelegate^ func) {

	// Grab pointer
	auto managedPointer = Marshal::GetFunctionPointerForDelegate(func);
	CSLDelegate unmanagedDelegate = static_cast<CSLDelegate>(managedPointer.ToPointer());

	// Grab delegate as pointer
	CSharpClosure* closure = static_cast<CSharpClosure*>(lua_newuserdata(this->pState, sizeof(CSharpClosure)));
	closure->set_func(unmanagedDelegate);

	// Keep the delegate alive throughout this procedure
	System::GC::KeepAlive(func);

	// Push closure
	lua_pushcclosure(this->pState, csharp_invoke_luahandle, 1);

}

void Lua::LuaState::PushLightUserdata(System::Object^ obj) {

	// Get the ID for userdata
	uint64_t* userdataId = LuaMarshal::CreateUserdata(obj);

	// Push it as light userdata
	lua_pushlightuserdata(this->pState, userdataId);

}

void Lua::LuaState::NewUserdata(System::Object^ obj) {

	// Get the ID for userdata
	uint64_t* userdataId = LuaMarshal::CreateUserdata(obj);

	// Get the userdata as a pointer
	uint64_t* lValue = static_cast<uint64_t*>(lua_newuserdata(this->pState, sizeof(uint64_t)));

	// Assign value
	*lValue = *userdataId;

}

Lua::LuaTable Lua::LuaState::CreateTable(int arraySize, int dictionarySize) {
	return LuaTable::New(this, arraySize, dictionarySize);
}

Lua::LuaTable Lua::LuaState::CreateTable(System::Collections::Hashtable^ table) {
	throw gcnew System::NotImplementedException();
}

Lua::LuaTable Lua::LuaState::NewMetatable(System::String^ name, [System::Runtime::InteropServices::OutAttribute] bool% alreadyExists) {

	// Get unmanaged string
	__UnmanagedString(strPtr, name, pLStr);

	// Set
	alreadyExists = !luaL_newmetatable(this->pState, pLStr);

	// Free
	__UnmangedFreeString(strPtr);

	// Return from top
	return LuaTable::from_top(this->pState, -1);

}

void Lua::LuaState::SetMetatable(System::String^ name) {

	// Get unmanaged string
	__UnmanagedString(strPtr, name, pLStr);

	// Set the metatable of top element
	luaL_setmetatable(this->pState, pLStr);

	// Free
	__UnmangedFreeString(strPtr);

}

void Lua::LuaState::Pop(int count) {
	lua_pop(this->pState, count);
}

int Lua::LuaState::GetTop() {
	return lua_gettop(this->pState);
}

void Lua::LuaState::SetTop(int top) {
	lua_settop(this->pState, top);
}

void Lua::LuaState::Remove(int index) {
	lua_remove(this->pState, index);
}

void Lua::LuaState::Insert(int index) {
	lua_insert(this->pState, index);
}

void Lua::LuaState::Replace(int index) {
	lua_replace(this->pState, index);
}

void Lua::LuaState::Call(int argc, int retc) {
	lua_call(this->pState, argc, retc);
}

int Lua::LuaState::PCall(int argc, int retc) {
	return lua_pcall(this->pState, argc, retc, 0);
}

Lua::LuaState^ Lua::LuaState::NewState() {
	return NewState(LuaLib::All);
}

Lua::LuaState^ Lua::LuaState::NewState(LuaLib libraries) {

	// Create new lua state
	lua_State* pState = luaL_newstate();
	if (!pState) {
		throw gcnew System::Exception("Failed to create new lua state");
	}

	// Check libs
	if (libraries != LuaLib::None) {

		// Open all libs
		if (libraries == LuaLib::All)
			luaL_openlibs(pState);
		else {

			// Load base
			if (libraries.HasFlag(LuaLib::Base) && !luaopen_base(pState))
				throw gcnew System::Exception("Failed to load base library");

			// Load libraries
			if (libraries.HasFlag(LuaLib::Coroutine) && !luaopen_coroutine(pState))
				throw gcnew System::Exception("Failed to load coroutine library");

			// Load libraries
			if (libraries.HasFlag(LuaLib::Debug) && !luaopen_debug(pState))
				throw gcnew System::Exception("Failed to load debug library");

			// Load libraries
			if (libraries.HasFlag(LuaLib::IO) && !luaopen_io(pState))
				throw gcnew System::Exception("Failed to load IO library");

			// Load libraries
			if (libraries.HasFlag(LuaLib::OS) && !luaopen_os(pState))
				throw gcnew System::Exception("Failed to load OS library");

			// Load libraries
			if (libraries.HasFlag(LuaLib::Math) && !luaopen_math(pState))
				throw gcnew System::Exception("Failed to load math library");

			// Load libraries
			if (libraries.HasFlag(LuaLib::Package) && !luaopen_package(pState))
				throw gcnew System::Exception("Failed to load package library");

			// Load libraries
			if (libraries.HasFlag(LuaLib::String) && !luaopen_string(pState))
				throw gcnew System::Exception("Failed to load string library");

		}

	}

	// Return the new state
	return gcnew LuaState(pState, true);

}
