#include "TessHexagon.h"

#include "MineSweeper.h"

#include "../Common/FP.h"

#define RADIUS(index) (index)[RadiusIndex].castTo<Value::typeIds<size_t>>()
#define ALFA(index) (index)[AlfaIndex].castTo<Value::typeIds<size_t>>()

#define assert_r_a(r,a) assert(((r) < radiusCount()) && ((a) < countOnRadius(r)))
#define assert_index(index) assert_r_a(RADIUS(index),ALFA(index))

// Count of neighbor poligons
static constexpr size_t poligonEdges = 6;
static constexpr size_t poligonEdgesHalf = poligonEdges / 2;

size_t TessHexagon::count(size_t radiusCount)
{
	return radiusCount ? 1 + poligonEdges * (radiusCount * (radiusCount - 1) / 2) : 0;
}

size_t TessHexagon::countOnRadius(size_t radius)
{
	return radius ? radius * poligonEdges : 1;
}

const Point2D& TessHexagon::prototypeSize()
{
	static const Point2D protosize_{{ Coord{500}, Coord{866} /* x*sqrt(3) */ }};
	return protosize_;
}

const Polygon& TessHexagon::prototype()
{
	static const Polygon prototype_ = Polygon{
		{{          prototypeSize()(C2D::X),  prototypeSize()(C2D::Y) }},
		{{ Coord(2)*prototypeSize()(C2D::X),                 Coord(0) }},
		{{          prototypeSize()(C2D::X), -prototypeSize()(C2D::Y) }},
		{{         -prototypeSize()(C2D::X), -prototypeSize()(C2D::Y) }},
		{{-Coord(2)*prototypeSize()(C2D::X),                 Coord(0) }},
		{{         -prototypeSize()(C2D::X),  prototypeSize()(C2D::Y) }}
	}.calculateMetriks();
	return prototype_;
}

TessHexagon::TessHexagon(const Value& size) : Tessellation(size) {}

size_t TessHexagon::radiusCount() const
{
	return size()[EnumValue{ IGame::RadiusCount }].castTo<Value::typeIds<size_t>>();
}

Value TessHexagon::makeIndex(size_t r, size_t a) const
{
	assert_r_a(r, a);
	return Value{ r, a };
}

size_t TessHexagon::nextAlfa(const Value& index) const
{
	assert_index(index);
	auto result = ALFA(index) + 1;
	return result < countOnRadius(RADIUS(index)) ? result : 0;
}

size_t TessHexagon::prevAlfa(const Value& index) const
{
	assert_index(index);
	return ALFA(index) ? ALFA(index) - 1 : countOnRadius(RADIUS(index)) - 1;
}

Value TessHexagon::beginIndex() const
{
	return Value{ size_t{0}, size_t{0} };
}

Value TessHexagon::endIndex() const
{
	return Value{ radiusCount(), size_t{0} };
}

Value TessHexagon::nextIndex(const Value& index) const
{
	assert_index(index);
	if (auto nextA = nextAlfa(index)) return { index[RadiusIndex], nextA };
	else return{ RADIUS(index) + 1, nextA };
}

Value TessHexagon::prevIndex(const Value& index) const
{
	assert_index(index);
	if (auto a = ALFA(index)) return { index[RadiusIndex], a };
	return makeIndex(RADIUS(index) - 1, 0);
}

