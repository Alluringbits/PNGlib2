#pragma once
#include <exception>
#include <variant>
#include <cstdint>
#include <string_view>

//put this in a namespace, create general PNG namespace?
struct PNGewb : public std::exception{
	//PNGewb() noexcept {};
	//PNGewb( PNGewb && t) noexcept  = default;
	//PNGewb & operator=( PNGewb && t) noexcept = default;
	virtual const char * what() const noexcept override = 0;
	virtual uint16_t code() const noexcept = 0;
	protected:
		std::variant<std::string_view, int64_t> val{};
};

