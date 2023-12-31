#pragma once

#include "Result.h"

namespace fcpp {

	namespace OptionEnum {

		enum NoneEnum { None };
	}

	template<typename T>
	class Option : public Result<T, OptionEnum::NoneEnum>
	{
	private:
		using BaseType = Result<T, OptionEnum::NoneEnum>;

	public:
		template<IsEnum E>
		Result<T, E> ok_or(E err) noexcept(BaseType::NoExceptMove)
		{
			using ReturnType = Result<T, E>;
			return this->mResult ? ReturnType(this->GetVal()) : ReturnType(err);
		}
		template<IsEnum E>
		Result<T, E> ok_or_else(IsFunc<E> auto&& err) noexcept(BaseType::NoExceptMove)
		{
			using ReturnType = Result<T, E>;
			return this->mResult ? ReturnType(this->GetVal()) : ReturnType(err);
		}
	};

	template<typename T>
	SCONSTEXPR Option<T> Some(T&& result)
	{
		return Option<T>(std::move(result));
	}
	template<typename T, typename... Args>
	SCONSTEXPR Option<T> Some(Args&&... args)
	{
		return Option<T>(T(std::forward<Args>(args)...));
	}

	template<typename T>
	SCONSTEXPR Option<T> None()
	{
		return Option<T>();
	}
}