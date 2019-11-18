#include <git2.h>
#include <string.h>
#include <regex.h>

#include "functions.h"


void get_branch_status(struct branch_status *bstatus, git_repository *repo) {
    int operation_status = git_repository_state(repo);
    if (operation_status != GIT_REPOSITORY_STATE_NONE) {
        switch (operation_status) {
            case GIT_REPOSITORY_STATE_MERGE:
                bstatus->current_operation = "[merge]";
                break;
            case GIT_REPOSITORY_STATE_REVERT:
            case GIT_REPOSITORY_STATE_REVERT_SEQUENCE:
                bstatus->current_operation = "[revert]";
                break;
            case GIT_REPOSITORY_STATE_CHERRYPICK:
            case GIT_REPOSITORY_STATE_CHERRYPICK_SEQUENCE:
                bstatus->current_operation = "[cherry-pick]";
                break;
            case GIT_REPOSITORY_STATE_BISECT:
                bstatus->current_operation = "[bisect]";
                break;
            case GIT_REPOSITORY_STATE_REBASE:
                bstatus->current_operation = "[rebase]";
                // TODO read the original branch and its status, instead of the current HEAD
                break;
            case GIT_REPOSITORY_STATE_REBASE_INTERACTIVE:
                bstatus->current_operation = "[rebase-i]";
                break;
            case GIT_REPOSITORY_STATE_REBASE_MERGE:
                bstatus->current_operation = "[rebase-merge]";
                break;
            // Not sure about those last 2, but these should be a minority of cases
            case GIT_REPOSITORY_STATE_APPLY_MAILBOX:
                bstatus->current_operation = "[mailbox]";
                break;
            case GIT_REPOSITORY_STATE_APPLY_MAILBOX_OR_REBASE:
                bstatus->current_operation = "[mailbox-rebase]";
                break;
        }
    }

    // Then we get the current reference name

    size_t ahead, behind;
    git_reference *head, *upstream;
    const char *head_name;

    if (git_repository_head(&head, repo)) {
        return;
    }

    if (git_reference_is_branch(head)) {
        // If the reference is a branch, we read its name, and if it's ahead and/or behind its remote counterpart
        head_name = git_reference_shorthand(head);

        if (!git_branch_upstream(&upstream, head)) {
            git_graph_ahead_behind(&ahead, &behind, repo, git_reference_target(head), git_reference_target(upstream));
            bstatus->ahead_count = (int) ahead;
            bstatus->behind_count = (int) behind;
            git_reference_free(upstream);
        }
    } else if (git_reference_is_tag(head) || git_reference_is_remote(head) || git_reference_is_note(head)) {
        // If the reference is something, we get its name
        head_name = git_reference_shorthand(head);
    } else {
        // In last resort, we use the reference's short_id
        git_object *obj;
        git_buf buffer = {NULL, 0, 0};
        git_reference_peel(&obj, head, GIT_OBJ_COMMIT);
        git_object_short_id(&buffer, obj);
        head_name = buffer.ptr;
    }

    bstatus->head_name = malloc(strlen(head_name));
    strcpy(bstatus->head_name, head_name);

    git_reference_free(head);
}
