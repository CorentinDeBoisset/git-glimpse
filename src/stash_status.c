#include <git2.h>
#include "functions.h"

int stash_cb(size_t index, const char *message, const int *stash_id, int *payload)
{
    (void)index; (void)message; (void)stash_id;
    (*payload)++;
    return 0;
}

int get_stash_count(git_repository *repo) {
    int stash_count = 0;
    git_stash_foreach(repo, (git_stash_cb)stash_cb, &stash_count);
    return stash_count;
}
