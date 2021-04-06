#include "Value.h"

#include <algorithm>

Value Value::fromString(Value::TypeIds typeId, const Type<Value::String>& string)
{
	SWITCH_TYPE_ID(typeId, return fromString<TYPE_ID>(string));
	return{}; /*vc++ warning*/
}

Value::Value(std::initializer_list<Type<MapStr>::value_type> list) : Value{Type<MapStr>(list)}
{}

Value::Value(std::initializer_list<Type<MapEnum>::value_type> list) : Value{Type<MapEnum>(list)}
{}

Value::Value(const Value& value) : _typeId(value._typeId)
{
	if (_typeId != Value::Unknown)
	{
		SWITCH_TYPE_ID(_typeId, Operation<Type<TYPE_ID>>::create(_data, value.castTo<TYPE_ID>()));
	}
}

Value::Value(Value&& value) : _typeId(value._typeId)
{
	if (_typeId != Value::Unknown) 
	{
		std::copy(value._data, value._data + dataSize, _data);
		value._typeId = Value::Unknown;
	}
}

Value::~Value()
{
	if (_typeId != Value::Unknown)
	{
		SWITCH_TYPE_ID(_typeId, Operation<Type<TYPE_ID>>::clear(_data));
		_typeId = Value::Unknown;
	}
}

Value& Value::operator =(std::initializer_list<Type<MapStr>::value_type> list)
{
	return operator =(Value{list});
}

Value& Value::operator =(std::initializer_list<Type<MapEnum>::value_type> list)
{
	return operator =(Value{list});
}

Value& Value::operator =(const Value& value)
{
	if (_typeId != value._typeId)
	{
		this->~Value();
		new (this) Value(value);
	}
	else
	{
		if ((this != &value) && (_typeId != Value::Unknown))
		{
			SWITCH_TYPE_ID(_typeId, castTo<TYPE_ID>() = value.castTo<TYPE_ID>());
		}
	}
	return *this;
}

Value& Value::operator =(Value&& value)
{
	if (this != &value)
	{
		this->~Value();
		new (this) Value(std::move(value));
	}
	return *this;
}

bool Value::operator ==(const Value& value) const
{
	auto result = false;
	if (_typeId == value._typeId)
	{
		SWITCH_TYPE_ID(_typeId, return castTo<TYPE_ID>() == value.castTo<TYPE_ID>());
	}
	return result;
}

size_t Value::size() const
{
	size_t result = 0;

	if (_typeId != Value::Unknown)
	{
		SWITCH_TYPE_ID(_typeId, result = Operation<Type<TYPE_ID>>::size(_data, int{}));
	}

	return result;
}

bool Value::empty() const
{
	auto result = true;

	if (_typeId != Value::Unknown)
	{
		SWITCH_TYPE_ID(_typeId, result = Operation<Type<TYPE_ID>>::empty(_data, int{}));
	}

	return result;
}

Value::Type<Value::String> Value::toString() const
{
	if (_typeId != Value::Unknown)
	{
		SWITCH_TYPE_ID(_typeId, return Operation<Type<TYPE_ID>>::toString(_data, int{}));
		return Type<String>();
	}
	else return Type<String>();
}
