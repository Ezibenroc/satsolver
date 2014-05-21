// Unicode-aware string size calculation.
unsigned long unisize(const char *s) {
    // From http://stackoverflow.com/a/4063229/539465
    unsigned long len=0;
    while (*s)
        len += (*s++ & 0xc0) != 0x80;
    return len;
}


