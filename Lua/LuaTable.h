#pragma once
#include "LuaFunction.hpp"
#include "LuaException.hpp"
#include "LuaMetamethods.hpp"

struct lua_State;

namespace Lua {

	ref class LuaState;

	/// <summary>
	/// Struct representing a table stored on the Lua stack.
	/// </summary>
	/// <remarks>
	/// The instance is only valid within the stack context of the Lua state and should not be stored in a class.
	/// </remarks>
	public value class LuaTable sealed : public System::Collections::IEnumerable {

	public:

		/// <summary>
		/// Struct representing a (key,value) pair in a <see cref="LuaTable"/>.
		/// </summary>
		value class KeyValue {
		private:
			System::Object^ mKey;
			System::Object^ mValue;
		public:
			/// <summary>
			/// Get the key value.
			/// </summary>
			property System::Object^ Key {
				System::Object^ get() { return this->mKey; }
			}
			/// <summary>
			/// Get the value.
			/// </summary>
			property System::Object^ Value {
				System::Object^ get() { return this->mValue; }
			}
			/// <summary>
			/// Initialise a new <see cref="KeyValue"/> instance with a key and value.
			/// </summary>
			KeyValue(System::Object^ k, System::Object^ v) {
				this->mKey = k;
				this->mValue = v;
			}
		};

		/// <summary>
		/// Supports a simple iteration over a <see cref="LuaTable"/> instance.
		/// </summary>
		ref struct Iterator : public System::Collections::Generic::IEnumerator<KeyValue> {
		public:
			Iterator(lua_State* L, int sourceOffset);
			~Iterator();
			// Inherited via IEnumerator
			virtual bool MoveNext();
			virtual void Reset();
			property KeyValue Current {
				virtual KeyValue get() = System::Collections::Generic::IEnumerator<KeyValue>::Current::get{
					return this->kvCurrent;
				}
			}
			property System::Object^ Current2 {
				virtual System::Object^ get() = System::Collections::IEnumerator::Current::get{
					return this->kvCurrent;
				}
			}
		private:
			lua_State* L;
			bool bIsIterating;
			int iSourceStackOffset;
			KeyValue kvCurrent;
		};

	public:

		/// <summary>
		/// Set the value of specified table field.
		/// </summary>
		/// <param name="key">The string identifier for the field.</param>
		/// <param name="value">The string value to set.</param>
		/// <exception cref="LuaTypeExpectedException"/>
		void SetField(System::String^ key, System::String^ value);

		/// <summary>
		/// Set the value of specified table field.
		/// </summary>
		/// <param name="key">The string identifier for the field.</param>
		/// <param name="value">The boolean value to set.</param>
		/// <exception cref="LuaTypeExpectedException"/>
		void SetField(System::String^ key, bool value);

		/// <summary>
		/// Set the value of specified table field.
		/// </summary>
		/// <param name="key">The string identifier for the field.</param>
		/// <param name="value">The integer value to set.</param>
		/// <exception cref="LuaTypeExpectedException"/>
		void SetField(System::String^ key, int value);

		/// <summary>
		/// Set the value of specified table field.
		/// </summary>
		/// <param name="key">The string identifier for the field.</param>
		/// <param name="value">The number value to set.</param>
		/// <exception cref="LuaTypeExpectedException"/>
		void SetField(System::String^ key, double value);

		/// <summary>
		/// Set the field value to the specified CSharp function.
		/// </summary>
		/// <param name="key">The string identifier for the field.</param>
		/// <param name="function">Function delegate to set.</param>
		/// <exception cref="LuaTypeExpectedException"/>
		void SetField(System::String^ key, LuaFunctionDelegate^ function);

		/// <summary>
		/// Set the value of specified table field using the current top value as field value.
		/// </summary>
		/// <param name="key">The string identifier for the field.</param>
		/// <exception cref="LuaTypeExpectedException"/>
		void SetField(System::String^ key);

		/// <summary>
		/// Set the specified table index to refer to the current top stack value.
		/// </summary>
		/// <param name="index">The table index to set.</param>
		/// <exception cref="LuaTypeExpectedException"/>
		void Set(int index);

		/// <summary>
		/// Set a metatable function.
		/// </summary>
		/// <param name="meta">The metamethod to set.</param>
		/// <param name="function">The C# function to invoke when the metamethod is triggered.</param>
		/// <exception cref="LuaTypeExpectedException"/>
		void SetMetatableFunction(LuaMetamethod meta, LuaFunctionDelegate^ function) {
			this->SetField(LuaMetamethods::ToString(meta), function);
		}

		/// <summary>
		/// Get the value of specified table field and pop it immediately from the stack.
		/// </summary>
		/// <param name="key">The string identifier for the field.</param>
		/// <returns>The field value or <see langword="default"/> if nil value or not found.</returns>
		/// <exception cref="LuaRuntimeException"/>
		/// <exception cref="LuaTypeExpectedException"/>
		generic<class T>
		T GetField(System::String^ key) {
			return this->GetField<T>(key, true);
		}

		/// <summary>
		/// Get the value of specified table field and if specified, pop the value from the stack.
		/// </summary>
		/// <param name="key">The string identifier for the field.</param>
		/// <param name="pop">Flag marking if the value should be popped from the stack.</param>
		/// <returns>The field value or <see langword="default"/> if nil value or not found.</returns>
		/// <exception cref="LuaRuntimeException"/>
		/// <exception cref="LuaTypeExpectedException"/>
		generic<class T>
		T GetField(System::String^ key, bool pop);

		/// <summary>
		/// Get the value of specified table field and pop it immediately from the stack.
		/// </summary>
		/// <param name="key">The string identifier for the field.</param>
		/// <returns>The field value or <see langword="null"/> if nil value or not found.</returns>
		/// <exception cref="LuaTypeExpectedException"/>
		System::Object^ GetField(System::String^ key) {
			return this->GetField(key, true);
		}

		/// <summary>
		/// Get the value of specified table field and if specified, pop the value from the stack.
		/// </summary>
		/// <param name="key">The string identifier for the field.</param>
		/// <param name="pop">Flag marking if the value should be popped from the stack.</param>
		/// <returns>The field value or <see langword="null"/> if nil value or not found.</returns>
		/// <exception cref="LuaTypeExpectedException"/>
		System::Object^ GetField(System::String^ key, bool pop);

		/// <summary>
		/// Get the value at the specified table index and pop it from the stack.
		/// </summary>
		/// <param name="index">The table index to get value from.</param>
		/// <returns>A .NET instance representing the stored value or <see langword="null"/> if index is out of bounds.</returns>
		/// <exception cref="LuaTypeExpectedException"/>
		System::Object^ Get(int index) {
			return this->Get(index, true);
		}

		/// <summary>
		/// Get the value at the specified table index and leave the value on the stack if <paramref name="pop"/> is <see langword="false"/>.
		/// </summary>
		/// <param name="index">The table index to get value from.</param>
		/// <param name="pop">Flag to set if the value should be popped from the stack.</param>
		/// <returns>A .NET instance representing the stored value or <see langword="null"/> if index is out of bounds.</returns>
		/// <exception cref="LuaTypeExpectedException"/>
		System::Object^ Get(int index, bool pop);

		/// <summary>
		/// Get the <typeparamref name="T"/> value at the specified index and leave the value on the stack if <paramref name="pop"/> is <see langword="false"/>.
		/// </summary>
		/// <typeparam name="T">The .NET type to return.</typeparam>
		/// <param name="index">The table index to get value from.</param>
		/// <param name="pop">Flag to set if the value should be popped from the stack.</param>
		/// <returns>A <typeparamref name="T"/> instance representing the stored value or <see langword="default"/> if index is out of bounds.</returns>
		/// <exception cref="LuaRuntimeException"/>
		/// <exception cref="LuaTypeExpectedException"/>
		generic<class T>
		T Get(int index, bool pop);

		/// <summary>
		/// Get the <typeparamref name="T"/> value at the specified table index and pop it from the stack.
		/// </summary>
		/// <typeparam name="T">The .NET type to return.</typeparam>
		/// <param name="index">The table index to get value from.</param>
		/// <returns>A <typeparamref name="T"/> instance representing the stored value or <see langword="default"/> if index is out of bounds.</returns>
		/// <exception cref="LuaRuntimeException"/>
		/// <exception cref="LuaTypeExpectedException"/>
		generic<class T>
		T Get(int index) { return this->Get<T>(index, true); }

		// Inherited via IEnumerable
		virtual System::Collections::IEnumerator^ GetEnumerator();

		/// <summary>
		/// Get the table as a <see cref="System::Collections::Hashtable"/> instance.
		/// </summary>
		/// <returns>The <see cref="System::Collections::Hashtable"/> representation of the table.</returns>
		System::Collections::Hashtable^ ToHashtable();

		/// <summary>
		/// Get the amount of ordered entries in the table.
		/// </summary>
		property int Count {
			int get() { return this->iLen; }
		}

		/// <summary>
		/// Get or set the value at the specified table index.
		/// </summary>
		/// <remarks>
		/// Lua-tables are 1-indexed and the first table index is, therefore, 1.
		/// </remarks>
		/// <exception cref="LuaTypeExpectedException"/>
		property System::Object^ default[int]{
			System::Object^ get(int index) { return this->Get(index); }
			void set(int index, System::Object^ value);
		}

		/// <summary>
		/// Get or set the value of the specified table field.
		/// </summary>
		/// <exception cref="LuaTypeExpectedException"/>
		property System::Object^ default[System::String^]{
			System::Object^ get(System::String^ field) { return this->GetField(field); }
			void set(System::String^ field, System::Object^ value);
		}

	public:

		/// <summary>
		/// Create a new <see cref="LuaTable"/> instance and push it into the top of the Lua stack.
		/// </summary>
		/// <param name="L">The state's stack to push table on to.</param>
		/// <param name="arraySize">The amount of elements to make space for in the array section of the table.</param>
		/// <param name="dictionarySize">The amount of elements to make space for in the key/value section of the table.</param>
		/// <returns>A <see cref="LuaTable"/> instance representing the newly pushed stack element.</returns>
		/// <exception cref="System::ArgumentOutOfRangeException"/>
		static LuaTable New(LuaState^ L, int arraySize, int dictionarySize);
		
		/// <summary>
		/// Create a new <see cref="LuaTable"/> instance and push it into the top of the Lua stack.
		/// </summary>
		/// <remarks>
		/// Equivalent to calling <c language="cs">LuaTable.New(L, length, 0);</c>
		/// </remarks>
		/// <param name="L">The state's stack to push table on to.</param>
		/// <param name="length">The expected amount of array elements.</param>
		/// <returns>A <see cref="LuaTable"/> instance representing the newly pushed stack element.</returns>
		/// <exception cref="System::ArgumentOutOfRangeException"/>
		static LuaTable NewArray(LuaState^ L, int length) {
			return New(L, length, 0);
		}

		/// <summary>
		/// Create a new <see cref="LuaTable"/> instance and push it into the top of the Lua stack.
		/// </summary>
		/// <remarks>
		/// Equivalent to calling <c language="cs">LuaTable.New(L, 0, count);</c>
		/// </remarks>
		/// <param name="L">The state's stack to push table on to.</param>
		/// <param name="count">The amount of entries to pre-allocate space for.</param>
		/// <returns>A <see cref="LuaTable"/> instance representing the newly pushed stack element.</returns>
		/// <exception cref="System::ArgumentOutOfRangeException"/>
		static LuaTable NewHashTable(LuaState^ L, int count) {
			return New(L, 0, count);
		}

		/// <summary>
		/// Create a <see cref="LuaTable"/> instance from the stack value located at the specified offset.
		/// </summary>
		/// <param name="L">The state to get table from.</param>
		/// <param name="offset">The offset from the top of the stack. A value of -1 is the stack top.</param>
		/// <returns>A <see cref="LuaTable"/> instance representing the table located at the specified offset.</returns>
		/// <exception cref="LuaTypeExpectedException"/>
		static LuaTable FromStack(LuaState^ L, int offset);

		/// <summary>
		/// Create a <see cref="LuaTable"/> instance from the current top stack value.
		/// </summary>
		/// <remarks>
		/// Equivalent to calling <c language="cs">LuaTable.FromStack(L, -1);</c>
		/// </remarks>
		/// <param name="L">The state to get table from.</param>
		/// <returns>The <see cref="LuaTable"/> currently at the top of the stack.</returns>
		/// <exception cref="LuaTypeExpectedException"/>
		static LuaTable FromTop(LuaState^ L) {
			return FromStack(L, -1);
		}

	private:

		LuaTable(lua_State* state, int offset);

		bool is_table_guard(int offset);

	private:

		lua_State* L;
		unsigned int iLen;
		unsigned int iStackOffset;

	internal:

		static LuaTable from_top(lua_State* L, int offset);

	};

}
