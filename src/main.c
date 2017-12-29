#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <git2.h>
#include <regex.h>

struct sigils_t {
    char *ahead;
    char *behind;
    char *staged;
    char *conflicts;
    char *unstaged;
    char *untracked;
    char *stashed;
    char *clean;
} sigils;

static const char *opt_string = ":vha:b:s:c:u:U:S:C:";
static const struct option long_opts[] = {
    { "help", no_argument, NULL, 'h' },
    { "ahead-sigil", required_argument, NULL, 'a' },
    { "behind-sigil", required_argument, NULL, 'b' },
    { "staged-sigil", required_argument, NULL, 's' },
    { "conflicts-sigil", required_argument, NULL, 'c' },
    { "unstaged-sigil", required_argument, NULL, 'u' },
    { "untracked-sigil", required_argument, NULL, 'U' },
    { "stashed-sigil", required_argument, NULL, 'S' },
    { "clean-sigil", required_argument, NULL, 'C' },
    { 0, 0, 0, 0 }
};

void display_usage(void)
{
    puts(
        "usage: fast-git-prompt [<options>]\n"
    );
    puts("    -h, --help                   show this help");
    puts("    -a, --ahead-sigil=SIG        overwrite the ahead sigil");
    puts("    -b, --behind-sigil=SIG       overwrite the behind sigil");
    puts("    -s, --staged-sigil=SIG       overwrite the staged sigil");
    puts("    -c, --conflicts-sigil=SIG    overwrite the conflicts sigil");
    puts("    -u, --unstaged-sigil=SIG     overwrite the unstaged sigil");
    puts("    -U, --untracked-sigil=SIG    overwrite the untracked sigil");
    puts("    -S, --stashed-sigil=SIG      overwrite the stashed sigil");
    puts("    -C, --clean-sigil=SIG        overwrite the clean sigil\n");
    exit(EXIT_FAILURE);
}

void parse_arguments(int argc, char **argv)
{
    int opt = 0;

    while (-1 != (opt = getopt_long(argc, argv, opt_string, long_opts, NULL)))
    {
        switch(opt) {
            case 'h':
                display_usage();
                break;
            case 'a':
                sigils.ahead = optarg;
                break;
            case 'b':
                sigils.behind = optarg;
                break;
            case 's':
                sigils.staged = optarg;
                break;
            case 'c':
                sigils.conflicts = optarg;
                break;
            case 'u':
                sigils.unstaged = optarg;
                break;
            case 'U':
                sigils.untracked = optarg;
                break;
            case 'S':
                sigils.stashed = optarg;
                break;
            case 'C':
                sigils.clean = optarg;
                break;
            case ':':
                fprintf(stderr, "option `-%c' requires an argument\n\n", optopt);
                exit(EXIT_FAILURE);
                break;
            case 0:
                break;
            default:
                fprintf(stderr, "option is invalid: ignored\n\n");
                exit(EXIT_FAILURE);
                break;
        }
    }

    if (NULL == sigils.ahead)
        sigils.ahead = "↑";
    if (NULL == sigils.behind)
        sigils.behind = "↓";
    if (NULL == sigils.staged)
        sigils.staged = "●";
    if (NULL == sigils.conflicts)
        sigils.conflicts = "✖";
    if (NULL == sigils.unstaged)
        sigils.unstaged = "✚";
    if (NULL == sigils.untracked)
        sigils.untracked = "…";
    if (NULL == sigils.stashed)
        sigils.stashed = "⚑";
    if (NULL == sigils.clean)
        sigils.clean = "✔";
}

int stash_cb(size_t index, const char *message, const int *stash_id, int *payload)
{
    (void)index; (void)message; (void)stash_id;
    (*payload)++;
    return 0;
}

