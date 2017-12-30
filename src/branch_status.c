#include <git2.h>
#include <string.h>
#include <regex.h>

#include "functions.h"


void get_branch_status(struct branch_status *bstatus, git_repository *repo) {
    size_t ahead, behind;
    git_reference *head, *upstream;
    const char *head_name = NULL;

    if (git_repository_head(&head, repo)) {
        head_name = "error";
        return;
    }

    if (git_reference_is_branch(head)) {
        head_name = git_reference_shorthand(head);

        if (!git_branch_upstream(&upstream, head)) {
            git_graph_ahead_behind(&ahead, &behind, repo, git_reference_target(head), git_reference_target(upstream));
            bstatus->ahead_count = (int) ahead;
            bstatus->behind_count = (int) behind;
            git_reference_free(upstream);
        }
    } else {
        git_reflog *reflog;
        size_t i, reflog_entrycount;
        const char *reflog_msg;
        regex_t reflog_regex;
        regmatch_t regex_matches[2];
        regcomp(&reflog_regex, "moving from [^ ]* to ([^ ]*)$", REG_EXTENDED);

        git_reflog_read(&reflog, repo, "HEAD");
        reflog_entrycount = git_reflog_entrycount(reflog);
        for (i = 0; i < reflog_entrycount; i++) {
            reflog_msg = git_reflog_entry_message(git_reflog_entry_byindex(reflog, i));
            if (0 == regexec(&reflog_regex, reflog_msg, 2, regex_matches, 0)) {
                /* TODO check if string is a branch, or tag or hash */
                head_name = reflog_msg + regex_matches[1].rm_so;
                break;
            }
        }
        if (NULL == head_name) {
            git_object *obj;
            git_buf buffer = {NULL, 0, 0};
            git_reference_peel(&obj, head, GIT_OBJ_COMMIT);
            git_object_short_id(&buffer, obj);
            head_name = buffer.ptr;
        }
    }

    bstatus->head_name = malloc(strlen(head_name));
    strcpy(bstatus->head_name, head_name);
    git_reference_free(head);
}
