#include "HawkUUID.h"

static std::random_device s_RandomDevice;
static std::mt19937_64 s_Engine(s_RandomDevice());
static std::uniform_int_distribution<uint64_t> s_Distribution;

HawkUUID::HawkUUID()
	: m_UUID(GenerateUUID())
{
}

HawkUUID::HawkUUID(uint64_t uuid)
	: m_UUID(uuid)
{
}

std::string HawkUUID::ToString() const {
	std::stringstream ss;
	ss << std::hex << std::setfill('0') << std::setw(16) << m_UUID;
	return ss.str();
}

uint64_t HawkUUID::GenerateUUID() {
	uint64_t id = s_Distribution(s_Engine);

	while (id == 0) {
		id = s_Distribution(s_Engine);
	}

	return id;
}