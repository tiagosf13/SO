#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "linked-list.h"

namespace base {

   Node* destroy(Node* list)
   {
      return NULL;
   }

   Node* append(Node* list, uint32_t nmec, char *name)
   {
      Node *new_node = (Node *)malloc(sizeof(Node));
      if (new_node == NULL){
         fprintf(stderr, "No memory!");
         exit(1);
      }
      new_node->reg.name = name;
      new_node->reg.nmec = nmec;
      new_node->next = NULL;
      
      if (list == NULL){
         return new_node;
      }
      Node *current_node = list;
      while (current_node->next != NULL)
      {
         current_node = current_node->next;
      }

      current_node->next = new_node;
      return list;
   }

   void print(Node* list)
   {
      Node *current_node = list;
      printf("Name   Nmec\n");
      
      while(current_node != NULL){
         printf("%s        %d\n", current_node->reg.name, current_node->reg.nmec);
         current_node = current_node->next;
      }
   }

   int exists(Node* list, uint32_t nmec)
   {
      Node *current_node = list;

      while (current_node != NULL)
      {
         if (current_node->reg.nmec == nmec){
            return 1;
         }
         
         current_node = current_node->next;
      }
      
      return 0;
   }

   Node* remove(Node* list, uint32_t nmec)
   {
      Node *current_node = list;
      Node *previous_node = NULL;

      while (current_node != NULL)
      {
         if (current_node->reg.nmec == nmec){
            Node *next_node;

            if (previous_node == NULL) {
               return current_node->next;
            }
            else {
               next_node = current_node->next;
            }
            previous_node->next = next_node;
            free(current_node);
            return list;
         }

         previous_node = current_node;
         current_node = current_node->next;
      }

      return list;
   }

   const char *search(Node* list, uint32_t nmec)
   {
      Node *current_node = list;

      while (current_node != NULL)
      {
         if (current_node->reg.nmec == nmec){
            return current_node->reg.name;
         }
         else{
            current_node = current_node->next;
         }
      }
      
      return NULL;
   }

   Node* sort_by_name(Node* list){

      Node *current_node = list;

      //quick sort algorithm

      while (current_node != NULL)
      {
         Node *next_node = current_node->next;

         while (next_node != NULL)
         {
            if (strcmp(current_node->reg.name, next_node->reg.name) < 0)
            {
               //create a temporary node to store the values
               uint32_t temp_nmec = current_node->reg.nmec;
               char *temp_name = current_node->reg.name;
               //swap the values
               current_node->reg.nmec = next_node->reg.nmec;
               current_node->reg.name = next_node->reg.name;
               next_node->reg.nmec = temp_nmec;
               next_node->reg.name = temp_name;
            }
            next_node = next_node->next;
         }
         current_node = current_node->next;
      }

      return list;
   }



   Node* sort_by_number(Node* list)
   {
      Node *current_node = list;

      //quick sort algorithm

      while (current_node != NULL)
      {
         Node *next_node = current_node->next;
         while (next_node != NULL)
         {
            if (current_node->reg.nmec > next_node->reg.nmec){
               //create a temporary node to store the values
               uint32_t temp_nmec = current_node->reg.nmec;
               char *temp_name = current_node->reg.name;
               //swap the values
               current_node->reg.nmec = next_node->reg.nmec;
               current_node->reg.name = next_node->reg.name;
               next_node->reg.nmec = temp_nmec;
               next_node->reg.name = temp_name;
            }
            next_node = next_node->next;
         }
         current_node = current_node->next;
      }
      

      return list;
   }
}
