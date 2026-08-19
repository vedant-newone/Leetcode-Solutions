int getMinDistance(int* nums, int numsSize, int target, int start) {
    int mindis = numsSize,distance;
    for(int i=0 ; i<numsSize ; i++){
        if(nums[i]==target){
            distance = abs(i - start);
            if(distance < mindis){
                mindis = distance;
            }  
        }
    }
    return mindis;
}