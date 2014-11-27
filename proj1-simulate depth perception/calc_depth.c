/*
 * PROJ1-1: YOUR TASK A CODE HERE
 *
 * Feel free to define additional helper functions.
 */

#include "calc_depth.h"
#include "utils.h"
#include <math.h>
#include <limits.h>
#include <stdio.h>

/* Implements the normalized displacement function */
unsigned char normalized_displacement(int dx, int dy,
        int maximum_displacement) {

    double squared_displacement = dx * dx + dy * dy;
    double normalized_displacement = round(255 * sqrt(squared_displacement) / sqrt(2 * maximum_displacement * maximum_displacement));
    return (unsigned char) normalized_displacement;
}

// int in_image(int img_row,int img_column, int feature_width, int feature_height, int image_width, int image_height, int feature_column, int feature_row) {
//     if((img_column - feature_width + feature_column < 0) || (img_column-feature_width+feature_column > image_width)) {
//         return 0;
//     }
//     if ((img_row - feature_height + feature_row < 0) || (img_row-feature_height+feature_row > image_height)) {
//         return 0;
//     }
//     return 1;
// }

int in_space(int space_row,int space_column, int feature_width, int feature_height, int space_width, int space_height) {//returns whether if feature is within a certain space
    if((space_column - feature_width < 0) || (space_column+feature_width >= space_width)) {
        return 0;
    }
    if ((space_row - feature_height < 0) || (space_row+feature_height >= space_height)) {
        return 0;
    }
    return 1;
}
    
int convert_to_1D(int row, int column, int image_width, int image_height) {
    int position = row*image_width + column;
    //printf("here\n");
    return position;
}


// int squared_euclidian_calculator(int feature_height, unsigned char feature_left[][2*feature_height+1], unsigned char feature_right[][2*feature_height+1], int feature_width){ //returns square euclidean distance between 2 features
//     int total = 0;
//     for(int i = 0; i < 2*feature_height+1; i++){
//         for(int j = 0; j<2*feature_width+1; j++){
//             total += ((feature_left[j][i] - feature_right[j][i])*(feature_left[j][i] - feature_right[j][i]));
//         }
//     }
//     return total;
// }

int difference_squared(int val1, int val2){
    return (val1 - val2)*(val1 - val2);
}

int squared_euclidean_distance(int x_left, int y_left, int x_right, int y_right, int feature_width, int feature_height, int image_width, int image_height, unsigned char* left, unsigned char* right) {
    int left_val = 0;
    int right_val = 0;
    int total = 0;
    for (int y = -feature_height; y <= feature_height; y++){
        for(int x = -feature_width; x <= feature_width; x++ ){ //less than equal to?
            // if(!in_space(y_right + y, x_right + x, feature_width, feature_height, image_width, image_height)){
            //     break;
            // } not sure if we need this check? To see if right feature goes outside image
            left_val = left[convert_to_1D(y_left + y, x_left + x, image_width, image_height)];
            right_val = right[convert_to_1D(y_right + y, x_right + x, image_width, image_height)];
            total += difference_squared(left_val, right_val);

        }
    }
    //printf("total %d\n",total);
    return total;
}

void calc_depth(unsigned char *depth_map, unsigned char *left,
        unsigned char *right, int image_width, int image_height,
        int feature_width, int feature_height, int maximum_displacement) {

    int max_disp = maximum_displacement;
    //tests
    // unsigned char feature_left[2*feature_width+1][2*feature_height+1];
    // unsigned char feature_right[2*feature_width+1][2*feature_height+1];
    // unsigned char set_array[image_width*image_height];
    
    if (maximum_displacement == 0) {
        for (int setter = 0; setter < image_width*image_height; setter++) {
            depth_map[setter] = 0;
        }
        return; 
    }
    unsigned int min_distance = ~(0);
    unsigned int temp_min = ~(0);
    // int search_width = (2*feature_width+1) + (2*maximum_displacement);
    // int search_height = (2*feature_height+1) + (2*maximum_displacement);
    
    int RF_X = 0, RF_Y = 0;
    //unsigned int MIN = ~(0);
    
    for (int i = 0; i < image_height; i++) {//looping through (height)
        for (int j = 0; j < image_width; j++) { // looping through (width)

            if (!in_space(i, j, feature_width, feature_height, image_width, image_height)) {
                // assign depth map to 0
                int convert_left = convert_to_1D(i,j,image_width,image_height);
                depth_map[convert_left] = 0;
                //printf("here\n");
                continue;
            }

            //Looping through search space
            for (int delta_y = -max_disp; delta_y <= max_disp; delta_y++) {
                for (int delta_x = -max_disp; delta_x <= max_disp; delta_x++) {
                    if (!in_space(i + delta_y, j + delta_x, feature_width, feature_height, image_width, image_height))/* The pixel at i + del_y, j + del_x, fw, fh */ {
                        //printf("breaking\n");
                        continue;
                    }
                    temp_min = squared_euclidean_distance(j, i, j + delta_x, i + delta_y, feature_width, feature_height, image_width, image_height, left, right);
                    //printf("temp_min %d\n", temp_min);
                    if(temp_min < min_distance){
                    //    printf("less than\n");
                        min_distance = temp_min;
                        RF_X = j + delta_x;
                        RF_Y = i + delta_y;
                    }
                    else if (temp_min == min_distance) {
                        unsigned char result1 = normalized_displacement(delta_x, delta_y, max_disp); //not sure about dx dy
                        unsigned char result2 = normalized_displacement(j-RF_X, i - RF_Y, max_disp);
                        if(result1 < result2){
                            min_distance = temp_min;
                            RF_X = j + delta_x;
                            RF_Y = i + delta_y;
                        }
                    }
                }
            }
            min_distance = ~(0);
            int convert_last = convert_to_1D(i,j,image_width,image_height);
            //printf("convert_last %d\n",convert_last );
            int normalized = normalized_displacement(j - RF_X,i - RF_Y,max_disp);
            depth_map[convert_last] = normalized;
        }
    }    
}