void get_file_status(git_repository *repo) {
    git_status_options status_opts;
    git_status_list *status_list;
    size_t i, status_list_size;
    const git_status_entry *status_entry;
    int staged = 0;
    int unstaged = 0;
    int untracked = 0;

    git_status_init_options(&status_opts, GIT_STATUS_OPTIONS_VERSION);
    status_opts.flags |= GIT_STATUS_OPT_INCLUDE_UNTRACKED;
    git_status_list_new(&status_list, repo, &status_opts);
    status_list_size = git_status_list_entrycount(status_list);

    for (i = 0; i < status_list_size; i++) {
        status_entry = git_status_byindex(status_list, i);

        /* if there are no other flags, the file is pristine */
        if (status_entry->status == GIT_STATUS_CURRENT) {
            continue;
        } else if (status_entry->status == GIT_STATUS_WT_NEW) {
            untracked++;
        } else {
            if (status_entry->status & (GIT_STATUS_INDEX_NEW | GIT_STATUS_INDEX_MODIFIED | GIT_STATUS_INDEX_DELETED | GIT_STATUS_INDEX_RENAMED | GIT_STATUS_INDEX_TYPECHANGE))
                staged++;
            if (status_entry->status & (GIT_STATUS_WT_DELETED | GIT_STATUS_WT_MODIFIED | GIT_STATUS_WT_RENAMED | GIT_STATUS_WT_TYPECHANGE))
                unstaged++;
        }
    }
    git_status_list_free(status_list);

    if (staged || unstaged || untracked) {
        printf(
            "%s%s%s",
            staged ? sigils.staged : "",
            unstaged ? sigils.unstaged : "",
            untracked ? sigils.untracked : ""
        );
    } else {
        printf("%s", sigils.clean);
    }
}

void get_branch_status(git_repository *repo) {
    size_t ahead, behind;
    git_reference *head, *upstream;
    
    if (git_repository_head(&head, repo))
        return;
    
    if (git_reference_is_branch(head)) {
        printf(git_reference_shorthand(head));
        
        if (git_branch_upstream(&upstream, head))
            return;

        git_graph_ahead_behind(&ahead, &behind, repo, git_reference_target(head), git_reference_target(upstream));

        if (ahead || behind) {
            printf("%s%s", ahead ? sigils.ahead : "", behind ? sigils.behind : "");
        }
    } else {
        git_reflog *reflog;
        const git_reflog_entry *last_entry;
        const char *reflog_msg;
        regex_t reflog_regex;
        regmatch_t regex_matches[2];
        regcomp(&reflog_regex, "moving from [^ ]* to ([^ ]*)$", REG_EXTENDED);
        
        git_reflog_read(&reflog, repo, "HEAD");
        last_entry = git_reflog_entry_byindex(reflog, 0);
        if (last_entry) {
            reflog_msg = git_reflog_entry_message(last_entry);
            if (regexec(&reflog_regex, reflog_msg, 2, regex_matches, 0) == 0) {
                if (regex_matches[1].rm_so == (regoff_t) -1)
                    return;

                if (regex_matches[1].rm_eo - regex_matches[1].rm_so == 40) {
                    // The reference is a commit hash
                    git_object *obj;
                    git_buf buffer = {0};
                    git_reference_peel(&obj, head, GIT_OBJ_COMMIT);
                    git_object_short_id(&buffer, obj);
                    printf("%s", buffer.ptr);
                } else {
                    printf("%s", reflog_msg + regex_matches[1].rm_so);
                }
            }
        }
    }
}

void get_stash_status(git_repository *repo) {
    int stash_count = 0;
    git_stash_foreach(repo, (git_stash_cb)stash_cb, &stash_count);
    printf("%s", stash_count ? sigils.stashed : "");
}

int main(int argc, char **argv)
{
    char cwd[BUFSIZ];
    git_repository *repo = NULL;

    parse_arguments(argc, argv);
    getcwd(cwd, BUFSIZ);

    git_libgit2_init();

    if(0 == git_repository_open(&repo, cwd)) {
        printf("(git:");
        get_branch_status(repo);
        get_file_status(repo);
        get_stash_status(repo);
        printf(")\n");

        git_repository_free(repo);
    }

    git_libgit2_shutdown();

    return 0;
}
