typedef enum { typeCon, typeId, typeOpr } nodeEnum;

/* constants */
typedef struct {
    int value;                  // value of constant
} conNodeType;

/* identifiers */
typedef struct {
    int index;                  // index to symbol table (sym[])
    char key[30];
} idNodeType;

/* operators */
typedef struct {
    int oper;                   // operator
    int nops;                   // number of operands
    struct node *op[0];         // operands
} oprNodeType;

typedef struct node{
    nodeEnum type;              // type of node

    // union must be last entry in nodeType since operNodeType may dynamically increase
    union {
        conNodeType con;        // constant
        idNodeType id;          // identifier
        oprNodeType opr;        // operator
    };
} nodeType;
