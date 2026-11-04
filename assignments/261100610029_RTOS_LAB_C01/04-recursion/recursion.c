int rec_sum(int i){
    if(i <= 1) return i;
    else return i + rec_sum(i - 1);
}

int main(void){
    int sum = 0;
    sum = rec_sum(10);

    return 0;
}