std::vector<Value> TessHexagon::neighbors(const Value& index) const
{
	assert_index(index);
	std::vector<Value> neighbors_{};
	neighbors_.reserve(poligonEdges);

	if (auto r = RADIUS(index))
	{
		auto a_r = ALFA(index);
		auto a_perc = Coord(a_r) / Coord(countOnRadius(r));

		// Previous radius
		auto rm1 = r - 1;
		auto a_rm1 = size_t(a_perc * Coord(countOnRadius(rm1)));

		// Next radius
		auto rp1 = r + 1;
		auto a_rp1 = size_t(a_perc * Coord(countOnRadius(rp1)));

		// Check if index is angular
		if (!(a_r % r))
		{
			neighbors_.push_back(makeIndex(rm1, a_rm1));
			neighbors_.push_back(makeIndex(r, prevAlfa(index)));
			neighbors_.push_back(makeIndex(r, nextAlfa(index)));

			if (rp1 < radiusCount())
			{
				auto index_rp1_a_rp1 = makeIndex(rp1, a_rp1);
				neighbors_.push_back(makeIndex(rp1, prevAlfa(index_rp1_a_rp1)));
				neighbors_.push_back(index_rp1_a_rp1);
				neighbors_.push_back(makeIndex(rp1, nextAlfa(index_rp1_a_rp1)));
			}
		}
		else
		{
			neighbors_.push_back(makeIndex(rm1, a_rm1));
			neighbors_.push_back(makeIndex(rm1, nextAlfa(neighbors_.back())));

			neighbors_.push_back(makeIndex(r, prevAlfa(index)));
			neighbors_.push_back(makeIndex(r, nextAlfa(index)));

			if (rp1 < radiusCount())
			{
				neighbors_.push_back(makeIndex(rp1, size_t(a_rp1)));
				neighbors_.push_back(makeIndex(rp1, nextAlfa(neighbors_.back())));
			}
		}
	}
	// The first index
	else
	{
		auto rp1 = r + 1;
		for (size_t a_rp1 = 0; a_rp1 < countOnRadius(rp1); ++a_rp1)
			neighbors_.push_back(makeIndex(rp1, a_rp1));
	}
	return neighbors_;
}

Value TessHexagon::index(size_t crossCuttingIndex) const
{
	Value index{ beginIndex() };
	// crossCuttingIndex = Sr = s0 + s1 + s2,..., sr = (r + 1)(2*s0 + d*r)
	// Root of the quadratic equation
	auto r_fp = -Coord(0.5) + Coord(0.5)*sqrt(Coord(1) + Coord(4) * Coord(crossCuttingIndex) / Coord(poligonEdgesHalf));
	auto r_fp_rnd = std::round(r_fp);
	if (FP::equal(r_fp, r_fp_rnd)) RADIUS(index) = size_t(r_fp_rnd);
	else RADIUS(index) = size_t(r_fp + 1);
	ALFA(index) = crossCuttingIndex - count(RADIUS(index));
	return index;
}

Value TessHexagon::index(const Point2D& point) const
{
	auto atan = std::atan2(point(C2D::X), point(C2D::Y));
	auto alfa = atan < 0 ? atan  + Coord{2} * Coord(M_PI) : atan;
	auto edgeNumber = size_t(alfa / (Coord(M_PI) / Coord(poligonEdgesHalf)));
	auto r_min = size_t(std::cos(std::abs(alfa - Coord(M_PI)*(Coord(2)*edgeNumber + 1) / Coord(poligonEdges)))
		* point.magnitude() / (Coord(3)*prototypeSize()(C2D::X)));
	auto a_min = size_t(point.applyToCopy(Transform2D::rotate(-Coord(edgeNumber) * (Coord(M_PI)	/ Coord(poligonEdgesHalf))))(C2D::X) 
			/ (Coord(3)*prototypeSize()(C2D::X)));

	for (auto r = r_min; (r <= (r_min + 1)) && (r < radiusCount()); ++r)
	{
		auto a = a_min + r*edgeNumber;
		if (a == countOnRadius(r)) a = 0;
		Value result = makeIndex(r, a);
		if (polygon(result).hasPoint(point)) return result;
		result = makeIndex(r, nextAlfa(result));
		if (polygon(result).hasPoint(point)) return result;
	}
	return {};
}

size_t TessHexagon::crossCuttingIndex(const Value& index) const
{
	assert_index(index);
	return count(RADIUS(index)) + ALFA(index);
}

size_t TessHexagon::count() const
{
	return count(radiusCount());
}

Point2D TessHexagon::point(const Value& index) const
{
	assert_index(index);

	if (RADIUS(index))
	{
		auto r = RADIUS(index);
		auto a = ALFA(index);
		auto r_fp_max = prototypeSize()(C2D::Y) * Coord{2} *Coord(r);
		Point2D center{{
			Coord(a % r) * Coord{3} * prototypeSize()(C2D::X),
			r_fp_max - Coord(a % r) * prototypeSize()(C2D::Y) }};
		return center.apply(Transform2D::rotate(Coord(a / r) * (Coord(M_PI) / Coord(poligonEdgesHalf))));
	}
	return {};
}

Polygon TessHexagon::polygon(const Value& index) const
{
	return (index == endIndex()) ? Polygon{} : prototype().applyToCopy(Transform2D::move(point(index)));
}

Point2D TessHexagon::polygonSize() const
{
	return prototypeSize();
}
