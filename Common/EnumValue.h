#ifndef ENUM_VALUE_H
#define ENUM_VALUE_H

#include <cassert>
#include <vector>
#include <tuple>

struct EnumValueData;
struct EnumData;

template<typename TEnum = void>
class Enum;

class EnumValue
{
public:
	enum FieldNames : size_t { Id, Name, Desc, _FieldCount };
	
	typedef std::tuple<size_t, std::string, std::string> Fields;
	
	static_assert(_FieldCount == std::tuple_size<Fields>::value, "Check EnumValue fields!");

	template<FieldNames FieldName>
	using Type = typename std::tuple_element<FieldName, Fields>::type;

public:
	template<typename TEnum, typename = std::enable_if_t<std::is_enum<TEnum>::value>>
	explicit EnumValue(TEnum id) : EnumValue(Enum<TEnum>().value(id)._data) { assert(_data); }
	explicit EnumValue(EnumValueData* data = nullptr) : _data(data) {}
	
	const Fields& fields() const;
	std::string toString() const;
	void update(const Fields& newFields);
	explicit operator bool() const { return _data != nullptr; }
	bool operator ==(const EnumValue& other) const { return _data && (_data == other._data); }
	Enum<> enumer() const;

	template<FieldNames FieldName>
	const Type<FieldName>& field() const { assert(_data); return std::get<FieldName>(fields()); }

private:
	EnumValueData* _data;
};

namespace std { template<> struct less<EnumValue> { bool operator() (const EnumValue& lhs, const EnumValue& rhs) const; }; }

template<>
class Enum<void>
{
protected:
	struct EnumDataPtr
	{
		EnumDataPtr(const std::string& name, const EnumValue::Fields* begin, const EnumValue::Fields* end);
		~EnumDataPtr();
		EnumData* ptr;
	};

public:
	Enum(EnumData* data = nullptr) : _data(data) {}
	const std::string& name() const;
	EnumValue value(const EnumValue::Type<EnumValue::Id>& id) const;
	EnumValue value(const EnumValue::Type<EnumValue::Name>& name) const;
	const std::vector<EnumValue::Type<EnumValue::Id>>& ids() const;
	bool contains(const EnumValue& value) const;
	explicit operator bool() const { return _data != nullptr; }
	bool operator ==(const Enum<>& other) const { return _data && (_data == other._data); }
	void update(const std::string& name, const std::vector<EnumValue::Fields>& fieldsArray);

protected:
	void setData(EnumData* data) { _data = data; }

private:
	EnumData* _data;
};

template<typename TEnum>
class Enum : public Enum<>
{
	static_assert(std::is_enum<TEnum>::value, "Check TEnum!");
	static_assert(std::is_same<std::underlying_type_t<TEnum>, EnumValue::Type<EnumValue::Id>>::value, "Check TEnum underlying type!");

public:
	Enum();
	using Enum<>::value;
	EnumValue value(TEnum id) const { return value(EnumValue::Type<EnumValue::Id>(id)); }

private:
	struct Fields : public EnumValue::Fields
	{
		template<typename... Args> Fields(TEnum id, Args&& ...rest) : EnumValue::Fields(size_t{ id }, std::forward<Args>(rest)...) {}
	};
	struct EnumDataPtr : public Enum<void>::EnumDataPtr
	{
		EnumDataPtr(const std::string& name, std::initializer_list<Fields> fieldsArray)
			: Enum<>::EnumDataPtr(name,	fieldsArray.begin(), fieldsArray.end())
		{}
	};
};

#define ENUM_NAMES(TEnum, ...) template<> Enum<TEnum>::Enum() { static EnumDataPtr data{#TEnum,__VA_ARGS__}; setData(data.ptr); }

#endif // ENUM_VALUE_H
