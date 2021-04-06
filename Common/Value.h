#ifndef VALUE_H
#define VALUE_H

#include "Color.h"
#include "EnumValue.h"
#include "Rect.h"

#include <type_traits>
#include <typeinfo>

#include <sstream>
#include <map>

class Value
{
public:
	static const size_t dataSize = sizeof(double);

public:
	enum TypeIds : size_t
	{
		Unknown,
		Bool,
		UInt,
		Int,
		ULongLong,
		LongLong,
		Float,
		Double,
		EnumValue,
		Color,
		Size2D,
		Point2D,
		Rect,
		String,
		Vector,
		MapStr,
		MapEnum,
		_Count
	};

private:
	template<TypeIds id, bool _partial_ = true> struct TypeById;
	template<typename T, bool _partial_ = true> struct IdByType;

#define BIND_ID_WITH_TYPE(id,...) \
	template<bool _partial_> struct TypeById<id, _partial_> { typedef __VA_ARGS__ Type; }; \
	template<bool _partial_> struct IdByType<__VA_ARGS__, _partial_> { static constexpr TypeIds typeId = id; };

	BIND_ID_WITH_TYPE(Bool,      bool)
	BIND_ID_WITH_TYPE(UInt,      unsigned)
	BIND_ID_WITH_TYPE(Int,       int)
#ifdef _WIN32
	BIND_ID_WITH_TYPE(ULongLong, unsigned long long)
	BIND_ID_WITH_TYPE(LongLong,  long long)
#else
	BIND_ID_WITH_TYPE(ULongLong, long unsigned)
	BIND_ID_WITH_TYPE(LongLong,  long int)
#endif
	BIND_ID_WITH_TYPE(Float,     float)
	BIND_ID_WITH_TYPE(Double,    double)
	BIND_ID_WITH_TYPE(EnumValue, ::EnumValue)
	BIND_ID_WITH_TYPE(Color,     ::Color)
	BIND_ID_WITH_TYPE(Size2D,    ::Size2D)
	BIND_ID_WITH_TYPE(Point2D,   ::Point2D)
	BIND_ID_WITH_TYPE(Rect,      ::Rect)
	BIND_ID_WITH_TYPE(String,    std::string)
	BIND_ID_WITH_TYPE(Vector,    std::vector<Value>)
	BIND_ID_WITH_TYPE(MapStr,    std::map<TypeById<String>::Type, Value>)
	BIND_ID_WITH_TYPE(MapEnum,   std::map<::EnumValue, Value>)

#undef BIND_ID_WITH_TYPE

#define CASE_TYPE_ID(id,...) case id: { constexpr auto TYPE_ID = id; __VA_ARGS__; } break;
#define SWITCH_TYPE_ID(id,...) switch(id) { \
	CASE_TYPE_ID(Bool,      __VA_ARGS__) \
	CASE_TYPE_ID(UInt,      __VA_ARGS__) \
	CASE_TYPE_ID(Int,       __VA_ARGS__) \
	CASE_TYPE_ID(ULongLong, __VA_ARGS__) \
	CASE_TYPE_ID(LongLong,  __VA_ARGS__) \
	CASE_TYPE_ID(Float,     __VA_ARGS__) \
	CASE_TYPE_ID(Double,    __VA_ARGS__) \
	CASE_TYPE_ID(EnumValue, __VA_ARGS__) \
	CASE_TYPE_ID(Color,     __VA_ARGS__) \
	CASE_TYPE_ID(Size2D,    __VA_ARGS__) \
	CASE_TYPE_ID(Point2D,   __VA_ARGS__) \
	CASE_TYPE_ID(Rect,      __VA_ARGS__) \
	CASE_TYPE_ID(String,    __VA_ARGS__) \
	CASE_TYPE_ID(Vector,    __VA_ARGS__) \
	CASE_TYPE_ID(MapStr,    __VA_ARGS__) \
	CASE_TYPE_ID(MapEnum,   __VA_ARGS__) \
	default : assert(false); }

public:
	template<TypeIds id> using Type = typename TypeById<id>::Type;
	template<typename T> static constexpr TypeIds typeIds = IdByType<T>::typeId;

