#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include <stdio.h>
/* Daniel J. Bernstein's "times 33" string hash function, from comp.lang.C;
   See https://groups.google.com/forum/#!topic/comp.lang.c/lSKWXiuNOAk */
unsigned long hash(char *str) {
  unsigned long hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

hashtable_t *make_hashtable(unsigned long size) {
  hashtable_t *ht = malloc(sizeof(hashtable_t));
  ht->size = size;
  ht->buckets = calloc(sizeof(bucket_t *), size);
  return ht;
}

void ht_put(hashtable_t *ht, char *key, void *val) {
  unsigned int idx = hash(key) % ht->size;
  bucket_t *n = ht->buckets[idx];
  while(n){
     if(strcmp(n->key, key) == 0){
       // free(key);
       // free(n->val);
       n->val = val;
       return;
      }
     n = n->next;
  }
  if(n == NULL){
    bucket_t *b = malloc(sizeof(bucket_t));
    b->key = key;
    b->val = val;
    b->next = ht->buckets[idx];
    ht->buckets[idx] = b;
  }
}

void *ht_get(hashtable_t *ht, char *key) {
  unsigned int idx = hash(key) % ht->size;
  bucket_t *b = ht->buckets[idx];
  while (b) {
    if (strcmp(b->key, key) == 0) {
      return b->val;
    }
    b = b->next;
  }
  return NULL;
}

void ht_iter(hashtable_t *ht, int (*f)(char *, void *)) {
  bucket_t *b;
  unsigned long i;
  for (i=0; i<ht->size; i++) {
    b = ht->buckets[i];
    while (b) {
      if (!f(b->key, b->val)) {
        return ; // abort iteration
      }
      b = b->next;
    }
  }
}

void free_hashtable(hashtable_t *ht) {
  
  unsigned long i;
  bucket_t *b;
  bucket_t *n;

  for (i=0; i<ht->size; i++) {
    b = ht->buckets[i];
    while (b) {
      free(b->key);
      free(b->val);
      n = b->next;
      free(b);
      b = n;
    }
  }
  free(ht->buckets); 
  free(ht); 
}

void  ht_del(hashtable_t *ht, char *key) {
  unsigned int idx = hash(key) % ht->size;
  bucket_t *n = ht->buckets[idx];
  bucket_t *pren = n;
  
  while(n){
     if(strcmp(n->key, key) == 0){
        if(n == pren){
           ht->buckets[idx] = n->next;
           free(n->key);
           free(n->val);
	   free(n);
           return;
         }
        else{
           pren->next = n->next;
           free(n->key);
           free(n->val);
           free(n);
           n = pren->next;
           return;
        }
     }
    pren=n;
    n = n->next;
 }
 free(n->key);
 free(n->val);
 free(n);
}

void  ht_rehash(hashtable_t *ht, unsigned long newsize) {
  hashtable_t *nht = make_hashtable(newsize);                                      
  bucket_t *b;
  bucket_t *n;
  unsigned long i;
  for (i=0; i<ht->size; i++) {
    b = ht->buckets[i];
    while (b){
      ht_put(nht, b->key, b->val);
      n = b->next;
      free(b);
      b = n;
    }
  }
  free(ht->buckets);                                                                                
  *ht =*nht;
  free(nht); 
                                        
}