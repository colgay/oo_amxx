#ifndef HASH_POLICIES_H
#define HASH_POLICIES_H

#include <amtl/am-string.h>
#include <amtl/am-hashtable.h>

namespace ke
{
	struct HashStringPolicy
	{
		static inline uint32_t hash(const char* key) {
			return ke::FastHashCharSequence(key, strlen(key));
		}
		static inline bool matches(const char* find, const ke::AString& key) {
			return key.compare(find) == 0;
		}
	};

	struct HashIntegerPolicy
	{
		static inline uint32_t hash(uint32_t key) {
			return ke::HashInteger<4>(key);
		}
		static inline bool matches(uint32_t find, uint32_t key) {
			return key == find;
		}
	};
}

#endif // HASH_POLICIES_H