#include "stl_arrays.h"

namespace Micro::STL::Arrays {
	void memcpy(void* __restrict src, void* __restrict dst, Bytes len) noexcept {
		if (!src || !dst) return;

		// Use AVX2, unroll at 128 bytes per cycle
		auto srcB = static_cast<std::byte*>(src);
		auto dstB = static_cast<std::byte*>(dst);
		__m256i a, b, c, d;
		for (size_t i = 0; i < len / 128; i += 128) {
			a = _mm256_load_si256(reinterpret_cast<__m256i const*>(srcB));
			b = _mm256_load_si256(reinterpret_cast<__m256i const*>(srcB + 32));
			c = _mm256_load_si256(reinterpret_cast<__m256i const*>(srcB + 64));
			d = _mm256_load_si256(reinterpret_cast<__m256i const*>(srcB + 96));

			_mm256_store_si256(reinterpret_cast<__m256i*>(dstB), a);
			_mm256_store_si256(reinterpret_cast<__m256i*>(dstB + 32), b);
			_mm256_store_si256(reinterpret_cast<__m256i*>(dstB + 64), c);
			_mm256_store_si256(reinterpret_cast<__m256i*>(dstB + 96), d);

			srcB += 128;
			dstB += 128;
		}
	}

	void memmove(void* src, void* dst, Bytes len) noexcept {
		
	}
}
