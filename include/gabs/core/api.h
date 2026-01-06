
#ifndef GABS_CORE_API_H__
#define GABS_CORE_API_H__

#include <gabs/compiler.h>

#define GABS_API    GABS_COMPILER_API
#define GABS_INLINE GABS_COMPILER_INLINE

#define GABS_DYN_IMPL_NAME(x_)    impl_##x_##__
#define GABS_DYN_IMPL(x_)         static GABS_DYN_IMPL_NAME(x_)
#define GABS_STATIC_IMPL_NAME(x_) x_
#define GABS_STATIC_IMPL(x_)      GABS_STATIC_IMPL_NAME(x_)

#endif /* GABS_CORE_API_H__ */
