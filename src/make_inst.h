#ifndef MAKE_INST_H
#define MAKE_INST_H

#define MAKE_INST_TEMPLATE(                                                                                            \
    inst, dst_operand_type, dst_value_type, dst_value, src_operand_type, src_value_type, src_value                     \
)                                                                                                                      \
    {                                                                                                                  \
        inst, .dst = { dst_operand_type, .dst_value_type = dst_value }, .src = {                                       \
            src_operand_type,                                                                                          \
            .src_value_type = src_value                                                                                \
        }                                                                                                              \
    }
#define MAKE_NOP { INST_NOP }
#define MAKE_PUSH( operand_type, value_type, value )                                                                   \
    MAKE_INST_TEMPLATE( INST_PUSH, operand_type, value_type, value, 0, imm, 0 )
#define MAKE_SWAP { INST_SWAP }
#define MAKE_DUP  { INST_DUP }
#define MAKE_PLUS { INST_PLUS }

#define MAKE_MOV( dst_operand_type, dst_value_type, dst_value, src_operand_type, src_value_type, src_value )           \
    MAKE_INST_TEMPLATE(                                                                                                \
        INST_MOV, dst_operand_type, dst_value_type, dst_value, src_operand_type, src_value_type, src_value             \
    )
#define MAKE_ADD( dst_operand_type, dst_value_type, dst_value, src_operand_type, src_value_type, src_value )           \
    MAKE_INST_TEMPLATE(                                                                                                \
        INST_ADD, dst_operand_type, dst_value_type, dst_value, src_operand_type, src_value_type, src_value             \
    )
#define MAKE_CMP( dst_operand_type, dst_value_type, dst_value, src_operand_type, src_value_type, src_value )           \
    MAKE_INST_TEMPLATE(                                                                                                \
        INST_CMP, dst_operand_type, dst_value_type, dst_value, src_operand_type, src_value_type, src_value             \
    )

#define MAKE_JNE( operand_type, value_type, value )                                                                    \
    MAKE_INST_TEMPLATE( INST_JNE, operand_type, value_type, value, 0, imm, 0 )
#define MAKE_CALL( operand_type, value_type, value )                                                                   \
    MAKE_INST_TEMPLATE( INST_CALL, operand_type, value_type, value, 0, imm, 0 )
#define MAKE_RET { INST_RET }

#define MAKE_SYSCALL { INST_SYSCALL }
#define MAKE_HALT    { INST_HALT }

#endif // !MAKE_INST_H
