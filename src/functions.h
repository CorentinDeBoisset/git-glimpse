#include <stdlib.h>
#include <stdbool.h>
#include <git2.h>

struct tree_status {
    int untracked_count;
    int unstaged_count;
    int staged_count;
    int conflict_count;
};

struct branch_status {
    int ahead_count;
    int behind_count;
    char *head_name;
    char *current_operation;
};

int main(int argc, char **argv);
void display_usage(void);
void parse_arguments(int argc, char **argv);

int stash_cb(size_t index, const char *message, const int *stash_id, int *payload);
int get_stash_count(git_repository *repo);
void get_tree_status(struct tree_status *tstatus, git_repository *repo);
void get_branch_status(struct branch_status *bstatus, git_repository *repo);
