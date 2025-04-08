#pragma once

#include <cstdint>
#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <functional>

class HawkUUID {
public:
	HawkUUID();

	explicit HawkUUID(uint64_t uuid);

	uint64_t GetValue() const { return m_UUID; }

	std::string ToString() const;

	bool operator==(const HawkUUID& other) const { return m_UUID == other.m_UUID; }
	bool operator!=(const HawkUUID& other) const { return m_UUID != other.m_UUID; }

	bool IsValid() const { return m_UUID != 0; }

	static HawkUUID Invalid() { return HawkUUID(0); }

private:
	uint64_t m_UUID;

	static uint64_t GenerateUUID();
};

namespace std {
	template<>
	struct hash<HawkUUID> {
		std::size_t operator()(const HawkUUID& uuid) const {
			return hash<uint64_t>()(uuid.GetValue());
		}
	};
}