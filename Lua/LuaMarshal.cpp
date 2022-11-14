#include "LuaMarshal.h"
#include "LuaTable.h"
#include "LuaState.h"
#include "LuaFunction.hpp"
#include "lua/luabind.hpp"
#include "CLIMacros.hpp"

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
		case Lua::LuaType::LightUserData: {
			uint64_t* uData = static_cast<uint64_t*>(lua_touserdata(L, idx));
			return GetUserdata(*uData);
		}
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
	} else if (ty->GetInterface("IList") != nullptr) {
		MarshalListToStack(L, safe_cast<System::Collections::IList^>(obj));
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

void Lua::LuaMarshal::MarshalListToStack(lua_State* L, System::Collections::IList^ list) {

	// Grab count
	int count = list->Count;

	// Push table
	lua_createtable(L, count, 0);

	// Loop over and set
	for (int i = 0; i < count; i++) {

		// Push N
		lua_pushnumber(L, i + 1);

		// Push value
		MarshalToStack(L, list[i]);

		// Set the table by index
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

	try {

		// Create
		int result = p->invoke(L);

		// Invoke and return
		return result;

	} catch (System::Exception^ ex) {

		// Push error
		__UnmanagedString(exPtr, ex->Message, pExStr);

		// Push
		lua_pushstring(L, pExStr);

		// Free
		__UnmangedFreeString(exPtr);

		// Err
		lua_error(L);

		// Return naught
		return 0;

	}

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

using namespace System::Reflection::Emit;

void LoadArgument(ILGenerator^ il, int i, LocalBuilder^ l) {
	il->Emit(OpCodes::Ldarg_0); // Load LuaState^
	il->Emit(OpCodes::Ldc_I4, -i); // Load lua stack index onto the stack
	il->EmitCall(OpCodes::Call, Lua::LuaMarshal::__netStackMarshal, nullptr); // Call lua marshal
	
	// Unbox from Object type if value type
	if (l->LocalType->IsValueType) {
		il->Emit(OpCodes::Unbox_Any, l->LocalType);
	} else {
		il->Emit(OpCodes::Castclass, l->LocalType);
	}
	
	il->Emit(OpCodes::Stloc, l); // Store in local

}

Lua::LuaFunctionDelegate^ Lua::LuaMarshal::CreateLuaDelegate(System::Reflection::MethodInfo^ method) {

	// Grab static flag
	auto isStatic = method->IsStatic;

	// Get dynamic method ctor args
	auto returnId = System::Int32::typeid;
	auto params = gcnew array<System::Type^>(1) { LuaState::typeid };
	auto dynamicName = System::String::Format("lua_{0}", method->Name->ToLowerInvariant());

	// Grab argument count
	auto methodParams = method->GetParameters();

	// Create the dynamic method
	auto dMethod = gcnew DynamicMethod(dynamicName, returnId, params);
	auto body = dMethod->GetILGenerator();

	// Prepare var for argument offset
	int argCount = methodParams->Length + (isStatic ? 0 : 1);

	// Prepare locals
	auto locals = gcnew array<LocalBuilder^>(argCount);

	// Demarshal stack values
	for (int i = 0; i < argCount; i++) {
		
		// Hande local
		int pi = isStatic ? i : (i - 1);
		auto locTy = (i == 0 && !isStatic) ? method->DeclaringType : methodParams[pi]->ParameterType;
		locals[i] = body->DeclareLocal(locTy);

		// Load the argument
		LoadArgument(body, argCount - i, locals[i]);

	}

	// Arg offset tracker
	int locOffset = 0;

	// Emit caller (null if static)
	if (!isStatic) {
		body->Emit(OpCodes::Ldloc, locals[0]);
		locOffset++;
	}

	// Push args
	for (int i = 0; i < methodParams->Length; i++) {
		body->Emit(OpCodes::Ldloc, locals[locOffset + i]);
	}

	// Emit call to method
	body->EmitCall(OpCodes::Callvirt, method, nullptr);

	// If void, push 0
	if (method->ReturnType == void::typeid) {
		body->Emit(OpCodes::Ldc_I4_0);
	} else { // TODO: Add support to tuple types

		// Store it in a local
		auto returnValLoc = body->DeclareLocal(System::Object::typeid);
		if (method->ReturnType->IsValueType) {
			body->Emit(OpCodes::Box, method->ReturnType);
		}
		body->Emit(OpCodes::Stloc, returnValLoc);

		// Load L
		body->Emit(OpCodes::Ldarg_0);

		// Load result
		body->Emit(OpCodes::Ldloc, returnValLoc);

		// Call marshal
		body->EmitCall(OpCodes::Call, __netToStackMarshal, nullptr);

		// Push one onto the stack as return value
		body->Emit(OpCodes::Ldc_I4_1);
	
	}

	// Push return instruction
	body->Emit(OpCodes::Ret);

	// Return delegate
	return safe_cast<LuaFunctionDelegate^>(dMethod->CreateDelegate(LuaFunctionDelegate::typeid));

}
