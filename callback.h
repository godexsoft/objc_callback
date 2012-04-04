#pragma once
#ifndef _OBJC_CALLBACK_H_
#define _OBJC_CALLBACK_H_

#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/iteration/local.hpp>

#ifndef CALLBACK_arg_max
#  define CALLBACK_arg_max 10
#endif

//
// Generate all obj-c callback bridges with safe signatures
//
#define CALLBACK_operator_params(z, n, text) BOOST_PP_COMMA_IF(n) \
    BOOST_PP_CAT(T,n) BOOST_PP_CAT(text,n)

#define CALLBACK_generator(z, n, unused)                \
template<typename R BOOST_PP_COMMA_IF(n)                \
    BOOST_PP_ENUM_PARAMS(n, typename T)>                \
class objc_callback_##n                                 \
{                                                       \
public:                                                 \
    typedef R (*func)(                                  \
        id, SEL BOOST_PP_COMMA_IF(n)                    \
        BOOST_PP_ENUM_PARAMS(n, T)                      \
    );                                                  \
    objc_callback_##n(SEL sel, id obj)                  \
        : sel_(sel)                                     \
        , obj_(obj)                                     \
        , fun_((func)[obj methodForSelector:sel])       \
    {}                                                  \
    inline R operator ()                                \
    (BOOST_PP_REPEAT_FROM_TO(                           \
        0, n, CALLBACK_operator_params, par))           \
    {                                                   \
        return fun_(obj_, sel_ BOOST_PP_COMMA_IF(n)     \
            BOOST_PP_ENUM_PARAMS(n, par));              \
    }                                                   \
private:                                                \
    SEL sel_;                                           \
    id obj_;                                            \
    func fun_;                                          \
};

#define BOOST_PP_LOCAL_MACRO(n)   CALLBACK_generator(~, n, ~)
#define BOOST_PP_LOCAL_LIMITS     (0, CALLBACK_arg_max-1)
#include BOOST_PP_LOCAL_ITERATE()

#undef CALLBACK_generator

//
// Now define the preferred syntax. This will be just a wrapper though
//
template<typename Signature> class objc_callback;

#define CALLBACK_generator(z, n, unused)                        \
template<typename R BOOST_PP_COMMA_IF(n)                        \
    BOOST_PP_ENUM_PARAMS(n, typename T)>                        \
class objc_callback<R(                                          \
    BOOST_PP_ENUM_PARAMS(n, T)                                  \
)>                                                              \
    : public objc_callback_##n<R BOOST_PP_COMMA_IF(n)           \
        BOOST_PP_ENUM_PARAMS(n, T)>                             \
{                                                               \
public:                                                         \
    objc_callback(SEL sel, id obj)                              \
        : objc_callback_##n<                                    \
            R BOOST_PP_COMMA_IF(n)                              \
            BOOST_PP_ENUM_PARAMS(n, T)                          \
        >(sel, obj)                                             \
    {                                                           \
    }                                                           \
};      

#define BOOST_PP_LOCAL_MACRO(n)   CALLBACK_generator(~, n, ~)
#define BOOST_PP_LOCAL_LIMITS     (0, CALLBACK_arg_max-1)
#include BOOST_PP_LOCAL_ITERATE()

#undef CALLBACK_operator_params
#undef CALLBACK_generator
#undef CALLBACK_arg_max

#endif // _OBJC_CALLBACK_H_
