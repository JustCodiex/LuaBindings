#include "LuaTable.h"
#include "LuaState.h"
#include "LuaMarshal.h"
#include "CLIMacros.h"
#include "lua/ltable.hpp"

Lua::LuaTable::Iterator::Iterator(lua_State* L) {
    this->L = L;
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
    if (lua_next(this->L, -2) != 0) {

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

Lua::LuaTable::LuaTable(lua_State*  L) {
    this->L = L;
    this->iLen = static_cast<int>(lua_rawlen(this->L, -1));
}

bool Lua::LuaTable::is_table_guard(int offset) {
    return lua_istable(this->L, offset);
}

void Lua::LuaTable::SetField(System::String^ key, System::String^ value) {

    // Ensure table
    if (!this->is_table_guard(-1))
        throw gcnew System::Exception("");

    // Grab C strings
    __UnmanagedString(kP, key, pKey);
    __UnmanagedString(kV, value, pValue);

    // Push value
    lua_pushstring(this->L, pValue);

    // Set field
    lua_setfield(this->L, -2, pKey);

    // Free
    __UnmangedFreeString(kP);
    __UnmangedFreeString(kV);

}

void Lua::LuaTable::SetField(System::String^ key, bool value) {

    // Ensure table
    if (!this->is_table_guard(-1))
        throw gcnew System::Exception("");

    // Grab key
    __UnmanagedString(kP, key, pKey);

    // Push value
    lua_pushboolean(this->L, value);

    // Set field
    lua_setfield(this->L, -2, pKey);

    // Free key
    __UnmangedFreeString(kP);

}

void Lua::LuaTable::SetField(System::String^ key, int value) {

    // Ensure table
    if (!this->is_table_guard(-1))
        throw gcnew System::Exception("");

    // Grab key
    __UnmanagedString(kP, key, pKey);

    // Push value
    lua_pushinteger(this->L, static_cast<lua_Integer>(value));

    // Set field
    lua_setfield(this->L, -2, pKey);

    // Free key
    __UnmangedFreeString(kP);

}

void Lua::LuaTable::SetField(System::String^ key, double value) {

    // Ensure table
    if (!this->is_table_guard(-1))
        throw gcnew System::Exception("");

    // Grab key
    __UnmanagedString(kP, key, pKey);

    // Push value
    lua_pushnumber(this->L, value);

    // Set field
    lua_setfield(this->L, -2, pKey);

    // Free key
    __UnmangedFreeString(kP);

}

void Lua::LuaTable::SetField(System::String^ key) {

    // Ensure table
    if (!this->is_table_guard(-2))
        throw gcnew System::Exception("");

    // Grab key
    __UnmanagedString(kP, key, pKey);

    // Set field
    lua_setfield(this->L, -2, pKey);

    // Free key
    __UnmangedFreeString(kP);

}

System::Collections::IEnumerator^ Lua::LuaTable::GetEnumerator() {

    // Ensure table
    if (!this->is_table_guard(-1))
        throw gcnew System::Exception("");

    // Return
    return gcnew LuaTable::Iterator(this->L);

}

System::Collections::Hashtable^ Lua::LuaTable::ToHashtable() {
    throw gcnew System::NotImplementedException();
}

Lua::LuaTable^ Lua::LuaTable::New(LuaState^ L, int arraySize, int dictionarySize) {
    lua_State* s = L->get_state();
    lua_createtable(s, arraySize, dictionarySize);
    return gcnew LuaTable(s);
}

Lua::LuaTable^ Lua::LuaTable::FromTop(LuaState^ L) {
    return from_top(L->get_state());
}

Lua::LuaTable^ Lua::LuaTable::from_top(lua_State* L) {
    if (lua_istable(L, -1)) {
        return gcnew LuaTable(L);
    } else {
        throw gcnew System::Exception("top element is not a table element.");
    }
}

System::Object^ Lua::LuaTable::GetField(System::String^ key, bool popValue) {

    // Ensure table
    if (!this->is_table_guard(-1))
        throw gcnew System::Exception("");

    // Grab key
    __UnmanagedString(kP, key, pKey);

    // Get the field
    lua_getfield(this->L, -1, pKey);

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

    // Ensure table
    if (!this->is_table_guard(-1))
        throw gcnew System::Exception("");

    // Grab key
    __UnmanagedString(kP, key, pKey);

    // Get the field
    lua_getfield(this->L, -1, pKey);

    // Free key
    __UnmangedFreeString(kP);

    // Grab val
    System::Object^ fldVal = LuaMarshal::MarshalStackValue(this->L, -1);

    // Pop top
    if (popValue) {
        lua_pop(this->L, 1);
    }

    // Return
    return safe_cast<T>(fldVal);

}
