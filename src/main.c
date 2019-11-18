#include <stdio.h>
#include <getopt.h>
#include <git2.h>
#include <string.h>

#include "functions.h"

#define C_RED  "%%F{red}"
#define C_GRN  "%%F{green}"
#define C_YEL  "%%F{yellow}"
#define C_BLU  "%%F{blue}"
#define C_MAG  "%%F{magenta}"
#define C_CYN  "%%F{cyan}"
#define C_WHT  "%%F{white}"
#define C_RESET "%%f"

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
        "usage: " BINNAME " [<options>]\n"
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



int main(int argc, char **argv)
{
    char cwd[BUFSIZ];
    char directory[BUFSIZ];
    git_repository *repo = NULL;
    struct tree_status tstatus;
    struct branch_status bstatus;
    int stash_count;
    int i;

    parse_arguments(argc, argv);
    getcwd(cwd, BUFSIZ);

    git_libgit2_init();

    if ('/' == cwd[0]) {
        for (i = strlen(cwd); i > 0; i--) {
            if (cwd[i] == '/' || cwd[i] == '\0') {
                strncpy(directory, cwd, i);
                directory[i] = '\0';
                if (0 == git_repository_open(&repo, directory)) {
                    stash_count = get_stash_count(repo);
                    get_branch_status(&bstatus, repo);
                    get_tree_status(&tstatus, repo);
                    printf(
                           C_WHT "(git:%s%s%s" C_RESET C_CYN "%s" C_RESET
                           C_BLU "%s"C_RESET C_YEL "%s" C_RESET C_WHT "%s" C_RESET C_RED "%s" C_RESET C_GRN "%s" C_RESET ")",
                           bstatus.head_name ? bstatus.head_name : "",
                           bstatus.ahead_count ? sigils.ahead : "",
                           bstatus.behind_count ? sigils.behind : "",
                           stash_count ? sigils.stashed : "",
                           tstatus.staged_count ? sigils.staged : "",
                           tstatus.unstaged_count ? sigils.unstaged : "",
                           tstatus.untracked_count ? sigils.untracked : "",
                           tstatus.conflict_count ? sigils.conflicts : "",
                           (!tstatus.staged_count && !tstatus.unstaged_count && !tstatus.untracked_count && !tstatus.conflict_count) ? sigils.clean : ""
                    );

                    if (NULL == bstatus.head_name)
                        free(bstatus.head_name);
                    git_repository_free(repo);

                    break;
                }
            }
        }
    }

    git_libgit2_shutdown();

    return 0;
}
