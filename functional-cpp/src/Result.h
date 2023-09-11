#pragma once

#include "Common.h"

namespace fcpp {

	template<typename T, IsEnum E>
	class Result;

	template<typename T, IsEnum E>
	class Result
	{
	private:
		using EnumUnderlyingType = int64_t;

		using Type = Result<T, E>;
		using StorageType = std::variant<T, E>;

	public:
		SCONSTEXPR bool NoExceptDefNew  = std::is_nothrow_default_constructible_v<T>;
		SCONSTEXPR bool NoExceptNew		= std::is_nothrow_constructible_v<T>;
		SCONSTEXPR bool NoExceptMove	= std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>;
		SCONSTEXPR bool NoExceptCopy	= std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_copy_assignable_v<T>;

		enum InternalEnum : EnumUnderlyingType
		{
			Ok = Limits<EnumUnderlyingType>::Min
		};

		using EnumUnionType = std::variant<InternalEnum, E>;
		using DataType = T;
		using EnumType = E;

	public:
		explicit constexpr Result(T&& result) noexcept(NoExceptMove)
			: mValue(std::move(result)), mResult(true) {}
		explicit constexpr Result(E error) noexcept
			: mValue(error), mResult(false) {}

		constexpr EnumUnionType as_enum() const noexcept { return mResult ? EnumUnionType(Ok) : GetErr(); }

		constexpr bool is_ok() const noexcept { return mResult; }
		constexpr bool is_err() const noexcept { return !mResult; }

		constexpr T clone() const noexcept(NoExceptCopy) { return GetValCopy(); }

		constexpr T&& expect(const std::string_view msg)
		{
			if (!mResult)
				throw std::runtime_error(msg.data());
			else
				return GetVal();
		}
		constexpr T&& unwrap() { return expect("emergency failure"); }

		constexpr T&& unwrap_or(T&& defaultValue) noexcept(NoExceptMove)
		{
			if (mResult)
				return GetVal();
			else
				return T(std::move(defaultValue));
		}
		constexpr T&& unwrap_or_default() noexcept(NoExceptDefNew)
		{
			return unwrap_or(T());
		}
		template<typename Func> requires IsFunc<Func, T>
		constexpr T&& unwrap_or_else(Func&& op) noexcept(noexcept(op()) && NoExceptMove)
		{
			return unwrap_or(op());
		}

		template<typename Func, typename R> requires IsFunc<Func, Result<R, E>, T&&>
		constexpr Result<R, E>&& map(Func&& op) noexcept(noexcept(op()) && Result<R, E>::NoExceptMove && NoExceptMove)
		{
			if (mResult)
				return op(GetVal());

			return Result<R, E>(GetErr());
		}

		template<typename Func, typename R = T> requires IsFunc<Func, Result<R, E>>
		constexpr Result<R, E> then(Func&& next) noexcept(noexcept(next()) && Result<R, E>::NoExceptMove)
		{
			if (mResult)
				return next();

			return Result<R, E>(GetErr());
		}

		template<typename Func, typename R = T> requires IsFunc<Func, Result<R, E>>
		constexpr Result<R, E> operator |(Func&& next) noexcept(noexcept(next()) && Result<R, E>::NoExceptMove)
		{
			if (mResult)
				return next();

			return Result<R, E>(GetErr());
		}

		constexpr T&& GetVal() noexcept(NoExceptMove) { return std::move(*std::get_if<T>(&mValue)); }
		constexpr T GetValCopy() { return *std::get_if<T>(&mValue); }
		constexpr E GetErr() const noexcept { return *std::get_if<E>(&mValue); }

	protected:
		bool mResult;

	private:
		StorageType mValue;
	};

	template<typename T, IsEnum E>
	SCONSTEXPR Result<T, E> Ok(T&& result) noexcept(Result<T, E>::NoExceptMove)
	{
		return Result<T, E>(std::move(result));
	}
	template<typename T, IsEnum E, typename... Args>
	SCONSTEXPR Result<T, E> Ok(Args&&... args) noexcept(Result<T, E>::NoExceptNew)
	{
		return Result<T, E>(T(std::forward<Args>(args)...));
	}

	template<typename T, IsEnum E>
	SCONSTEXPR Result<T, E> Err(E error) noexcept
	{
		return Result<T, E>(error);
	}
}