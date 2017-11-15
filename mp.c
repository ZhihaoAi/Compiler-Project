#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include "parser.tab.h"

extern void yyerror(char *s);
extern int sym[26];

static int cont = 0;
static int brk = 0;
static int loop_depth = 0;

int ex(nodeType *p)
{
    if ((cont || brk)){
        if(loop_depth!=0){
            return 0;
        }else{
            yyerror("Continue or Break statement should be used within loop.");
            exit(1);
        }
    }
    int valRead;
    int itr;
    int index;
    if (!p) return 0;
    switch(p->type) {
    case typeCon:       return p->con.value;
    case typeId:        return sym[p->id.index];
    case typeOpr:
        switch(p->opr.oper) {
        case FOR:	    for (itr=0; itr<3; ++itr){
                            if (p->opr.op[itr]->opr.oper == BREAK||
                                p->opr.op[itr]->opr.oper == CONTINUE){
                                yyerror("Invalid Continue or Break statement.");
                                exit(1);
                            }
                        }
                        
                        ex(p->opr.op[0]);
                        loop_depth++;
    			        while (ex(p->opr.op[1])) {
    				        ex(p->opr.op[3]);
                            if (brk){
                                brk = 0;
                                loop_depth--;
                                return 0;
                            }
                            cont = 0;
    				        ex(p->opr.op[2]);
    			        }
                        loop_depth--;
    			        return 0;
        case WHILE:     if (p->opr.op[0]->type == typeOpr){
                            if (p->opr.op[0]->opr.oper == BREAK ||
                                p->opr.op[0]->opr.oper == CONTINUE){
                                yyerror("Invalid Continue or Break statement.");
                                exit(1);
                            }
                        }
                        loop_depth++;
                        while(ex(p->opr.op[0])){
                            ex(p->opr.op[1]);
                            if (brk){
                                brk = 0;
                                loop_depth--;
                                return 0;
                            }
                            cont = 0;
                        }
                        loop_depth--;
                        return 0;
        case IF:        if (ex(p->opr.op[0]))
                            ex(p->opr.op[1]);
                        else if (p->opr.nops > 2)
                            ex(p->opr.op[2]);
                        return 0;
        case PRINT:     printf("%d\n", ex(p->opr.op[0])); return 0;

        case READ:	    printf("Enter an integer: "); scanf("%d", &valRead);
        				if (p->opr.op[0]->type == typeId){
                            index = p->opr.op[0]->id.index;
                        } else if (p->opr.op[0]->type == typeOpr){
                            index = p->opr.op[0]->opr.op[0]->id.index
                                    + ex(p->opr.op[0]->opr.op[1]);
                        }
                        if(index<26 && index >=0){
                            return (sym[index] = valRead);
                        } else {
                            yyerror("Index out of range.");
                            exit(1);
                        }
    		            return sym[p->opr.op[0]->id.index] = index;

        case ';':       ex(p->opr.op[0]); return ex(p->opr.op[1]);
        case '=':       if (p->opr.op[0]->type == typeOpr){
                            index = p->opr.op[0]->opr.op[0]->id.index +
                                    ex(p->opr.op[0]->opr.op[1]);
                            if(index<26 && index >=0){
                                return sym[index] = ex(p->opr.op[1]);
                            }else{
                                yyerror("Index out of range.");
                                exit(1);
                            }
                        }
                        return sym[p->opr.op[0]->id.index] = ex(p->opr.op[1]);

        case UMINUS:    return -ex(p->opr.op[0]);
        case UPLUS:     return +ex(p->opr.op[0]);
        case '~':       return ~ex(p->opr.op[1]);

        case '&':       return ex(p->opr.op[0]) & ex(p->opr.op[1]);
        case '|':       return ex(p->opr.op[0]) | ex(p->opr.op[1]);
        case '^':       return ex(p->opr.op[0]) ^ ex(p->opr.op[1]);

        case '+':       return ex(p->opr.op[0]) + ex(p->opr.op[1]);
        case '-':       return ex(p->opr.op[0]) - ex(p->opr.op[1]);
        case '*':       return ex(p->opr.op[0]) * ex(p->opr.op[1]);
        case '/':       return ex(p->opr.op[0]) / ex(p->opr.op[1]);
        case '%':       return ex(p->opr.op[0]) % ex(p->opr.op[1]);

        case '<':       return ex(p->opr.op[0]) < ex(p->opr.op[1]);
        case '>':       return ex(p->opr.op[0]) > ex(p->opr.op[1]);
        case GE:        return ex(p->opr.op[0]) >= ex(p->opr.op[1]);
        case LE:        return ex(p->opr.op[0]) <= ex(p->opr.op[1]);
        case NE:        return ex(p->opr.op[0]) != ex(p->opr.op[1]);
        case EQ:        return ex(p->opr.op[0]) == ex(p->opr.op[1]);

        case '!':       return !ex(p->opr.op[0]);
        case AND:	    return ex(p->opr.op[0]) && ex(p->opr.op[1]);
        case OR:	    return ex(p->opr.op[0]) || ex(p->opr.op[1]);

        case BREAK:     brk = 1; return 0;
        case CONTINUE:  cont = 1; return 0;
        case SUBSCRIPT: index = p->opr.op[0]->id.index+ex(p->opr.op[1]);
                        if (index < 26){
                            return sym[index];
                        } else {
                            yyerror("Index out of range");
                            exit(1);
                        }
        }
    }
    return 0;
}
