#ifndef PROJECT_LINE_VAR_MACRO_H
#define PROJECT_LINE_VAR_MACRO_H

#define MSGRPC_CAT(a, b)      MSGRPC_CAT_I(a, b)
#define MSGRPC_CAT_I(a, b)    MSGRPC_CAT_II(~, a ## b)
#define MSGRPC_CAT_II(p, res) res

#define ___LINE_VAR(base) MSGRPC_CAT(base, __LINE__)

#endif //PROJECT_LINE_VAR_MACRO_H
