#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre.h>

#define ERRBUF_SIZE 256

void compile_and_match(const char *pattern, const char *subject) {
    const char *error;
    int erroffset;
    pcre *re;
    pcre_extra *extra;
    int ovector[30];
    int rc;

    re = pcre_compile(pattern, 0, &error, &erroffset, NULL);
    if (re == NULL) {
        fprintf(stderr, "PCRE compilation failed at offset %d: %s\n", erroffset, error);
        return;
    }

    extra = pcre_study(re, 0, &error);
    if (error != NULL) {
        fprintf(stderr, "PCRE study failed: %s\n", error);
        pcre_free(re);
        return;
    }

    rc = pcre_exec(re, extra, subject, strlen(subject), 0, 0, ovector, sizeof(ovector)/sizeof(ovector[0]));
    if (rc < 0) {
        switch(rc) {
            case PCRE_ERROR_NOMATCH:
                printf("No match\n");
                break;
            default:
                printf("Matching error %d\n", rc);
                break;
        }
    } else {
        printf("Match succeeded\n");
        for (int i = 0; i < rc; i++) {
            int start = ovector[2*i];
            int end = ovector[2*i + 1];
            printf("Match %d: %.*s\n", i+1, end - start, subject + start);
        }
    }

    pcre_free(extra);
    pcre_free(re);
}

int main() {
    const char *pattern = "a[bcd]*e";
    const char *subject = "abbbcde";

    compile_and_match(pattern, subject);

    return 0;
}
