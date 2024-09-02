void triangular (int n) 
{
    int result = 0;
    for (;;n--) {
        result = result + n;
        printf("%d ~~~ %d", result , n );
    }
}

int main () 
{
    triangular(5);
    return 0;
}
