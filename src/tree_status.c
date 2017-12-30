#include <git2.h>

#include "functions.h"


void get_tree_status(struct tree_status *tstatus, git_repository *repo) {
    git_index *index;
    git_status_options status_opts;
    git_status_list *status_list;
    const git_status_entry *status_entry;
    size_t i, status_list_size;

    tstatus->untracked_count = 0;
    tstatus->unstaged_count = 0;
    tstatus->staged_count = 0;
    tstatus->conflict_count = 0;

    git_repository_index(&index, repo);
    if (git_index_has_conflicts(index)) {
        tstatus->conflict_count++;
        return;
    }

    git_status_init_options(&status_opts, GIT_STATUS_OPTIONS_VERSION);
    status_opts.flags |= GIT_STATUS_OPT_INCLUDE_UNTRACKED;
    git_status_list_new(&status_list, repo, &status_opts);
    status_list_size = git_status_list_entrycount(status_list);

    for (i = 0; i < status_list_size; i++) {
        status_entry = git_status_byindex(status_list, i);

        /* if there are no other flags, it means the file is untouched */
        if (status_entry->status == GIT_STATUS_CURRENT) {
            continue;
        } else if (status_entry->status == GIT_STATUS_WT_NEW) {
            tstatus->untracked_count++;
        } else {
            if (status_entry->status & (GIT_STATUS_WT_DELETED | GIT_STATUS_WT_MODIFIED | GIT_STATUS_WT_RENAMED | GIT_STATUS_WT_TYPECHANGE))
                tstatus->unstaged_count++;
            if (status_entry->status & (GIT_STATUS_INDEX_NEW | GIT_STATUS_INDEX_MODIFIED | GIT_STATUS_INDEX_DELETED | GIT_STATUS_INDEX_RENAMED | GIT_STATUS_INDEX_TYPECHANGE))
                tstatus->staged_count++;
        }
    }
    git_status_list_free(status_list);
}
