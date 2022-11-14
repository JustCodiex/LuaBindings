#include "LuaTable.h"
#include "LuaState.h"
#include "LuaMarshal.h"
#include "CLIMacros.hpp"
#include "LuaException.hpp"
#include "lua/ltable.hpp"

#define __TABLEGUARD(idx) if (!this->is_table_guard(idx)) { throw gcnew Lua::LuaTypeExpectedException(static_cast<LuaType>(lua_type(this->L, idx)), LuaType::Table); }

Lua::LuaTable::Iterator::Iterator(lua_State* L, int sourceOffset) {
    this->L = L;
    this->iSourceStackOffset = sourceOffset;
}

Lua::LuaTable::Iterator::~Iterator() {
}

bool Lua::LuaTable::Iterator::MoveNext() {
    
    // Push nil to being (if not started)
    if (!this->bIsIterating) {
        lua_pushnil(this->L);
        this->bIsIterating = true;
    }

    // get next
    if (lua_next(this->L, this->iSourceStackOffset - 1) != 0) {

        // Read value
        System::Object^ v = LuaMarshal::MarshalStackValue(this->L, -1);
        System::Object^ k = LuaMarshal::MarshalStackValue(this->L, -2);

        // Pop value
        lua_pop(this->L, 1);

        // Set val
        this->kvCurrent = KeyValue(k, v);

        // Return OK
        return true;

    }

    // Reset is iterating
    this->bIsIterating = false;

    // Return false, nothing new to move top
    return false;

}

void Lua::LuaTable::Iterator::Reset() {
    throw gcnew System::NotSupportedException();
}

Lua::LuaTable::LuaTable(lua_State*  L, int offset) {
    this->L = L;
    this->iStackOffset = offset;
    this->iLen = static_cast<int>(lua_rawlen(this->L, this->iStackOffset));
}

bool Lua::LuaTable::is_table_guard(int offset) {
    return lua_istable(this->L, offset);
}

void Lua::LuaTable::SetField(System::String^ key, System::String^ value) {

    // Ensure table
    __TABLEGUARD(this->iStackOffset);

    // Grab C strings
    __UnmanagedString(kP, key, pKey);
    __UnmanagedString(kV, value, pValue);

    // Push value
    lua_pushstring(this->L, pValue);

    // Set field
    lua_setfield(this->L, this->iStackOffset - 1, pKey);

    // Free
    __UnmangedFreeString(kP);
    __UnmangedFreeString(kV);

}

void Lua::LuaTable::SetField(System::String^ key, bool value) {

    // Ensure table
    __TABLEGUARD(this->iStackOffset);

    // Grab key
    __UnmanagedString(kP, key, pKey);

    // Push value
    lua_pushboolean(this->L, value);

    // Set field
    lua_setfield(this->L, this->iStackOffset - 1, pKey);

    // Free key
    __UnmangedFreeString(kP);

}

void Lua::LuaTable::SetField(System::String^ key, int value) {

    // Ensure table
    __TABLEGUARD(this->iStackOffset);

    // Grab key
    __UnmanagedString(kP, key, pKey);

    // Push value
    lua_pushinteger(this->L, static_cast<lua_Integer>(value));

    // Set field
    lua_setfield(this->L, this->iStackOffset - 1, pKey);

    // Free key
    __UnmangedFreeString(kP);

}

void Lua::LuaTable::SetField(System::String^ key, double value) {

    // Ensure table
    __TABLEGUARD(this->iStackOffset);

    // Grab key
    __UnmanagedString(kP, key, pKey);

    // Push value
    lua_pushnumber(this->L, value);

    // Set field
    lua_setfield(this->L, this->iStackOffset - 1, pKey);

    // Free key
    __UnmangedFreeString(kP);

}

void Lua::LuaTable::SetField(System::String^ key, LuaFunctionDelegate^ function) {

    // Ensure table
    __TABLEGUARD(this->iStackOffset);

    // Grab key
    __UnmanagedString(kP, key, pKey);

    // Push delegate
    LuaMarshal::CreateCSharpLuaFunction(this->L, function);

    // Set field
    lua_setfield(this->L, this->iStackOffset - 1, pKey);

    // Free key
    __UnmangedFreeString(kP);

}

void Lua::LuaTable::SetField(System::String^ key) {

    // Ensure table
    __TABLEGUARD(this->iStackOffset - 1);

    // Grab key
    __UnmanagedString(kP, key, pKey);

    // Set field
    lua_setfield(this->L, this->iStackOffset - 1, pKey);

    // Free key
    __UnmangedFreeString(kP);

}

System::Collections::IEnumerator^ Lua::LuaTable::GetEnumerator() {

    // Ensure table
    __TABLEGUARD(this->iStackOffset);

    // Return
    return gcnew LuaTable::Iterator(this->L, this->iStackOffset);

}

