// Basic macro layer

#ifndef ALIGNAS
#define ALIGNAS(align) alignas(align)
#endif

#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif

#ifndef NODISCARD
#define NODISCARD [[nodiscard]]
#endif

#ifndef MAYBE_UNUSED
#define MAYBE_UNUSED [[maybe_unused]]
#endif

#ifndef STATIC_ASSERT
#define STATIC_ASSERT(cond, msg) static_assert(cond, msg)
#endif

#ifndef MOVE_DEFAULTS
#define MOVE_DEFAULTS(name)				      \
name(name&&) noexcept = default;		  \
name& operator=(name&&) noexcept = default;
#endif

#ifndef COPY_DEFAULTS
#define COPY_DEFAULTS(name)					  \
name(const name&) = default;			  \
name& operator=(const name&) = default;
#endif

#ifndef ABSTRACT
#define ABSTRACT = 0
#endif

// includes
#include <bit>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <type_traits>

namespace Micro::STL::Arrays {
	// General Type defs
	using Bytes = size_t;
	using Pvoid = void*;

	static constexpr Bytes PAGE_SIZE = 4ULL * 1024;
	static constexpr Bytes RESERVE_ALIGNMENT = 64ULL * 1024;
	static constexpr Bytes PAGE_ALIGNMENT = 4 * 1024ULL;

	// Alignment
	FORCEINLINE size_t reserveAlign(size_t v) {
		return (v + RESERVE_ALIGNMENT - 1) & ~(RESERVE_ALIGNMENT - 1);
	}

	FORCEINLINE size_t alignToPage(size_t v) {
		return (v + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
	}

	FORCEINLINE size_t align(size_t v, size_t align) {
		return (v + align - 1) & ~(align - 1);
	}

	FORCEINLINE void* alignPtr(void* p, size_t align) {
		auto v = std::bit_cast<uintptr_t>(p);
		return reinterpret_cast<void*>(align(v, align));
	}

	// Assumes best case inputs always, must be wrapped

	void memcpy(void* __restrict src, void* __restrict dst, Bytes len) noexcept;
	void memrevcpy(void* __restrict src, void* __restrict dst, Bytes len) noexcept;

	// Streaming / non temporal version
	void memcpyStream(void* __restrict src, void* __restrict dst, Bytes len) noexcept;

	bool memcmp(void* __restrict src, void* __restrict dst, Bytes len) noexcept;
	void memset(void* src, std::byte val) noexcept;

	// Unaligned support versions, upto the wrapper to choose which to call

	void memcpyu(void* __restrict src, void* __restrict dst, Bytes len) noexcept;
	void memrevcpyu(void* __restrict src, void* __restrict dst, Bytes len) noexcept;

	// Streaming / non temporal version
	void memcpyuStream(void* __restrict src, void* __restrict dst, Bytes len) noexcept;

	bool memcmpu(void* __restrict src, void* __restrict dst, Bytes len) noexcept;
	void memsetu(void* src, std::byte val) noexcept;

	// User Facing calls
	void memcopy(void* src, void* dst, size_t len);
	void memrevcopy(void* src, void* dst, size_t len);
	void memmove(void* __restrict src, void* __restrict dst, Bytes len) noexcept;

	template<typename T>
	void memoryCopy(T* src, T* dst, Bytes len) {
		if (!src || !dst) return;
		static_assert(std::is_trivially_copyable_v<T> && std::is_trivially_move_constructible_v<T>,
					  "Invalid type, cannot perform byte copying");
		//Delegate to actual call
		memcopy(src, dst, len);
	}

	template<typename T>
	void memMove(T* src, T* dst, Bytes len) {
		if (!src || !dst) return;
		static_assert(std::is_trivially_copyable_v<T> && std::is_trivially_move_constructible_v<T>,
					  "Invalid type, cannot perform byte copying");
		//Delegate to actual call
		memmove(src, dst, len);
	}

	Pvoid align(Pvoid p_Ptr, size_t v_Align);
	Bytes align(Bytes v_Val, size_t v_Align);

		namespace Memory {
		struct ALIGNAS(32) MemHandle final {
			Bytes m_TotalSize = 0;
			Bytes m_CommittedSize = 0;
			Pvoid m_BaseAddress = nullptr;

		private:
			MAYBE_UNUSED Bytes reserved_ = 0;
		public:
			COPY_DEFAULTS(MemHandle)
				MOVE_DEFAULTS(MemHandle)
				MemHandle() = default;
			~MemHandle() = default;

			bool isValid() const;
		};

		enum class MemOp : uint8_t {
			Reserve,
			Commit,
			Decommit,
			Release
		};

		class VirtualMemory final {
		public:
			static bool alloc(MemHandle* handle, size_t size, MemOp operation);
			static bool free(MemHandle* handle, size_t size, MemOp operation);
		};
	}
}
