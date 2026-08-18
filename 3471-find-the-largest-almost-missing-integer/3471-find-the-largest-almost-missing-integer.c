int largestInteger(int* nums, int numsSize, int k) {
    int count[51] = {0};

    // Count frequency of every number
    for(int i = 0; i < numsSize; i++) {
        count[nums[i]]++;
    }

    // k = 1
    if(k == 1) {
        for(int i = 50; i >= 0; i--) {
            if(count[i] == 1) {
                return i;
            }
        }
        return -1;
    }

    // k = numsSize
    if(k == numsSize) {
        int max = nums[0];

        for(int i = 1; i < numsSize; i++) {
            if(nums[i] > max) {
                max = nums[i];
            }
        }

        return max;
    }

    // 1 < k < numsSize
    int ans = -1;

    if(count[nums[0]] == 1) {
        ans = nums[0];
    }

    if(count[nums[numsSize - 1]] == 1 &&
       nums[numsSize - 1] > ans) {
        ans = nums[numsSize - 1];
    }

    return ans;
}