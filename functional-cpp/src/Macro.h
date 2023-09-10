#pragma once

#pragma region Common

    #define ASSERT(x, ...)  assert(x)
    #define SASSERT(x, ...) static_assert(x)

    #define SCONSTEXPR static constexpr
    #define SCONSTEVAL static consteval

    #define ifc if constexpr

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

    #define NEXT(DoAction) [&]() { return DoAction; }

    #define DO_ACTION(ResultExpression)
    #define DO(ResultExpression) \
            {\
	            auto finalResult = (ResultExpression);\
	            if (finalResult.is_err())\
		            return finalResult;\
            }

    #define DO_R(ResultExpression) \
            NewVarType NewVarName;\
            {\
	            auto finalResult = (ResultExpression);\
	            if (finalResult.is_err())\
		            return finalResult;\
            \
                \
                using ResultType = std::decay_t<decltype(finalResult)>;\
                SASSERT(std::is_same_v<type, ResultType::DataType>, "Return type must match result underlying type.")\
                NewVarName = std::move(finalResult.unwrap());\
            }

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

#pragma endregion