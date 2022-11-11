#include "LuaMarshal.h"
#include "LuaTable.h"
#include "LuaState.h"
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
		case Lua::LuaType::UserData: {
			uint64_t* ptr = static_cast<uint64_t*>(lua_touserdata(L, idx));
			return GetUserdata(*ptr);
		}
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

System::Object^ Lua::LuaMarshal::GetUserdata(uint64_t dataId) {
	
	// Check if key is contained
	if (__identifierToUserdata->ContainsKey(dataId))
		return __identifierToUserdata[dataId];

	// Return null
	return nullptr;

}

uint64_t* Lua::LuaMarshal::CreateUserdata(System::Object^ obj) {
	
	// Get map
	std::map<uint64_t, uint64_t*> ptrmap = *__managedPointers;

	// Return if already contained
	if (__userdataToIdentifier->ContainsKey(obj)) {
		uint64_t k = safe_cast<uint64_t>(__userdataToIdentifier[obj]);;
		uint64_t* ptr = ptrmap[k];
		return ptr;
	}

	// Grab identifier
	uint64_t identifier = __userdataIdentifierCounter++;
	
	// Register
	__userdataToIdentifier->Add(obj, identifier);
	__identifierToUserdata->Add(identifier, obj);

	// Store
	uint64_t* ptr = new uint64_t(identifier);
	ptrmap[identifier] = ptr;

	// Return identifer
	return ptr;

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

void Lua::LuaMarshal::CreateCSharpLuaFunction(lua_State* L, LuaFunctionDelegate^ delegate) {

	// Grab pointer
	auto managedPointer = Marshal::GetFunctionPointerForDelegate(delegate);
	CSLDelegate unmanagedDelegate = static_cast<CSLDelegate>(managedPointer.ToPointer());

	// Grab delegate as pointer
	CSharpClosure* closure = static_cast<CSharpClosure*>(lua_newuserdata(L, sizeof(CSharpClosure)));
	closure->set_func(unmanagedDelegate);

	// Keep the delegate alive throughout this procedure
	System::GC::KeepAlive(delegate);

	// Push closure
	lua_pushcclosure(L, csharp_invoke_luahandle, 1);

}

Lua::LuaType Lua::LuaMarshal::ToLuaType(lua_State* L, int idx) {
	return static_cast<LuaType>(lua_type(L, idx));
}
