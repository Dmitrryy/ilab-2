{
    a = 3;
    {
        b = a + 1;
        {
            c = b + 1;
            while(c != a) {
                print c;
                c = c-1;
            }
        }
    }
}