int var = 10;
extern int my_var;
extern int my_func(int, int);

int main() {
    int a, b;
    a = var;
    b = my_var;
    return my_func(a, b);
}