private:
	template<typename T, typename TRet = void> using IfSmall = std::enable_if_t<(sizeof(T) <= dataSize), TRet>;
	template<typename T, typename TRet = void> using IfBig =   std::enable_if_t<(sizeof(T)  > dataSize), TRet>;
	template<typename T1, typename T2, typename T3 = T1> using IfSameThen = std::enable_if_t<std::is_same<T1, T2>::value, T3>;

	template<typename T>
	struct Operation
	{
		template<typename TT = T> static IfBig  <TT, const T*> ptr(const uint8_t* data) { return *(T**)data; }
		template<typename TT = T> static IfBig  <TT,       T*> ptr(      uint8_t* data) { return *(T**)data; }
		template<typename TT = T> static IfSmall<TT, const T*> ptr(const uint8_t* data) { return (T*)data; }
		template<typename TT = T> static IfSmall<TT,       T*> ptr(      uint8_t* data) { return (T*)data; }

		template<typename TT = T> static IfBig  <TT> clear(uint8_t* data) { delete ptr(data); }
		template<typename TT = T> static IfSmall<TT> clear(uint8_t* data) { if (std::is_class<T>::value) ptr(data)->~T(); }

		template<typename TT = T> static IfBig  <TT> create(uint8_t* data, TT&& source) { *(T**)data = new T(std::forward<TT>(source)); }
		template<typename TT = T> static IfSmall<TT> create(uint8_t* data, TT&& source) { new(data) T(std::forward<TT>(source)); }

		static const T& value(const uint8_t* data) { return *ptr(data); }
		static       T& value(      uint8_t* data) { return *ptr(data); }

		template<typename TT = T> static auto empty(const uint8_t* data, int)->decltype(Operation<TT>::value(data).empty())
		{ return value(data).empty(); }
		template<typename TT = T> static auto empty(const uint8_t* data, ...)
		{ return false; }

		template<typename TT = T> static auto size(const uint8_t* data, int)->decltype(Operation<TT>::value(data).size())
		{ return value(data).size(); }
		template<typename TT = T> static auto size(const uint8_t* data, ...)
		{ return size_t{}; }

		template<typename TT = T> static auto toString(const uint8_t* data, int)->IfSameThen<TT, Type<String>>
		{ return value(data); }
		template<typename TT = T> static auto toString(const uint8_t* data, int)->IfSameThen<TT, Type<EnumValue>, Type<String>>
		{ return value(data).toString(); }
		template<typename TT = T> static auto toString(const uint8_t* data, int)->decltype(std::to_string(Operation<TT>::value(data)), Type<String>{})
		{ return std::to_string(value(data)); }
		static auto toString(const uint8_t*, ...) { return Type<String>{}; }

		template<typename TT = T>
		static auto fromString(uint8_t* data, const std::string& s, int)->decltype(std::istringstream(s).operator>>(Operation<TT>::value(data)), void())
		{ std::istringstream(s) >> value(data); }
		static auto fromString(uint8_t*, const std::string&, ...)->void { assert(false); }

		template<typename TT = T, typename TIndex>
		static auto at(const uint8_t* data, const TIndex& index, int)->IfSameThen<const Value&, decltype(Operation<TT>::value(data).at(index))>
		{ return value(data).at(index); }
		static auto at(const uint8_t*, ...)->const Value& { throw std::bad_cast{}; }

		template<typename TT = T, typename TIndex>
		static auto at(uint8_t* data, const TIndex& index, int)->IfSameThen<Value&, decltype(Operation<TT>::value(data).at(index))>
		{ return value(data).at(index); }
		static auto at(uint8_t*, ...)->Value& { throw std::bad_cast{}; }
	};

public:
	template<TypeIds typeId>
	static Value fromString(const Type<String>& string)
	{
		Value result{};
		Operation<Type<typeId>>::fromString(result._data, string, int{});
		result._typeId = typeId;
		return result;
	}

	static Value fromString(TypeIds typeId, const Type<String>& string);

public:
	Value() : _typeId{Value::Unknown} {}
	Value(const Value& value);
	Value(Value&& value);
	Value(std::initializer_list<Type<MapStr>::value_type> list);
	Value(std::initializer_list<Type<MapEnum>::value_type> list);

	template<typename T, typename TT = std::decay_t<T>, typename = std::enable_if_t<!std::is_same<TT, Value>::value>>
	explicit Value(T&& value) : _typeId(typeIds<TT>) { Operation<TT>::create(_data, std::forward<T>(value)); }

	template<typename ...Ts, typename = std::enable_if_t<(sizeof...(Ts) > 1)>>
	Value(Ts&& ...values) : Value(Type<Vector>{ Value(std::forward<Ts>(values))... }) {}

	~Value();

	TypeIds typeId() const { return _typeId; }

	template<TypeIds typeId>
	Value::Type<typeId>& castTo()
	{
		assert(_typeId == typeId);
		return Operation<Type<typeId>>::value(_data);
	}

	template<TypeIds typeId>
	const Type<typeId>& castTo() const
	{
		assert(_typeId == typeId);
		return Operation<Type<typeId>>::value(_data);
	}

	Value& operator =(const Value& value);
	Value& operator =(Value&& value);

	template<typename T, typename TT = std::decay_t<T>, typename = std::enable_if_t<!std::is_same<TT, Value>::value>>
	Value& operator =(T&& value)
	{
		if (_typeId != typeIds<TT>)
		{
			this->~Value();
			new (this) Value( std::forward<T>(value) );
		}
		else
		{
			castTo<typeIds<TT>>() = std::forward<T>(value);
		}
		return *this;
	}

	Value& operator =(std::initializer_list<Type<MapStr>::value_type> list);
	Value& operator =(std::initializer_list<Type<MapEnum>::value_type> list);

	bool operator ==(const Value& value) const;
	template<typename T>
	bool operator ==(const T& value) const { return castTo<typeIds<T>>() == value; }
	bool operator !=(const Value& value) const { return !(*this == value); }
	template<typename T>
	bool operator !=(const T& value) const { return !(*this == value); }

	template<typename TIndex>
	const Value& operator [] (const TIndex& index) const
	{ SWITCH_TYPE_ID(_typeId, return Operation<Type<TYPE_ID>>::at(_data, index, int{})); throw std::bad_cast{}; }
	template<typename TIndex>
	Value& operator [] (const TIndex& index)
	{ SWITCH_TYPE_ID(_typeId, return Operation<Type<TYPE_ID>>::at(_data, index, int{})); throw std::bad_cast{};	}

	size_t size() const;
	bool empty() const;
	Type<String> toString() const;

private:
	TypeIds _typeId;
	uint8_t _data[dataSize];
};

#endif // VALUE_H
