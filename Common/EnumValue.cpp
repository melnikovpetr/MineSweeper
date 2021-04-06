#include "EnumValue.h"

#include <memory>
#include <map>

struct EnumValueData
{
public:
	EnumValueData(EnumData* enumData, const EnumValue::Fields& fields);

	void update(const EnumValue::Fields& newFields);

	EnumData* _enumData;
	EnumValue::Fields _fields;
};

struct EnumData
{
	EnumData(const std::string& name, const EnumValue::Fields* begin, const EnumValue::Fields* end);

	void update(const std::string& name, const std::vector<EnumValue::Fields>& fieldsArray);

	std::string _name;
	std::vector<EnumValue::Type<EnumValue::Id> > _ids;
	std::map<EnumValue::Type<EnumValue::Id>, std::unique_ptr<EnumValueData>> _valuesById;
	std::map<EnumValue::Type<EnumValue::Name>, EnumValue::Type<EnumValue::Id> > _idsByName;
};

EnumValueData::EnumValueData(EnumData* enumData, const EnumValue::Fields& fields) : _enumData(enumData), _fields(fields)
{
	assert(_enumData);
	assert(_enumData->_idsByName.find(std::get<EnumValue::Name>(_fields)) == _enumData->_idsByName.end());

	_enumData->_idsByName.insert({
		std::get<EnumValue::Name>(_fields), std::get<EnumValue::Id>(_fields) });
}

void EnumValueData::update(const EnumValue::Fields& newFields)
{
	assert(std::get<EnumValue::Id>(_fields) == std::get<EnumValue::Id>(newFields));
	assert(_enumData);
	if (std::get<EnumValue::Name>(_fields) != std::get<EnumValue::Name>(newFields))
	{
		_enumData->_idsByName.erase(std::get<EnumValue::Name>(_fields));
		_enumData->_idsByName.insert({
			std::get<EnumValue::Name>(newFields), std::get<EnumValue::Id>(_fields) });
	}
	_fields = newFields;
}

EnumData::EnumData(const std::string& name, const EnumValue::Fields* begin, const EnumValue::Fields* end)
{
	_name = name;
	for (auto fields = begin; fields != end; ++fields)
	{
		assert(_valuesById.find(std::get<EnumValue::Id>(*fields)) == _valuesById.end());
		_valuesById.insert(std::make_pair(
			std::get<EnumValue::Id>(*fields), std::make_unique<EnumValueData>(this, *fields)));
	}
	for (const auto& pair : _valuesById) _ids.push_back(pair.first);
}

void EnumData::update(const std::string& name, const std::vector<EnumValue::Fields>& fieldsArray)
{
	_name = name;
	for (auto& fields : fieldsArray)
	{
		assert(_valuesById.find(std::get<EnumValue::Id>(fields)) != _valuesById.end());
		_valuesById[std::get<EnumValue::Id>(fields)]->update(fields);
	}
}

void EnumValue::update(const Fields& newFields)
{
	assert(_data);
	_data->update(newFields);
}

Enum<> EnumValue::enumer() const
{
	return Enum<>(_data->_enumData);
}

const EnumValue::Fields& EnumValue::fields() const
{
	assert(_data);
	return _data->_fields;
}

std::string EnumValue::toString() const
{
	return field<EnumValue::Name>();
}

Enum<>::EnumDataPtr::EnumDataPtr(const std::string& name, const EnumValue::Fields* begin, const EnumValue::Fields* end)
	: ptr(std::make_unique<EnumData>(name, begin, end).release())
{}

Enum<>::EnumDataPtr::~EnumDataPtr() { if (ptr) { delete ptr; ptr = nullptr; } }

const std::string & Enum<void>::name() const
{
	assert(_data);
	return _data->_name;
}

EnumValue Enum<>::value(const EnumValue::Type<EnumValue::Id>& id) const
{
	assert(_data);
	if (_data->_valuesById.find(id) != _data->_valuesById.end())
		return EnumValue(_data->_valuesById[id].get());
	else
		return EnumValue();
}

EnumValue Enum<>::value(const EnumValue::Type<EnumValue::Name>& name) const
{
	assert(_data);
	if (_data->_idsByName.find(name) != _data->_idsByName.end())
		return value(_data->_idsByName[name]);
	else
		return EnumValue();
}

namespace std
{
	bool less<EnumValue>::operator() (const EnumValue& lhs, const EnumValue& rhs) const
	{
		return lhs.field<EnumValue::Id>() < rhs.field<EnumValue::Id>();
	}
}

const std::vector<EnumValue::Type<EnumValue::Id>>& Enum<>::ids() const
{
	assert(_data);
	return _data->_ids;
}

bool Enum<>::contains(const EnumValue& value) const
{
	assert(_data);
	if (value)
		return ((value.enumer()._data == _data)
			&& (_data->_valuesById.find(value.field<EnumValue::Id>()) != _data->_valuesById.end()));
	else
		return false;
}

void Enum<>::update(const std::string& name, const std::vector<EnumValue::Fields>& fieldsArray)
{
	assert(_data);
	_data->update(name, fieldsArray);
}
