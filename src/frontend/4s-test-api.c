#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <libgen.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#include <rasqal.h>

#include "../../4store-config.h"
#include "../common/4store.h"
#include "../common/error.h"
#include "../common/gnu-options.h"

#include "../frontend/query.h"
#include "../frontend/import.h"
#include "../frontend/update.h"

static fsp_link *fsplink;

char *data;

int main(int argc, char *argv[])
{
    char *password = NULL;
    char *kb_name = "demo";
    int count = 0, errors = 0;
    char *model_uri = "file:///home/stip/smart-m3/benchmark/yago.n3";
    char *muri;
    muri = (char *)model_uri;

    FILE *fp;


    char *mimetype = "application/rdf+xml";
    fsplink = fsp_open_link(kb_name, password, FS_OPEN_HINT_RW);
    const char *features = fsp_link_features(fsplink);

    fprintf(stderr, "Features: %s\n", features);

    if (fs_import_stream_start(fsplink, muri, mimetype, 1, &count))
    {
        fprintf(stderr, "Failed to start stream parse\n");
        goto teardown;
    }

    fp = fopen("test.rdf", "r");
    fseek(fp, 0, SEEK_END);
    long pos = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    data = malloc(pos);
    fread(data, pos, 1, fp);
    fclose(fp);

    fprintf(stderr, "%s\n", data);

    if ( fs_import_stream_data(fsplink, data, pos) )
    {
        fprintf(stderr, "Failed to start stream parse\n");
        goto teardown;
    }

    fprintf(stderr, "Boo\n");

    if (fs_import_stream_finish(fsplink, &count, &errors))
    {
        fprintf(stderr, "Failed to finish stream parse\n");
        goto teardown;
    }

    fprintf(stderr, "Count: %d Errors: %d\n", count, errors);


    /* Tear down */
    teardown:
    fsp_close_link(fsplink);
    free(data);

}
