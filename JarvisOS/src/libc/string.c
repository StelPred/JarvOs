#include "string.h"

void* memcpy(void* dest, const void* src, size_t n) {
    char* d = dest;
    const char* s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

void* memmove(void* dest, const void* src, size_t n) {
    char* d = dest;
    const char* s = src;
    if (d < s) {
        while (n--) {
            *d++ = *s++;
        }
    } else {
        d += n;
        s += n;
        while (n--) {
            *--d = *--s;
        }
    }
    return dest;
}

char* strcpy(char* dest, const char* src) {
    char* tmp = dest;
    while ((*dest++ = *src++) != '\0')
        ;
    return tmp;
}

char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    char* d = dest;
    const char* s = src;
    for (i = 0; i < n && s[i] != '\0'; i++)
        d[i] = s[i];
    for (; i < n; i++)
        d[i] = '\0';
    return dest;
}

char* strcat(char* dest, const char* src) {
    char* tmp = dest;
    while (*dest)
        dest++;
    while ((*dest++ = *src++) != '\0')
        ;
    return tmp;
}

char* strncat(char* dest, const char* src, size_t n) {
    char* tmp = dest;
    while (*dest)
        dest++;
    while (n-- && (*dest = *src++))
        dest++;
    *dest = '\0';
    return tmp;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0)
        return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

size_t strlen(const char* s) {
    const char* p = s;
    while (*p)
        p++;
    return (p - s);
}

void* memset(void* s, int c, size_t n) {
    unsigned char* p = s;
    while (n--)
        *p++ = (unsigned char)c;
    return s;
}

void* memchr(const void* s, int c, size_t n) {
    const unsigned char* p = s;
    while (n--) {
        if (*p == (unsigned char)c)
            return (void*)p;
        p++;
    }
    return NULL;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* p1 = s1;
    const unsigned char* p2 = s2;
    while (n--) {
        if (*p1 != *p2)
            return *p1 - *p2;
        p1++;
        p2++;
    }
    return 0;
}

char* strchr(const char* s, int c) {
    while (*s) {
        if (*s == (char)c)
            return (char*)s;
        s++;
    }
    if (c == '\0')
        return (char*)s;
    return NULL;
}

char* strrchr(const char* s, int c) {
    const char* last = NULL;
    while (*s) {
        if (*s == (char)c)
            last = s;
        s++;
    }
    if (c == '\0')
        return (char*)s;
    return (char*)last;
}

size_t strspn(const char* s, const char* accept) {
    const char* p = s;
    while (*p && strchr(accept, *p))
        p++;
    return p - s;
}

size_t strcspn(const char* s, const char* reject) {
    const char* p = s;
    while (*p && !strchr(reject, *p))
        p++;
    return p - s;
}

char* strpbrk(const char* s, const char* accept) {
    while (*s) {
        if (strchr(accept, *s))
            return (char*)s;
        s++;
    }
    return NULL;
}

char* strstr(const char* haystack, const char* needle) {
    if (!*needle)
        return (char*)haystack;
    for (; *haystack; haystack++) {
        if (strncmp(haystack, needle, strlen(needle)) == 0)
            return (char*)haystack;
    }
    return NULL;
}

char* strtok(char* str, const char* delim) {
    static char* static_str = NULL;
    char* token = NULL;

    if (str != NULL)
        static_str = str;

    if (static_str == NULL)
        return NULL;

    // Skip leading delimiters
    static_str += strspn(static_str, delim);

    if (*static_str == '\0') {
        static_str = NULL;
        return NULL;
    }

    // Find end of token
    token = static_str;
    static_str += strcspn(static_str, delim);

    if (*static_str) {
        *static_str++ = '\0';
    } else {
        static_str = NULL;
    }

    return token;
}

size_t strxfrm(char* dest, const char* src, size_t n) {
    size_t len = strlen(src);
    if (n > 0) {
        size_t copy_len = (len < n) ? len : n - 1;
        memcpy(dest, src, copy_len);
        dest[copy_len] = '\0';
    }
    return len;
}