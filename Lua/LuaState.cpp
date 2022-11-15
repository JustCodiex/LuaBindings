#include "LuaState.h"
#include "LuaStateAux.h"
#include "LuaTable.h"
#include "LuaMarshal.h"
#include "LuaException.hpp"
#include "CLIMacros.hpp"
#include <stdlib.h>
#include <vector>

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

Lua::CallResult Lua::LuaState::LoadString(System::String^ lStr) {

	// Grab C++ string
	__UnmanagedString(strPtr, lStr, pLStr);

	// Invoke
	int result = luaL_loadstring(this->pState, pLStr);

	// Free unmanaged string
	__UnmangedFreeString(strPtr);

	// Return if success
	return static_cast<CallResult>(result);

}

Lua::CallResult Lua::LuaState::LoadFile(System::String^ filepath) {

	// Grab C++ string
	__UnmanagedString(strPtr, filepath, pLStr);

	// Invoke
	int result = luaL_loadfile(this->pState, pLStr);

	// Free unmanaged string
	__UnmangedFreeString(strPtr);

	// Return if success
	return static_cast<CallResult>(result);

}

Lua::CallResult Lua::LuaState::LoadStream(System::IO::Stream^ stream, System::String^ chunkname) {

	// Grab C++ chunk name string
	__UnmanagedString(strPtr, chunkname, pLStr);

	// Create reader
	auto reader = gcnew System::IO::BinaryReader(stream);

	// Handle bytes
	array<unsigned  char>^ data = reader->ReadBytes(static_cast<int>(stream->Length));

	// Grab pinned ptr
	pin_ptr<unsigned char> pinnedPtr = &data[0];
	const char* pData = reinterpret_cast<const char*>(pinnedPtr);

	// Invoke load
	int result = luaL_loadbuffer(this->pState, pData, static_cast<int>(stream->Length), pLStr);

	// Free unmanaged string
	__UnmangedFreeString(strPtr);

	// Return if success
	return static_cast<CallResult>(result);

}

#pragma managed(push, off)
struct csharp_dumpbuffer {
	unsigned char* data;
	size_t size;
};
#pragma managed(pop)

const char* csharp_luadumpreader(lua_State* L, void* data, size_t* sz) {

	// Get buffer
	csharp_dumpbuffer* buf = static_cast<csharp_dumpbuffer*>(data);

	// Make space for data
	char* result = new char[buf->size];

	// Copy from buffer
	memcpy_s(result, buf->size, buf->data, buf->size);
	
	// Return result
	return result;

}

Lua::CallResult Lua::LuaState::Load(array<unsigned char>^ buffer, System::String^ chunkname) {

	// Grab C++ chunk name string
	__UnmanagedString(strPtr, chunkname, pLStr);

	// Get pinned
	pin_ptr<unsigned char> pinnedPtr = &buffer[0];

	// Grab a pointer
	csharp_dumpbuffer buf{};
	buf.data = static_cast<unsigned char*>(pinnedPtr);
	buf.size = buffer->Length;

	// Invoke load
	int result = lua_load(this->pState, csharp_luadumpreader, static_cast<void*>(&buf), pLStr, 0);

	// Free unmanaged string
	__UnmangedFreeString(strPtr);

	// Return if success
	return static_cast<CallResult>(result);

}

Lua::CallResult Lua::LuaState::DoString(System::String^ lStr) {

	// Grab C++ string
	__UnmanagedString(strPtr, lStr, pLStr);

	// Invoke
	int result = luaL_dostring(this->pState, pLStr);

	// Free unmanaged string
	__UnmangedFreeString(strPtr);

	// Return if success
	return static_cast<CallResult>(result);

}

Lua::CallResult Lua::LuaState::DoFile(System::String^ filepath) {

	// Grab C++ string
	__UnmanagedString(strPtr, filepath, pLStr);

	// Invoke
	int result = luaL_dofile(this->pState, pLStr);

	// Free unmanaged string
	__UnmangedFreeString(strPtr);

	// Return if success
	return static_cast<CallResult>(result);

}

Lua::LuaType Lua::LuaState::Type(int index) {
	return static_cast<Lua::LuaType>(lua_type(this->pState, index));
}

System::String^ Lua::LuaState::Typename(int index) {

	// Grab the typename in C string form
	const char* pStr = lua_typename(this->pState, index);

	// Return a managed version
	return Marshal::PtrToStringAnsi(static_cast<System::IntPtr>(const_cast<char*>(pStr)));

}

double Lua::LuaState::GetNumber(int index) {
	return lua_tonumber(this->pState, index);
}

bool Lua::LuaState::GetBoolean(int index) {
	return lua_toboolean(this->pState, index);
}

int64_t Lua::LuaState::GetInteger(int index) {
	return lua_tointeger(this->pState, index);
}

System::String^ Lua::LuaState::GetString(int index) {

	// Get C string
	const char* pStr = lua_tostring(this->pState, index);

	// Get 
	return Marshal::PtrToStringAnsi(static_cast<System::IntPtr>(const_cast<char*>(pStr)));

}

