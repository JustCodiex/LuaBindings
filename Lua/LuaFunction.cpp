#include "LuaFunction.hpp"
#include "LuaException.hpp"
#include "LuaState.h"
#include "LuaMarshal.h"

System::Object^ Lua::LuaFunction::Invoke(int resultValueCount, ... array<System::Object^>^ args) {
	
	// Push arguments
	for (int i = 0; i < args->Length; i++) {
		LuaMarshal::MarshalToStack(this->L, args[i]);
	}
	
	// Call
	this->Call(args->Length, resultValueCount);
	
	// Return null if no value was expected, otherwise pop top value
	if (resultValueCount == 0)
		return nullptr;
	else
		return LuaMarshal::MarshalTopStackAndPop(this->L);

}

generic<class T>
T Lua::LuaFunction::Invoke(... array<System::Object^>^ args) {
	
	// Grab type to return (to determine if we need to handle tuple edge case)
	auto tty = T::typeid;

	// Determine if tuple (very basic method) and determine return count (either tuple element count or 1).
	bool isTuple = tty->Namespace == "System" && tty->Name->StartsWith("ValueTuple`");
	int retc = isTuple ? tty->GenericTypeArguments->Length : 1;

	// Push arguments
	for (int i = 0; i < args->Length; i++) {
		LuaMarshal::MarshalToStack(this->L, args[i]);
	}

	// Invoke lua function
	this->Call(args->Length, retc);
	
	// If not a tuple, just pop top element as return value
	if (!isTuple) {
		return safe_cast<T>(LuaMarshal::MarshalTopStackAndPop(this->L));
	}
	
	// Collect values from stack
	array<System::Object^>^ vals = gcnew array<System::Object^>(retc);
	for (int i = 0; i < retc; i++) {
		vals[retc - 1 - i] = LuaMarshal::MarshalTopStackAndPop(this->L);
	}
	
	// Convert to tuple
	return LuaMarshal::ArrayToTuple<T>(vals);

}

bool Lua::LuaFunction::InvokeProtected(int resultValueCount, [System::Runtime::InteropServices::OutAttribute] System::Object^% result, ... array<System::Object^>^ args) {
	for (int i = 0; i < args->Length; i++) {
		LuaMarshal::MarshalToStack(this->L, args[i]);
	}
	if (!this->ProtectedCall(args->Length, resultValueCount)) {
		return false;
	}
	result = LuaMarshal::MarshalStackValue(this->L, -1);
	return true;
}

generic<class T>
bool Lua::LuaFunction::InvokeProtected([System::Runtime::InteropServices::OutAttribute] T% result, ... array<System::Object^>^ args) {
	System::Object^ obj = nullptr;
	if (this->InvokeProtected(1, obj, args)) {
		result = safe_cast<T>(obj);
		return true;
	}
	return false;
}

void Lua::LuaFunction::Call(int argc, int retc) {
	lua_call(this->L, argc, retc);
}

bool Lua::LuaFunction::ProtectedCall(int argc, int retc) {
	return lua_pcall(this->L, argc, retc, 0) == LUA_OK;
}

Lua::LuaFunction Lua::LuaFunction::from_top(lua_State* L, int idx) {
	if (!lua_isfunction(L, idx)) {
		throw gcnew Lua::LuaTypeExpectedException(static_cast<LuaType>(lua_type(L, idx)), LuaType::Function);
	}
	return LuaFunction(L, idx);
}

bool Lua::LuaFunction::IsCallable::get() {
	if (this->iStackOffset != -1) {
		return false;
	}
	return lua_isfunction(this->L, this->iStackOffset);
}

Lua::LuaFunction Lua::LuaFunction::FromTop(LuaState^ L) {
	return from_top(L->get_state(), -1);
}
