#pragma once

#pragma region Common

    #define ASSERT(x, ...)  assert(x)
    #define SASSERT(x, ...) static_assert(x)

    #define SCONSTEXPR static constexpr
    #define SCONSTEVAL static consteval

    #define ifc if constexpr

    #define typeof(T) std::decay_t<decltype(T)>

#pragma endregion

#pragma region Reflection

    #if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
    #    define FCPP_FUNC_SIGNATURE __PRETTY_FUNCTION__
    #    define FCPP_FUNC_SIGNATURE_PREFIX '='
    #    define FCPP_FUNC_SIGNATURE_SUFFIX ']'
    #elif defined(__DMC__) && (__DMC__ >= 0x810)
    #    define FCPP_FUNC_SIGNATURE __PRETTY_FUNCTION__
    #    define FCPP_FUNC_SIGNATURE_PREFIX '='
    #    define FCPP_FUNC_SIGNATURE_SUFFIX ']'
    #elif (defined(__FUNCSIG__) || (_MSC_VER))
    #    define FCPP_FUNC_SIGNATURE __FUNCSIG__
    #    define FCPP_FUNC_SIGNATURE_PREFIX '<'
    #    define FCPP_FUNC_SIGNATURE_SUFFIX '>'
    #else
    #   error FCPP_FUNC_SIGNATURE "FCPP_FUNC_SIGNATURE unknown!"
    #endif

#define FCPP_FUNC_SIG_STRING std::string_view { std::source_location::current().function_name() }

#pragma endregion

#pragma region Matching

    #define FCPP_TAG(result, value) result##value

    #define MATCH_START(result) \
            do \
            {\
                using FCPP_TAG(result, ResultType) = std::decay_t<decltype(result)>;\
                using FCPP_TAG(result, EnumUnionType) = FCPP_TAG(result, ResultType)::EnumUnionType;\
                const auto& FCPP_TAG(result, _ref) = result;

    #define MATCH(result, option, code) \
                if (FCPP_TAG(result, _ref).as_enum() == FCPP_TAG(result, EnumUnionType)(option)) {\
                    code \
                } else 
    #define MATCH_A(result, code) \
                {\
                    code \
                }

    #define MATCH_END {}\
            } while (false);

#pragma endregion

#pragma region Do Notation

    #define FUNC_RETURN_TYPE FCPP_FUNC_SIG_STRING.substr(0, FCPP_FUNC_SIG_STRING.find_first_of(' ') - 1)

    #define TRY_RET(value) \
                ifc (FUNC_RETURN_TYPE == TypeTraits<typeof(value)>::Name)\
                {\
                    if (value.is_err())\
                        return value;\
                }\

    #define START(DoAction, ...) DoAction(__VA_ARGS__)
    #define NEXT(DoAction, ...) [&]() -> FunctionTraits<typeof(DoAction)>::ReturnType { return DoAction(__VA_ARGS__); }

    #define DO(ResultExpression) \
            {\
	            auto finalResult = (ResultExpression);\
                TRY_RET(finalResult);\
            }

    #define DO_R(ResultType, ResultEnum, NewVarName, ResultExpression) \
            Result<ResultType, ResultEnum> NewVarName;\
            {\
	            auto finalResult = (ResultExpression);\
                TRY_RET(finalResult);\
                NewVarName = std::move(finalResult); \
            }

#pragma endregion