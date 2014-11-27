/*
 * PROJ1-1: YOUR TASK B CODE HERE
 *
 * Feel free to define additional helper functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include "quadtree.h"
#include "make_qtree.h"
#include "utils.h"

#define ABS(x) (((x) < 0) ? (-(x)) : (x))

int convert_to_1D(int row, int column, int image_width) {
    int position = row*image_width + column;
    return position;
}


int homogenous(unsigned char *depth_map, int map_width, int x, int y, int section_width) {

    /* YOUR CODE HERE */
    unsigned char val = depth_map[convert_to_1D(y, x, map_width)];
    for(int i = x; i< x +section_width; i++){
        for(int j = y; j < y + section_width; j++){
            if(val != depth_map[convert_to_1D(j, i, map_width)]){
                return 256;
            }
        }
    }
    return val;

}

qNode *helper(unsigned char *depth_map, int map_width, int x, int y, int section_width){
    qNode *root;
    root = (qNode *)malloc(sizeof(qNode));
    if (root == NULL) {
        allocation_failed();
    }
    root->size = section_width;
    root->x = x;
    root->y = y;
    int val = homogenous(depth_map, map_width, root->x, root->y, section_width);
    root->gray_value = depth_map[convert_to_1D(root->y, root->x, map_width)];
    root->leaf = 0;
    // if(root->leaf){
    //     return root;
    // }
    if(val == root->gray_value){
        root->child_NW = NULL;
        root->child_NE = NULL;
        root->child_SE = NULL;
        root->child_SW = NULL;
        root->leaf = 1;

        
    } else if (val == 256){
        root->gray_value = val;
        root->child_NW = helper(depth_map, map_width, root->x, root->y, (root->size)/2);
        root->child_NE = helper(depth_map, map_width, root->x + root->size/2, root->y, (root->size)/2); 
        root->child_SE = helper(depth_map, map_width,root->x + root->size/2,root->y+root->size/2, (root->size)/2);
        root->child_SW = helper(depth_map, map_width, root->x, root->y + root->size/2, (root->size)/2);
  
    }
    return root;
    
}
qNode *depth_to_quad(unsigned char *depth_map, int map_width) {

    /* YOUR CODE HERE */
    
    return helper(depth_map, map_width, 0, 0, map_width);
   
    //return NULL;
}




void free_qtree(qNode *qtree_node) {
    if(qtree_node) {
        if(!qtree_node->leaf){
            free_qtree(qtree_node->child_NW);
            free_qtree(qtree_node->child_NE);
            free_qtree(qtree_node->child_SE);
            free_qtree(qtree_node->child_SW);
        }
        free(qtree_node);
    }
}