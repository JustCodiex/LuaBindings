#pragma once

struct lua_State;

namespace Lua {

	ref class LuaState;

	/// <summary>
	/// 
	/// </summary>
	public ref class LuaTable sealed : public System::Collections::IEnumerable {

	public:

		/// <summary>
		/// 
		/// </summary>
		value class KeyValue {
		private:
			System::Object^ mKey;
			System::Object^ mValue;
		public:
			property System::Object^ Key {
				System::Object^ get() { return this->mKey; }
			}
			property System::Object^ Value {
				System::Object^ get() { return this->mValue; }
			}
			KeyValue(System::Object^ k, System::Object^ v) {
				this->mKey = k;
				this->mValue = v;
			}
		};

		/// <summary>
		/// 
		/// </summary>
		ref struct Iterator : public System::Collections::Generic::IEnumerator<KeyValue> {
		public:
			Iterator(lua_State* L);
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
			KeyValue kvCurrent;
		};

	public:

		/// <summary>
		/// Set the value of specified table field.
		/// </summary>
		/// <param name="key">The string identifier for the field.</param>
		/// <param name="value">The string value to set.</param>
		void SetField(System::String^ key, System::String^ value);

		/// <summary>
		/// Set the value of specified table field.
		/// </summary>
		/// <param name="key">The string identifier for the field.</param>
		/// <param name="value">The boolean value to set.</param>
		void SetField(System::String^ key, bool value);

		/// <summary>
		/// Set the value of specified table field.
		/// </summary>
		/// <param name="key">The string identifier for the field.</param>
		/// <param name="value">The integer value to set.</param>
		void SetField(System::String^ key, int value);

		/// <summary>
		/// Set the value of specified table field.
		/// </summary>
		/// <param name="key">The string identifier for the field.</param>
		/// <param name="value">The number value to set.</param>
		void SetField(System::String^ key, double value);

		/// <summary>
		/// Set the value of specified table field using the current top value as field value.
		/// </summary>
		/// <param name="key">The string identifier for the field.</param>
		void SetField(System::String^ key);

		generic<class T>
		T GetField(System::String^ key) {
			return this->GetField<T>(key, true);
		}

		generic<class T>
		T GetField(System::String^ key, bool pop);

		System::Object^ GetField(System::String^ key) {
			return this->GetField(key, true);
		}

		System::Object^ GetField(System::String^ key, bool pop);

		// Inherited via IEnumerable
		virtual System::Collections::IEnumerator^ GetEnumerator();

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		System::Collections::Hashtable^ ToHashtable();

		/// <summary>
		/// Get the amount of ordered entries in the table.
		/// </summary>
		property int Count {
			int get() { return this->iLen; }
		}

	public:

		/// <summary>
		/// 
		/// </summary>
		/// <param name="L"></param>
		/// <param name="arraySize"></param>
		/// <param name="dictionarySize"></param>
		/// <returns></returns>
		static LuaTable^ New(LuaState^ L, int arraySize, int dictionarySize);
		
		/// <summary>
		/// 
		/// </summary>
		/// <param name="L"></param>
		/// <param name="length"></param>
		/// <returns></returns>
		static LuaTable^ NewArray(LuaState^ L, int length) {
			return New(L, length, 0);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="L"></param>
		/// <param name="count"></param>
		/// <returns></returns>
		static LuaTable^ NewHashTable(LuaState^ L, int count) {
			return New(L, 0, count);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="L"></param>
		/// <returns></returns>
		static LuaTable^ FromTop(LuaState^ L);

	private:

		LuaTable(lua_State* state);

		bool is_table_guard(int offset);

	private:

		lua_State* L;
		unsigned int iLen;

	internal:

		static LuaTable^ from_top(lua_State* L);

	};

}