System::Object^ Lua::LuaState::GetUserdata(int index) {

	// Get userdata
	uint64_t* userdataPtr = static_cast<uint64_t*>(lua_touserdata(this->pState, index));

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

void Lua::LuaState::PushCSharpFunction(LuaFunctionDelegate^ func) {
	LuaMarshal::CreateCSharpLuaFunction(this->pState, func);
}

void Lua::LuaState::PushLightUserdata(System::Object^ obj) {

	// Get the ID for userdata
	uint64_t* userdataId = LuaMarshal::CreateUserdata(obj);

	// Push it as light userdata
	lua_pushlightuserdata(this->pState, userdataId);

}

System::Object^ Lua::LuaState::NewUserdata(System::Type^ userdataType) {

	// Get the userdata as a pointer
	uint64_t* lValue = static_cast<uint64_t*>(lua_newuserdata(this->pState, sizeof(uint64_t)));

	// Create object
	System::Object^ obj = System::Activator::CreateInstance(userdataType);

	// Assign value
	*lValue = *LuaMarshal::CreateUserdata(obj);

	// Return
	return obj;

}

Lua::LuaTable Lua::LuaState::CreateTable(int arraySize, int dictionarySize) {
	return LuaTable::New(this, arraySize, dictionarySize);
}

Lua::LuaTable Lua::LuaState::CreateTable(System::Collections::Hashtable^ table) {
	LuaMarshal::MarshalHashTableToStack(this->pState, table);
	return LuaTable::from_top(this->pState, -1);
}

Lua::LuaTable Lua::LuaState::CreateTable(System::Collections::IList^ list) {
	LuaMarshal::MarshalListToStack(this->pState, list);
	return LuaTable::from_top(this->pState, -1);
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

Lua::CallResult Lua::LuaState::PCall(int argc, int retc, int errfunc) {
	return static_cast<CallResult>(lua_pcall(this->pState, argc, retc, errfunc));
}

int Lua::LuaState::GC(GarbageCollectWhat what, int data) {
	return lua_gc(this->pState, static_cast<int>(what), data);
}

void Lua::LuaState::Error() {
	lua_error(this->pState);
}

int Lua::LuaState::Yield(int results) {
	return lua_yield(this->pState, results);
}

void Lua::LuaState::Concat(int n) {
	lua_concat(this->pState, n);
}

Lua::LuaState^ Lua::LuaState::NewState() {
	return NewState(LuaLib::All);
}

int csharp_luadumpwriter(lua_State* L, const void* P, size_t sz, void* up) {

	// Grab uffer
	csharp_dumpbuffer* buf = static_cast<csharp_dumpbuffer*>(up);

	// Reintrepret data
	const unsigned char* data = reinterpret_cast<const unsigned char*>(static_cast<const char*>(P));
	
	// Grab new content
	unsigned char* content = new unsigned char[buf->size + sz];
	
	// Fill existing content
	if (buf->data) {
		memcpy_s(content, buf->size + sz, buf->data, buf->size);
		delete buf->data;
	}

	// Add our own
	memcpy_s(content + buf->size, sz, data, sz);

	// Update
	buf->data = content;
	buf->size += sz;

	// Return 0 (OK)
	return LUA_OK;

}

Lua::CallResult Lua::LuaState::Dump([System::Runtime::InteropServices::OutAttribute] array<unsigned char>^% buffer) {
	
	// Verify not C/C# function
	if (lua_iscfunction(this->pState, -1))
		throw gcnew LuaRuntimeException("Cannot dump a C or C# function.");

	// Verify is function
	if (!lua_isfunction(this->pState, -1))
		throw gcnew LuaTypeExpectedException(static_cast<LuaType>(lua_type(this->pState, -1)), LuaType::Function);

	csharp_dumpbuffer dmpBuf{};
	int result = lua_dump(this->pState, csharp_luadumpwriter, &dmpBuf, 0);
	if (result != LUA_OK)
		return static_cast<CallResult>(result);

	// Create buffer
	buffer = gcnew array<unsigned char>(static_cast<int>(dmpBuf.size));

	// Copy contents
	System::Runtime::InteropServices::Marshal::Copy(System::IntPtr(dmpBuf.data), buffer, 0, static_cast<int>(dmpBuf.size));

	// Free
	delete dmpBuf.data;

	// Return OK
	return CallResult::Ok;

}

int csharp_luapanic(lua_State* L) {
	throw gcnew Lua::LuaRuntimeException(Lua::LuaMarshal::MarshalStackValue(L, -1)->ToString());
	return 0;
}

Lua::LuaState^ Lua::LuaState::NewState(LuaLib libraries) {

	// Create new lua state
	lua_State* pState = luaL_newstate();
	if (!pState) {
		throw gcnew System::Exception("Failed to create new lua state");
	}

	// Set panic
	lua_atpanic(pState, csharp_luapanic);

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

void Lua::LuaState::pop_generic_safe(lua_State* L, int amount) {
	lua_pop(L, amount);
}
