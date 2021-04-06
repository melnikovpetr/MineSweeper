#ifndef SETTINGS_H
#define SETTINGS_H

#include "Value.h"

#include <mutex>
#include <functional>

struct Validator
{
	enum Attributes : size_t
	{
		Type,
		Constraint,
	};

	enum Types : size_t
	{
		IntRange,
		DoubleRange,
		RegExp,
	};

	enum IntRangeAttrs : size_t
	{
		IntRangeMin,
		IntRangeMax,
	};

	enum DoubleRangeAttrs : size_t
	{
		DoubleRangeMin,
		DoubleRangeMax,
		DoubleRangeDecimals,
	};

	static Value intRange(int min, int max);
	static Value doubleRange(double min, double max, int decimals);
	static Value regExp(const std::string& regExpStr);
};

class Param
{
	friend class Settings;

public:
	Param(
		const EnumValue& enumValue,
		const Value& value,
		const Value& validator = {} )
		: _enumValue(enumValue), _validator(validator), _value(value), _settings(nullptr)
	{}

	const EnumValue& enumValue() const { return _enumValue; }
	const EnumValue::Type<EnumValue::Id>& id() const { return _enumValue.field<EnumValue::Id>(); }
	const EnumValue::Type<EnumValue::Name>& name() const { return _enumValue.field<EnumValue::Name>(); }
	const EnumValue::Type<EnumValue::Desc>& desc() const { return _enumValue.field<EnumValue::Desc>(); }

	const Value& validator() const { return _validator; }

	Value::TypeIds valueTypeId() const;
	Value value() const;
	void setValue(const Value& value);

private:
	void setSettings(class Settings* settings) { _settings = settings; }

private:
	const EnumValue _enumValue;
	const Value _validator;

	Value _value;

	class Settings* _settings;
};

class Settings
{
public:
	typedef std::map<size_t, Value> ParamValues;
	typedef std::function< void(const Param& param) > SaveParam;
	typedef std::function< void(Param& param) > LoadParam;

public:
	Settings(std::initializer_list<Param> params);

	auto makeLock() const { return std::unique_lock<decltype(_mutex)>(_mutex); }
	void lock() const { _mutex.lock(); }
	bool try_lock() const { return _mutex.try_lock(); }
	void unlock() const { _mutex.unlock(); }

	const std::vector<size_t>& paramIds() const { return _paramIds; }
	
	const Param& param(size_t id) const { return _params.at(id); }
	Param& param(size_t id) { return _params.at(id); }
	
	const Param& param(const EnumValue& enumValue) const;
	Param& param(const EnumValue& enumValue);

	ParamValues paramValues() const;

	void save(const SaveParam& saveParam) const;
	void load(const LoadParam& saveParam);

private:
	mutable std::recursive_mutex _mutex;

	std::vector<size_t> _paramIds;
	std::map<size_t, Param> _params;
};

#endif // SETTINGS_H
