#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "hashtable.h"
#include "parser.tab.h"

extern void yyerror(char *s);
extern hashtable_t *ht;
extern int sym[26];

static int cont = 0;
static int brk = 0;
static int loop_depth = 0;

int print_iter(char *key, void *val) {
  printf("%s -> %s\n", key, (char *)val);
  return 1;
}

void print_ht_stats(hashtable_t *ht) {
  bucket_t *b;
  unsigned long idx, len, max_len=0, num_buckets=0, num_chains=0;
  for (idx=0; idx<ht->size; idx++) {
    b = ht->buckets[idx];
    len = 0;
    while (b) {
      len++;
      num_buckets++;
      b = b->next;
    }
    if (len > 0) {
      num_chains++;
    }
    if (max_len < len) {
      max_len = len;
    }
  }
  printf("Num buckets = %lu\n", num_buckets);
  printf("Max chain length = %lu\n", max_len);
  printf("Avg chain length = %0.2f\n", (float)num_buckets / num_chains);
}

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
    char *key;
    void *val;
    char buf[80];
    if (!p) return 0;
    switch(p->type) {
    case typeCon:       return p->con.value;
    case typeId:        
                        return atoi(ht_get(ht, p->id.key));
                        // return sym[p->id.index];
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

                        // sprintf(buf, "%d", (ex(p->opr.op[1])));
                        // key = strdup(p->opr.op[0]->id.key);
                        // val = strdup(buf);
                        // ht_put(ht, key, val);
                        // return atoi(buf);
    		            return sym[p->opr.op[0]->id.index] = index;

        case ';':       ex(p->opr.op[0]); return ex(p->opr.op[1]);
        case '=':       
                        // if (p->opr.op[0]->type == typeOpr){
                        //     index = p->opr.op[0]->opr.op[0]->id.index +
                        //             ex(p->opr.op[0]->opr.op[1]);
                        //     if(index<26 && index >=0){
                        //         return sym[index] = ex(p->opr.op[1]);
                        //     }else{
                        //         yyerror("Index out of range.");
                        //         exit(1);
                        //     }
                        // }
                        
                        sprintf(buf, "%d", (ex(p->opr.op[1])));
                        key = strdup(p->opr.op[0]->id.key);
                        val = strdup(buf);
                        ht_put(ht, key, val);
                        return atoi(buf);
                        // return sym[p->opr.op[0]->id.index] = ex(p->opr.op[1]);

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
