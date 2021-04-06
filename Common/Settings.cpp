#include "Settings.h"

ENUM_NAMES(Validator::Types, {
	{ Validator::IntRange,    "Integer range",        "" },
	{ Validator::DoubleRange, "Floating point range", "" },
	{ Validator::RegExp,      "Regular expression",   "" } } )

Value Validator::intRange(int min, int max)
{
	return Value{ EnumValue{ IntRange }, Value{ min, max } };
}

Value Validator::doubleRange(double min, double max, int decimals)
{
	return Value{ EnumValue{ DoubleRange }, Value{ min, max, decimals } };
}

Value Validator::regExp(const std::string& regExpStr)
{
	return Value{ EnumValue{ RegExp }, regExpStr };
}

Value::TypeIds Param::valueTypeId() const
{
	auto lock = _settings ? _settings->makeLock() : decltype(_settings->makeLock()){};
	return _value.typeId();
}

Value Param::value() const
{
	auto lock = _settings ? _settings->makeLock() : decltype(_settings->makeLock()){};
	return _value;
}

void Param::setValue(const Value& value)
{
	auto lock = _settings ? _settings->makeLock() : decltype(_settings->makeLock()){};
	assert(_value.typeId() == value.typeId());
	_value = value;
}

Settings::Settings(std::initializer_list<Param> params) : _mutex()
{
	_paramIds.reserve(params.size());
	for (auto& param : params)
	{
		assert(_params.find(param.id()) == _params.end());
		_params.insert({ param.id(), param});
		_paramIds.push_back(param.id());
	}
}

const Param& Settings::param(const EnumValue& enumValue) const
{
	assert(_params.begin()->second.enumValue().enumer().contains(enumValue));
	return _params.at(enumValue.field<EnumValue::Id>());
}

Param& Settings::param(const EnumValue& enumValue)
{
	assert(_params.begin()->second.enumValue().enumer().contains(enumValue));
	return _params.at(enumValue.field<EnumValue::Id>());
}

Settings::ParamValues Settings::paramValues() const
{
	auto lock = makeLock();
	ParamValues paramValues{};
	for (const auto& param : _params) paramValues[param.first] = param.second.value();
	return paramValues;
}

void Settings::save(const SaveParam& saveParam) const
{
	auto lock = makeLock();
	for (auto& param : _params) saveParam(param.second);
}

void Settings::load(const LoadParam& saveParam)
{
	auto lock = makeLock();
	for (auto& param : _params) saveParam(param.second);
}
