#ifndef PyHP_H
#define PyHP_H

#include "PyHP/iterator_meta.h"
#include "PyHP/statement_type.h"
#include "PyHP/parser_match.h"
#include "PyHP/parser.h"
#include "PyHP/prepare_result.h"
#include "PyHP/prepare.h"
#include "PyHP/command_type.h"
#include "PyHP/command.h"
#include "PyHP/compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

int PyHP_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* PyHP_H */