System::Collections::Hashtable^ Lua::LuaTable::ToHashtable() {
    
    // Create table
    System::Collections::Hashtable^ t = gcnew System::Collections::Hashtable(this->iLen);

    // Iterate over entries
    auto itt = this->GetEnumerator();
    while (itt->MoveNext()) {

        // Grab current
        KeyValue kv = safe_cast<KeyValue>(itt->Current);
        
        // Add
        t->Add(kv.Key, kv.Value);

    }

    // Return
    return t;

}

System::Object^ Lua::LuaTable::GetField(System::String^ key, bool popValue) {

    // Ensure table
    __TABLEGUARD(this->iStackOffset);

    // Grab key
    __UnmanagedString(kP, key, pKey);

    // Get the field
    lua_getfield(this->L, this->iStackOffset, pKey);

    // Free key
    __UnmangedFreeString(kP);

    // Grab val
    System::Object^ fldVal = LuaMarshal::MarshalStackValue(this->L, -1);

    // Pop top
    if (popValue) {
        lua_pop(this->L, 1);
    }

    // Return
    return fldVal;

}

generic <class T>
T Lua::LuaTable::GetField(System::String^ key, bool popValue) {
    System::Object^ v = this->GetField(key, false);
    T t = T();
    try {
        t = safe_cast<T>(v);
    } catch (System::InvalidCastException^ icex) {
        System::String^ err = System::String::Format("Lua field value type {0} cannot be cast to managed type {1}.", LuaMarshal::ToLuaType(this->L, -1), T::typeid->FullName);
        throw gcnew LuaRuntimeException(err, icex);
    }
    if (popValue) {
        LuaState::pop_generic_safe(this->L, 1);
    }
    return t;
}

System::Object^ Lua::LuaTable::Get(int index, bool pop) {

    // Ensure table
    __TABLEGUARD(this->iStackOffset);

    // Get the index
    lua_pushnumber(this->L, index);
    lua_gettable(this->L, this->iStackOffset - 1);

    // Grab val
    System::Object^ fldVal = LuaMarshal::MarshalStackValue(this->L, -1);

    // Pop top
    if (pop) {
        lua_pop(this->L, 1);
    }

    // Return
    return fldVal;

}

generic<class T>
T Lua::LuaTable::Get(int index, bool pop) {
    System::Object^ v = this->Get(index, false);
    T t = T();
    try {
        t = safe_cast<T>(v);
    } catch (System::InvalidCastException^ icex) {
        System::String^ err = System::String::Format("Lua index value type {0} cannot be cast to managed type {1}.", LuaMarshal::ToLuaType(this->L, -1), T::typeid->FullName);
        throw gcnew LuaRuntimeException(err, icex);
    }
    if (pop) {
        LuaState::pop_generic_safe(this->L, 1);
    }
    return t;
}

void Lua::LuaTable::Set(int index) {

    // Ensure table
    __TABLEGUARD(this->iStackOffset - 1);

    // Push index
    lua_pushnumber(this->L, index);

    // Swap key/value so value is on top
    lua_insert(this->L, -2);

    // Set it
    lua_settable(this->L, this->iStackOffset - 2);

}

void Lua::LuaTable::default::set(int index, System::Object^ value) {
    __TABLEGUARD(this->iStackOffset);
    LuaMarshal::MarshalToStack(this->L, value);
    this->Set(index);
}

void Lua::LuaTable::default::set(System::String^ field, System::Object^ value) {
    __TABLEGUARD(this->iStackOffset);
    LuaMarshal::MarshalToStack(this->L, value);
    this->SetField(field);
}

Lua::LuaTable Lua::LuaTable::New(LuaState^ L, int arraySize, int dictionarySize) {
    if (arraySize < 0)
        throw gcnew System::ArgumentOutOfRangeException("arraySize", System::String::Format("A value of {0} is not a valid array size. Value must be greater than or equal to 0.", arraySize));
    if (dictionarySize < 0)
        throw gcnew System::ArgumentOutOfRangeException("dictionarySize", System::String::Format("A value of {0} is not a valid dictionary size. Value must be greater than or equal to 0.", dictionarySize));
    lua_State* s = L->get_state();
    lua_createtable(s, arraySize, dictionarySize);
    return LuaTable(s, -1);
}

Lua::LuaTable Lua::LuaTable::FromStack(LuaState^ L, int offset) {
    return from_top(L->get_state(), offset);
}

Lua::LuaTable Lua::LuaTable::from_top(lua_State* L, int offset) {
    if (!lua_istable(L, offset)) {
        throw gcnew Lua::LuaTypeExpectedException(static_cast<LuaType>(lua_type(L, offset)), LuaType::Table);
    }
    return LuaTable(L, offset);
}
