#include <Windows.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include "stl_arrays.h"

int main() {
	constexpr size_t Size = 1703936ull;// 64 MiB
	constexpr int Iterations = 1000;
	constexpr int WarmupIterations = 100;

	void* src = ::VirtualAlloc(nullptr, Size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	void* dst = ::VirtualAlloc(nullptr, Size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!src || !dst) {
		std::cout << "VirtualAlloc failed\n";
		return -1;
	}
	std::memset(src, 0xAB, Size);

	// Warmup: touch all pages on both buffers, warm branch predictor / I-cache
	// for both code paths before any timing begins.
	for (int i = 0; i < WarmupIterations; ++i) {
		Micro::STL::Arrays::memcopy(src, dst, Size);
		std::memcpy(dst, src, Size);
	}

	// Per-iteration interleaved timing. Interleaving (rather than running
	// all "mine" iterations then all "CRT" iterations) controls for thermal
	// throttling, frequency scaling, and background OS jitter drifting over
	// the ~64GB+ of traffic this benchmark generates -- any such drift now
	// affects both measurements roughly equally per-iteration instead of
	// biasing whichever block happened to run second.
	std::vector<double> myTimes(Iterations);
	std::vector<double> crtTimes(Iterations);

	for (int i = 0; i < Iterations; ++i) {
		auto t0 = std::chrono::high_resolution_clock::now();
		Micro::STL::Arrays::memcopy(src, dst, Size);
		auto t1 = std::chrono::high_resolution_clock::now();
		myTimes[i] = std::chrono::duration<double, std::milli>(t1 - t0).count();

		auto t2 = std::chrono::high_resolution_clock::now();
		std::memcpy(dst, src, Size);
		auto t3 = std::chrono::high_resolution_clock::now();
		crtTimes[i] = std::chrono::duration<double, std::milli>(t3 - t2).count();
	}

	auto stats = [](std::vector<double> v) {
		std::sort(v.begin(), v.end());
		double total = 0.0;
		for (double x : v) total += x;
		struct { double total, avg, median, min, max; } s;
		s.total = total;
		s.avg = total / v.size();
		s.median = v[v.size() / 2];
		s.min = v.front();
		s.max = v.back();
		return s;
		};

	auto myStats = stats(myTimes);
	auto crtStats = stats(crtTimes);

	std::cout << "\n===== Results (interleaved, " << Iterations << " iters, "
		<< (Size / (1024 * 1024)) << " MiB/copy) =====\n\n";

	std::cout << "Micro STL\n";
	std::cout << "  Total  : " << myStats.total << " ms\n";
	std::cout << "  Avg    : " << myStats.avg << " ms\n";
	std::cout << "  Median : " << myStats.median << " ms\n";
	std::cout << "  Min    : " << myStats.min << " ms\n";
	std::cout << "  Max    : " << myStats.max << " ms\n\n";

	std::cout << "CRT memcpy\n";
	std::cout << "  Total  : " << crtStats.total << " ms\n";
	std::cout << "  Avg    : " << crtStats.avg << " ms\n";
	std::cout << "  Median : " << crtStats.median << " ms\n";
	std::cout << "  Min    : " << crtStats.min << " ms\n";
	std::cout << "  Max    : " << crtStats.max << " ms\n\n";

	std::cout << "Speedup (avg)    : " << crtStats.avg / myStats.avg << "x\n";
	std::cout << "Speedup (median) : " << crtStats.median / myStats.median << "x\n";

	::VirtualFree(src, 0, MEM_RELEASE);
	::VirtualFree(dst, 0, MEM_RELEASE);
	return 0;
}