#include "stl_arrays.h"

namespace Micro::STL::Arrays {

	// Requires a multiple of 128 size, and assumes that it is aligned
	void memcpy(void* __restrict src, void* __restrict dst, Bytes len) noexcept {
		// Guarantees that this will always be called with 32B aligned memory
		__assume((reinterpret_cast<uintptr_t>(src) & 31) == 0);
		__assume((reinterpret_cast<uintptr_t>(dst) & 31) == 0);

		// Use AVX2, unroll at 128 bytes per cycle
		auto srcB = static_cast<std::byte*>(src);
		auto dstB = static_cast<std::byte*>(dst);
		__m256i a, b, c, d;
		for (size_t i = 0; i < len; i += 128) {
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

	// Only requires len to be multiple of 128
	void memcpyu(void* __restrict src, void* __restrict dst, Bytes len) noexcept{
		// Use AVX2, unroll at 128 bytes per cycle
		auto srcB = static_cast<std::byte*>(src);
		auto dstB = static_cast<std::byte*>(dst);
		__m256i a, b, c, d;
		for (size_t i = 0; i < len; i += 128) {
			a = _mm256_loadu_si256(reinterpret_cast<__m256i const*>(srcB));
			b = _mm256_loadu_si256(reinterpret_cast<__m256i const*>(srcB + 32));
			c = _mm256_loadu_si256(reinterpret_cast<__m256i const*>(srcB + 64));
			d = _mm256_loadu_si256(reinterpret_cast<__m256i const*>(srcB + 96));

			_mm256_storeu_si256(reinterpret_cast<__m256i*>(dstB), a);
			_mm256_storeu_si256(reinterpret_cast<__m256i*>(dstB + 32), b);
			_mm256_storeu_si256(reinterpret_cast<__m256i*>(dstB + 64), c);
			_mm256_storeu_si256(reinterpret_cast<__m256i*>(dstB + 96), d);

			srcB += 128;
			dstB += 128;
		}
	}

	void memcopy(void* src, void* dst, size_t len){
		// Perform alignment and tail checks
		size_t tail = len & 127;
		if((reinterpret_cast<uintptr_t>(src) & 31) == 0 
		&& (reinterpret_cast<uintptr_t>(dst) & 31) == 0 )
			// Fully aligned, let 'em rip
			memcpy(src, dst, len - tail);
		else {
			// No alignment guarantees
			memcpyu(src, dst, len- tail);
		}

		// Now finish tail
		auto* s = static_cast<std::byte*>(src) + (len - tail);
		auto* d = static_cast<std::byte*>(dst) + (len - tail);

		for(size_t i = 0; i < tail; i++) d[i] = s[i];
	}

	void memrevcpy(void* __restrict src, void* __restrict dst, Bytes len) noexcept{
		// Guarantees that this will always be called with 32B aligned memory
		__assume((reinterpret_cast<uintptr_t>(src) & 31) == 0);
		__assume((reinterpret_cast<uintptr_t>(dst) & 31) == 0);

		// Use AVX2, unroll at 128 bytes per cycle
		auto srcB = static_cast<std::byte*>(src) + len;
		auto dstB = static_cast<std::byte*>(dst) + len;		
		__m256i a, b, c, d;
		while (len){
			srcB -= 128;
			dstB -= 128;

			a = _mm256_load_si256(reinterpret_cast<__m256i const*>(srcB));
			b = _mm256_load_si256(reinterpret_cast<__m256i const*>(srcB + 32));
			c = _mm256_load_si256(reinterpret_cast<__m256i const*>(srcB + 64));
			d = _mm256_load_si256(reinterpret_cast<__m256i const*>(srcB + 96));

			_mm256_store_si256(reinterpret_cast<__m256i*>(dstB), a);
			_mm256_store_si256(reinterpret_cast<__m256i*>(dstB + 32), b);
			_mm256_store_si256(reinterpret_cast<__m256i*>(dstB + 64), c);
			_mm256_store_si256(reinterpret_cast<__m256i*>(dstB + 96), d);
			
			len -= 128;
		}
	}

	void memrevcpyu(void* __restrict src, void* __restrict dst, Bytes len) noexcept{
		// Use AVX2, unroll at 128 bytes per cycle
		auto srcB = static_cast<std::byte*>(src) + len;
		auto dstB = static_cast<std::byte*>(dst) + len;		
		__m256i a, b, c, d;
		while (len){
			srcB -= 128;
			dstB -= 128;

			a = _mm256_loadu_si256(reinterpret_cast<__m256i const*>(srcB));
			b = _mm256_loadu_si256(reinterpret_cast<__m256i const*>(srcB + 32));
			c = _mm256_loadu_si256(reinterpret_cast<__m256i const*>(srcB + 64));
			d = _mm256_loadu_si256(reinterpret_cast<__m256i const*>(srcB + 96));

			_mm256_storeu_si256(reinterpret_cast<__m256i*>(dstB), a);
			_mm256_storeu_si256(reinterpret_cast<__m256i*>(dstB + 32), b);
			_mm256_storeu_si256(reinterpret_cast<__m256i*>(dstB + 64), c);
			_mm256_storeu_si256(reinterpret_cast<__m256i*>(dstB + 96), d);
			
			len -= 128;
		}
	}

	void memrevcopy(void* src, void* dst, size_t len){
		// Perform alignment and tail checks
		size_t tail = len & 127;
		if((reinterpret_cast<uintptr_t>(src) & 31) == 0 
		&& (reinterpret_cast<uintptr_t>(dst) & 31) == 0 )
			// Fully aligned, let 'em rip
			memrevcpy(src, dst, len - tail);
		else {
			// No alignment guarantees
			memrevcpyu(src, dst, len- tail);
		}

		// Now finish tail
		auto* s = static_cast<std::byte*>(src);
		auto* d = static_cast<std::byte*>(dst);
		if(!tail) return;
		for (size_t i = tail; i-- > 0; ) d[i] = s[i];
	}

	void memmove(void* __restrict src, void* __restrict dst, Bytes len) noexcept {
		auto* s = static_cast<std::byte*>(src);
		auto* d = static_cast<std::byte*>(dst);
		
		if(d < s || d >= s+ len) memcopy(src, dst, len);
		else memrevcopy(src, dst, len);
	}
}
